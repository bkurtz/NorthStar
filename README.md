NorthStar
===============

NorthStar is a tool that can be used to download sessions and other data from various smart watches by Polar (e.g. M400, probably V800).  Activity sessions are converted to TCX files which can then be used locally or uploaded to a website of your choice (e.g. Strava).

NorthStar was developed leaning heavily on code from <a href="https://www.github.com/profanum429/v800_downloader">V800 Downloader</a> and <a href="https://www.github.com/pcolby/bipolar">Bipolar</a>.  Specifically, the USB communication code is taken almost verbatim from V800 Downloader, and the protobuf templates from Bipolar were exceedingly useful in parsing the resulting data files.  Unlike these original programs, NorthStar is designed to run from the command line, and does not depend on Qt.

## Dependencies
* python (2.7?)
* <a href="https://developers.google.com/protocol-buffers/">Protocol Buffers</a>
* a C++ compiler

## Installation
After installing Protocol Buffers (Mac OS X users may find it convenient to install the `protobuf` package in [Homebrew](http://brew.sh)), you should be able to run `make` at the top level of the source repository.  This will build the python files for parsing data from the watch, as well as compiling the C++ code that takes care of the USB communications.  Once that's done, you can run `./northstar.py --help` to get started.  I haven't made any attempts to set this up to be installed anywhere, so it's likely easiest to just add the source directory to your path, or make a shell alias or type the whole path.

## Options
Options are probaby best documented with the online help: `./northstar.py --help`.  I built in all the options I thought I needed plus a few that I suspect I won't use very much, but it's still extremely basic.  I think my favorite mode will end up being `./northstar.py -m -o ~/polar_data/`, which keeps updates the existing set of files in `~/polar_data/` with any new activities that are present on the watch.  You can even compress the output TCX files with your favorite compression program  and it won't re-download a new copy, assuming the filename only changed by the addition of a `.gz`, `.bz2`, or `.xz`.

If you want to do fancier things or play with the raw data, there are several additional protobuf message formats in the `protobuf/` directory of the source, which you can parse with `protoc` (part of the protocol buffers distribution; see it's documentation for more info on how to use it), or you can use `protoc --decode_raw` to parse raw messages from the watch.  You can always run the compiled USB download program directly (`src/polar_downloader`) to download data from the watch manually, and there are some bonus routines/scripts in the `python/` source directory that can be used for useful things like recursive directory listings or recursive file downloads from the watch.

## Caveats
* In theory, this should work on any platform, however, in practice I work on a Mac and have not yet made the effort to build and test on Linux/Unix.  I think it's mostly a matter of figuring out how to make the C++ makefile pass the right OS flag to the compiler, and then making sure the result actually builds and runs.  I might eventually get around to this, but it's not much of a priority.
* Currently only supports TCX output.  I mostly use the data for upload to Strava, so one format was enough, and I decided to go for TCX because it appears to support inclusion of lap data most easily (though NorthStar doesn't currently implement that).
* Currently quite limited in the range of data it parses.  I don't have any fancy power sensors or anything, so I'm mostly looking at running cadence, sometimes heartrate, and GPS coordinates.
	* I actually even leave the distance out of the TCX file for now, because it seems that Strava is happy to re-calculate distance from GPS points, and the week I was uploading this I _happened_ to have an issue with Polar's web interface exporting really buggy distance measurements.  So far, the data from the watch is usually fine, but since I don't really need it, I decided to leave it out.
	* I don't support multi-sport sessions.  I don't generally _do_ multisport sessions, and I don't even know how to make one on my M400, so it's way too early to think about making the code support those.  If you want support for that and have example data you'd be willing to share, hit me up.

## Credits
* [V800 Downloader](https://www.github.com/profanum429/v800_downloader) for raw USB source code, which is modified somewhat and can be found in the `src/` directory.
* [Bipolar](https://www.github.com/pcolby/bipolar) For all the hints on protobuf message formats
* [rawhid](https://www.pjrc.com/teensy/rawhid.html) V1.0 for OS X is used for USB communication in OS X, as in V800 Downloader

## License
I guess everything is licensed GPL3.  That's what V800 Downloader and Bipolar use, and I'm not inclined to be picky.  Basically, if you make useful changes, I'd really appreciate it if you kick them back to me.
