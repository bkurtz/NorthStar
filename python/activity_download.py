#!/usr/bin/python

# Usage:
# dir_recursive.py /


import directory_pb2
import route_pb2
import sys
import subprocess
import re

def date_to_string(d):
	if(d.isUTC):
		tz = 'Z'
	else:
		tz = "%+i" % (d.tz_minutes/60)
	return "%04i-%02i-%02i %02i:%02i:%02i %s" % (d.date.year, d.date.month, d.date.day, d.time.hour, d.time.minute, d.time.second, tz)

# determine the location of the polar_downloader binary
polar_downloader = "/Users/ben/Downloads/polar/v800_downloader/src/usb/polar_downloader"

def download_directory(path):
	if not path.endswith('/'):
		raise ValueError('Directory paths must end with a "/"')
	
	data = subprocess.check_output([polar_downloader, path])
	
	directory = directory_pb2.Directory()
	directory.ParseFromString(data)
	
	return directory

def download_activity_info(path):
	if not re.search('/U/\d/\d{8}/E/\d{6}/', path):
		raise ValueError('Path does not match expected format for an activity');
	
	data = subprocess.check_output([polar_downloader, path+'TSESS.BPB'])
	
	activity = route_pb2.Activity()
	activity.ParseFromString(data)
	
	return activity

def recursive_download_and_print(path):
	directory = download_directory(path)
	
	for item in directory.item:
		print("%s%s	%i" % (path, item.name, item.size))
		print("  created:  %s" % (date_to_string(item.created)))
		print("  modified: %s" % (date_to_string(item.modified)))
		print("  long ago: %s" % (date_to_string(item.d3)))
	
	for item in directory.item:
		if item.name.endswith('/'):
			recursive_download_and_print(path + item.name)

def get_activity_directories():
	user_path = '/U/0/'
	user_dir = download_directory(user_path)
	days = []
	
	for item in user_dir.item:
		if re.search('^\d{8}/$', item.name):
			days.append(item.name)
	
	activities = []
	for day in days:
		day_path = user_path + day + 'E/'
		day_dir = download_directory(day_path)
		for item in day_dir.item:
			if re.search('^\d{6}/$', item.name):
				this_act = {'path': day_path + item.name}
				this_act["info"] = download_activity_info(this_act["path"])
				activities.append(this_act)
	
	for a in activities:
		act_dir = download_directory(a["path"] + '00/')
		has_route = False
		has_samples = False
		for item in act_dir.item:
			if item.name == 'SAMPLES.GZB':
				has_samples = True
			elif item.name == 'ROUTE.GZB':
				has_route = True
		a["stats_only"] = not (has_samples and has_route)
	
	return activities

def print_activity_list(activities):
	i = 1
	for a in activities:
		a_name = a["info"].sport.name[0]
		a_time = a["info"].start_time
		a_dist = a["info"].distance/1000;
		if a["stats_only"]:
			line_symbol = '*'
		else:
			line_symbol = "%i" % (i)
			i += 1
		print("%s\t%s\t%s, %.1f km" % (line_symbol, date_to_string(a_time), a_name, a_dist))

print("Fetching activity list")
print_activity_list(get_activity_directories())
