LDFLAGS=-framework Foundation -framework IOKit
CPPFLAGS=-g

polar_downloader: polar_downloader.cpp v800usb.o rawhid_api.o native_usb.o QByteArray.o

test: test.cpp QByteArray.o

.PHONY: clean
clean:
	rm -rf *.dSYM *.o polar_downloader test