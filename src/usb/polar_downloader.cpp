#include "v800usb.h"

int main(int argc, char * argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage:\n\t%s file_to_fetch\n", argv[0]);
		return -1;
	}
	fprintf(stderr, "download file %s\n", argv[1]);

	V800usb watch (M400);
	if(!watch.start()) {
		fprintf(stderr, "couldn't connect to watch\n");
		return 1;
	}

	watch.get_file(argv[1]);

	return 0;
}
