# Detect Linux vs Mac OS
ifeq ($(wildcard /System/),)
    OS = Linux
else
    OS = MacOSX
endif


CPPFLAGS += -g
ifeq ($(OS),MacOSX)
	LDFLAGS += -framework Foundation -framework IOKit
	EXTRA_OFILE=rawhid_api.o
	CPPFLAGS += -DQ_OS_MAC
else
	LUSB = libusb-1.0
	LIBS += $(shell pkg-config --libs $(LUSB))
	CPPFLAGS += -DQ_OS_LINUX $(shell pkg-config --cflags $(LUSB))
endif

polar_downloader: polar_downloader.cpp v800usb.o $(EXTRA_OFILE) native_usb.o QByteArray.o $(LIBS)

test: test.cpp QByteArray.o

.PHONY: clean
clean:
	rm -rf *.dSYM *.o polar_downloader test
