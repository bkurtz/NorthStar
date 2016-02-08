#!/usr/bin/python

# Usage:
# ./activity_download

from __future__ import print_function
import directory_pb2
import route_pb2
import sys
import subprocess
import re
import activity_to_tcx
import argparse
# from os import listdir
from os import mkdir
from os.path import isfile, join, isdir, dirname, abspath

def date_to_string(d):
	if(d.isUTC):
		tz = 'Z'
	else:
		tz = "%+i" % (d.tz_minutes/60)
	return "%04i-%02i-%02i %02i:%02i:%02i %s" % (d.date.year, d.date.month, d.date.day, d.time.hour, d.time.minute, d.time.second, tz)

# determine the location of the polar_downloader binary
polar_downloader = dirname(abspath(__file__))
if polar_downloader.endswith("python"):
	polar_downloader = dirname(polar_downloader)
polar_downloader = join(polar_downloader, 'src', 'polar_downloader')

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
	for item in dirlist.item:
		if item.name.endswith('/'):
			if not isdir(out_root): mkdir(out_root)
			recursive_download(watch_path + item.name, join(out_root, item.name[:-1]), skip)
		elif item.name not in skip:
			msg("Fetching %s..." % (watch_path+item.name))
			data = subprocess.check_output([polar_downloader, watch_path+item.name])
			with open(join(out_root, item.name), 'w') as rfile:
				rfile.write(data)

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

def print_activity_list(activities, quiet=False):
	i = 1
	for a in activities:
		a_name = a["info"].sport.name[0]
		a_time = a["info"].start_time
		a_dist = a["info"].distance/1000;
		if a["stats_only"]:
			line_symbol = '*'
			if quiet: continue
		else:
			line_symbol = "%i" % (i)
			i += 1
		# NOTE: this one goes to stdout, not stderr
		print("%s\t%s\t%s, %.1f km" % (line_symbol, date_to_string(a_time), a_name, a_dist))

def get_activity_date(activity):
	return activity_to_tcx.pb_date_to_Date(activity["info"].start_time)

def fn_for_activity(activity):
	d = get_activity_date(activity)
	return d.strftime('%Y-%m-%d_%H-%M-%S.tcx')

# msg is used to print messages that are omitted when -q is passed
# calling it once with quiet=False changes the behavior for all subsequent calls
def msg(m, quiet=None):
	if quiet is not None:
		msg.quiet = quiet
	if not msg.quiet:
		print(m, file=sys.stderr)
msg.quiet = False

def download_activity_data(activity, quiet=False, save_raw=False):
	# generate paths to downlaod from
	s_path = activity['path'] + '00/SAMPLES.GZB'
	r_path = activity['path'] + '00/ROUTE.GZB'
	# download them from the watch
	msg("Downloading Route File...")
	rdata = subprocess.check_output([polar_downloader, r_path]);
	msg("Downloading Samples File...")
	sdata = subprocess.check_output([polar_downloader, s_path]);
	# save raw output if requested
	# do this before parsing so as to have data to work with in the event that parsing fails
	if save_raw:
		msg("Saving raw activity data...")
		tmpdir = "/tmp/NorthStar"
		actdir = join(tmpdir, '00')
		if not isdir(tmpdir): mkdir(tmpdir)
		if not isdir(actdir): mkdir(actdir)
		with open(join(actdir,'SAMPLES.GZB'), 'w') as rfile:
			rfile.write(sdata)
		with open(join(actdir,'ROUTE.GZB'), 'w') as rfile:
			rfile.write(rdata)
		recursive_download(activity['path'], tmpdir, skip=['SAMPLES.GZB', 'ROUTE.GZB'])
	# parse protocol buffers
	route = activity_to_tcx.parse_route(rdata)
	samp = activity_to_tcx.parse_samples(sdata)
	return (route, samp)

