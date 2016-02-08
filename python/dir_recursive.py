#!/usr/bin/python

# Usage:
# dir_recursive.py /


import directory_pb2
import sys
import subprocess
from os.path import isfile, join, isdir, dirname, abspath

def date_to_string(d):
	if(d.isUTC):
		tz = 'Z'
	else:
		tz = "%+i" % (d.tz_minutes/60)
	return "%04i-%02i-%02i %02i:%02i:%02i %s" % (d.date.year, d.date.month, d.date.day, d.time.hour, d.time.minute, d.time.second, tz)

if len(sys.argv) > 1:
	fn = sys.argv[1]
else:
	sys.exit(1)

# determine the location of the polar_downloader binary
polar_downloader = join(dirname(dirname(abspath(__file__))), 'src', 'polar_downloader')

def recursive_download_and_print(path):
	data = subprocess.check_output([polar_downloader, path]);
	
	directory = directory_pb2.Directory()
	directory.ParseFromString(data)
	
	for item in directory.item:
		print("%s%s	%i" % (path, item.name, item.size))
		print("  created:  %s" % (date_to_string(item.created)))
		print("  modified: %s" % (date_to_string(item.modified)))
		print("  long ago: %s" % (date_to_string(item.d3)))
	
	for item in directory.item:
		if item.name.endswith('/'):
			recursive_download_and_print(path + item.name)

recursive_download_and_print(fn)
