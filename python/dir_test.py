#!/usr/bin/python

# Usage:
# dir_test.py dir_file
# polar_downloader / 2>/dev/null | dir_test.py

import directory_pb2
import sys

def date_to_string(d):
	if(d.isUTC):
		tz = 'Z'
	else:
		tz = "%+i" % (d.tz_minutes/60)
	return "%04i-%02i-%02i %02i:%02i:%02i %s" % (d.date.year, d.date.month, d.date.day, d.time.hour, d.time.minute, d.time.second, tz)

if len(sys.argv) > 1:
	fn = sys.argv[1]
	with open(fn, 'r') as myfile:
		data=myfile.read()
else:
	data=sys.stdin.read()

directory = directory_pb2.Directory()
directory.ParseFromString(data)

for item in directory.item:
	print("%s	%i" % (item.name, item.size))
	print("  created:  %s" % (date_to_string(item.created)))
	print("  modified: %s" % (date_to_string(item.modified)))
	print("  long ago: %s" % (date_to_string(item.d3)))
