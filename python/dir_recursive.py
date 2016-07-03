#!/usr/bin/python

# Usage:
# dir_recursive.py /


from __future__ import print_function
import directory_pb2
import sys
import subprocess
from os import mkdir
from os.path import isfile, join, isdir, dirname, abspath

# msg is used to print messages that are omitted when -q is passed
# calling it once with quiet=False changes the behavior for all subsequent calls
def msg(m, quiet=None):
	if quiet is not None:
		msg.quiet = quiet
	if not msg.quiet:
		print(m, file=sys.stderr)
msg.quiet = False

def date_to_string(d):
	if(d.isUTC):
		tz = 'Z'
	else:
		tz = "%+i" % (d.tz_minutes/60)
	return "%04i-%02i-%02i %02i:%02i:%02i %s" % (d.date.year, d.date.month, d.date.day, d.time.hour, d.time.minute, d.time.second, tz)

# determine the location of the polar_downloader binary
polar_downloader = join(dirname(dirname(abspath(__file__))), 'src', 'polar_downloader')

def download_directory(path):
	if not path.endswith('/'):
		raise ValueError('Directory paths must end with a "/"')
	
	data = subprocess.check_output([polar_downloader, path])
	
	directory = directory_pb2.Directory()
	directory.ParseFromString(data)
	
	return directory

def recursive_listing(path):
	directory = download_directory(path)
	
	for item in directory.item:
		print("%s%s	%i" % (path, item.name, item.size))
		print("  created:  %s" % (date_to_string(item.created)))
		print("  modified: %s" % (date_to_string(item.modified)))
		print("  long ago: %s" % (date_to_string(item.d3)))
	
	for item in directory.item:
		if item.name.endswith('/'):
			recursive_listing(path + item.name)

def recursive_download(watch_path, out_root, skip=[]):
	dirlist = download_directory(watch_path)
	if not isdir(out_root): mkdir(out_root)
	for item in dirlist.item:
		if item.name.endswith('/'):
			recursive_download(watch_path + item.name, join(out_root, item.name[:-1]), skip)
		elif item.name not in skip:
			msg("Fetching %s..." % (watch_path+item.name))
			data = subprocess.check_output([polar_downloader, watch_path+item.name])
			with open(join(out_root, item.name), 'w') as rfile:
				rfile.write(data)

if len(sys.argv) > 1:
	fn = sys.argv[1]
	if len(sys.argv) > 2:
		outroot = sys.argv[2]
		do_download = True
	else:
		do_download = False
else:
	sys.exit(1)

if do_download:
	recursive_download(fn, outroot)
else:
	recursive_listing(fn)