parser = argparse.ArgumentParser(description="Download activity files from Polar watch and save as TCX")
parser.add_argument("-l", "--list", action="store_true", help="list available activities")
parser.add_argument("-n", "--download-number", help="download one or more numbered activities from the list (negative goes from end; e.g. -1 is last item in list)", type=int)
parser.add_argument("-m", "--missing", action="store_true", help="Download all activities not present in the output directory (see -o)")
parser.add_argument("-i", "--interactive", action="store_true", help="Download interactively (default if none of -l, -n, -m specified)");
parser.add_argument("-o", "--output", help="Output directory. Defaults to current directory if unspecified.  Files are named as yyyy-mm-dd_HH-MM-SS.tcx")
parser.add_argument("-q", "--quiet", action="store_true")
parser.add_argument("-r", "--raw", action="store_true", help="Save raw files for each activity in /tmp/NorthStar/ for debugging.  Only latest files are saved.")
parser.add_argument("outfile", help="File to write for downloading a single activity with -n.  Default is stdout unless -o is specified.", type=argparse.FileType('w'), nargs='?')
args = parser.parse_args()

# do some extra arg checking
outdir = './'
if args.output:
	outdir = args.output
	if not isdir(args.output):
		sys.exit("output directory does not exist")
be_interactive = (not (args.download_number or args.missing or args.list)) or args.interactive

# download and sort the activity list
msg("Fetching activity list", args.quiet)
activity_list = get_activity_directories()
activity_list.sort(key=get_activity_date)

if be_interactive or args.list:
	print_activity_list(activity_list, quiet=args.quiet)

# filter activity list to remove activities without GPS data
activity_list = [ a for a in activity_list if not a["stats_only"] ]

# implement non-interactive modes
if args.download_number:
	# handle positive/negative N
	if args.download_number > 0:
		N = args.download_number - 1;
	else:
		N = args.download_number
	# download the files and convert to tcx
	(route, samples) = download_activity_data(activity_list[N], quiet=args.quiet, save_raw=args.raw)
	xml_str = activity_to_tcx.track_to_xml(activity_list[N]["info"], route, samples)
	# write to file
	if args.outfile or not args.output:
		if not args.outfile:
			args.outfile = sys.stdout;
		args.outfile.write(xml_str)
	else:
		with open(outdir + '/' + fn_for_activity(activity_list[N]), 'w') as tcxfile:
			tcxfile.write(xml_str)
if args.missing:
	# pare down to just activities that don't have a file already
	missing_activities = []
	for a in activity_list:
		fn = join(outdir, fn_for_activity(a))
		# go ahead and check compressed filenames too...
		if not (isfile(fn) or isfile(fn+'.gz') or isfile(fn+'.xz') or isfile(fn+'.bz2')):
			missing_activities.append(a)
	msg("%i Activities to download\n" % (len(missing_activities)))
	for a in missing_activities:
		fn = join(outdir, fn_for_activity(a))
		(route, samples) = download_activity_data(a, quiet=args.quiet)
		msg("Saving to file %s..." % (fn))
		xml_str = activity_to_tcx.track_to_xml(a['info'], route, samples)
		with open(fn, 'w') as tcxfile:
			tcxfile.write(xml_str)
	msg("Done!")

if not be_interactive:
	sys.exit(0)

# that leaves us to interactive mode, where we prompt for a number (or q), download, then repeat
while True:
	N = raw_input("Activity number to download ('q' or blank to quit)")
	try:
		N = int(N)
	except ValueError:
		print("bye")
		sys.exit(0)
	# get data
	(route, samples) = download_activity_data(activity_list[N-1])
	xml_str = activity_to_tcx.track_to_xml(activity_list[N-1]["info"], route, samples)
	# write file
	fn = fn_for_activity(activity_list[N-1])
	with open(outdir + '/' + fn, 'w') as tcxfile:
		tcxfile.write(xml_str)
	# repeat; don't think we actually need to print the list again

