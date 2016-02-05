#!/usr/bin/python

import directory_pb2
import route_pb2
import sys
import subprocess
import re
import zlib
import gzip
import datetime

def parse_route(routedata):
	route = route_pb2.Route()
	route.ParseFromString(zlib.decompress(routedata, 16+zlib.MAX_WBITS))
	return route

def parse_samples(sampledata):
	# decompress and parse input
	samples = route_pb2.Samples()
	samples.ParseFromString(zlib.decompress(sampledata, 16+zlib.MAX_WBITS))
	
	# look for offline sensor periods and set their values to 'None' accordingly
	for offline in samples.heartrate_offline:
		for i in range(offline.start_index,offline.stop_index+1): samples.heartrate[i] = -99999
	for offline in samples.altitude_offline:
		for i in range(offline.start_index,offline.stop_index+1): samples.altitude[i] = -99999
	for offline in samples.cadence_offline:
		for i in range(offline.start_index,offline.stop_index+1): samples.cadence[i] = -99999
	
	return samples

def pb_date_to_Date(pbdate):
	tz = None # seems like ignoring the timezone and doing "everything is UTC" works okay for now?  apparently python is stupid and needs a third-party library or custom classes to do timezones properly?  WTF.
	t = datetime.datetime(pbdate.date.year, pbdate.date.month, pbdate.date.day, pbdate.time.hour, pbdate.time.minute, pbdate.time.second, pbdate.time.millisecond*1000, tz)
	if (not pbdate.isUTC) and (pbdate.tz_minutes != 0):
		t += datetime.timedelta(minutes = -pbdate.tz_minutes)
	return t;

def tcx_header():
	return '<?xml version="1.0" encoding="UTF-8"?>\n<TrainingCenterDatabase xmlns="http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2">\n<Activities>\n'

def tcx_footer():
	return '</Activities>\n<Author><Name>NorthStar</Name></Author>\n</TrainingCenterDatabase>\n'

def tcx_activity_header(tsess):
	t = pb_date_to_Date(tsess.start_time)
	t = t.strftime('%Y-%m-%dT%H:%M:%S.%f') # there's probably a clever way to put this all into one format string below, but I don't feel like figuring it out right now...
	return '<Activity Sport="%s"><Id>%sZ</Id>\n' % (tsess.sport.name[0], t)

def tcx_activity_footer(tsess):
	return '<Training><Name>%s</Name></Training>\n<Creator xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="Device_t"><Name>%s</Name></Creator>\n</Activity>\n' % (tsess.sport.name[0], tsess.device_model)

def track_to_xml(activity, route, samples):
	start = pb_date_to_Date(route.timestamp)
	xml = tcx_header() + tcx_activity_header(activity);
	xml += '<Lap><Track>\n' # TODO: improve lap support
	for i, duration in enumerate(route.duration):
		time = start + datetime.timedelta(milliseconds = duration)
		if time.microsecond == 0:
			ms_str = '';
		else:
			ms_str = '.%03i' % (time.microsecond/1000)
		xml += '<Trackpoint><Time>%s%sZ</Time>\n<Position><LatitudeDegrees>%.8f</LatitudeDegrees><LongitudeDegrees>%.8f</LongitudeDegrees></Position>\n' % (time.strftime('%Y-%m-%dT%H:%M:%S'), ms_str, route.latitude[i], route.longitude[i])
		# this would be filtered altitude from the samples file, but I don't like it
		# if samples.altitude[i] != -99999:
		# 	xml += '<AltitudeMeters>%.3f</AltitudeMeters>\n' % samples.altitude[i]
		xml += '<AltitudeMeters>%.3f</AltitudeMeters>\n' % route.altitude[i]
		# this would be distance, but I don't actually like distance either; strava will recalculate it
		# xml += '<DistanceMeters>%.4f</DistanceMeters>\n' % samples.distance[i]
		if samples.heartrate[i] != -99999:
			xml += '<HeartRateBpm><Value>%i</Value></HeartRateBpm>\n' % samples.heartrate[i]
		if samples.cadence[i] != -99999:
			xml += '<Cadence>%i</Cadence>\n' % samples.cadence[i]
		xml += '</Trackpoint>\n'
	xml += '</Track></Lap>\n' # TODO: improve lap support
	xml += tcx_activity_footer(activity) + tcx_footer()
	return xml

if __name__ == "__main__":
	if len(sys.argv) > 2:
		fn = sys.argv[1]
		if not fn.endswith('/'):
			fn += '/'
		with open(fn + 'ROUTE.GZB', 'r') as myfile:
			routedata = myfile.read()
		with open(fn + 'SAMPLES.GZB', 'r') as myfile:
			sampledata = myfile.read()
		with open(sys.argv[2], 'r') as myfile:
			tsessdata = myfile.read()
	else:
		print("Give me a data directory and tsess file to work with!")
		sys.exit(1)
	
	route = parse_route(routedata)
	samples = parse_samples(sampledata)
	tsess = route_pb2.Activity()
	tsess.ParseFromString(tsessdata)
	
	xml_str = track_to_xml(tsess, route, samples)
	print(xml_str)
