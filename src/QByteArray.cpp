#include "QByteArray.h"

QByteArray::QByteArray () {
	bytes = NULL;
	n = 0;
}
QByteArray::~QByteArray () {
//no idea why, but this was causing crashes, so it's going away for now
	/*if(bytes!=NULL) {
		free(bytes);
		bytes = NULL;
	}*/
}

int QByteArray::length() { return n; }
void QByteArray::resize(size_t length) {
	if(length != n) {
		bytes = (uint8_t *)realloc(bytes, length);
		n = length;
	}
} 
void QByteArray::fill(char v) {
	int i;
	for(i=0; i<n; i++) {
		bytes[i] = v;
	}
}
void QByteArray::prepend(QByteArray otherpacket) {
	prepend((uint8_t *)otherpacket.constData(), otherpacket.length());
}
void QByteArray::prepend(uint8_t * data, size_t length) {
	size_t orig_n = n;
	resize(n+length);
	memmove(bytes+length, bytes, orig_n);
	memcpy(bytes, data, length);
}
void QByteArray::append(QByteArray otherpacket) {
	append((uint8_t *)otherpacket.constData(), otherpacket.length());
}
void QByteArray::append(uint8_t * data, size_t length) {
	size_t orig_n = n;
	resize(n+length);
	memcpy(bytes+orig_n, data, length);
}

void * QByteArray::constData() {
	return (void *) bytes;
}

void QByteArray::clear() {
	bytes = NULL;
	n = 0;
}

void QByteArray::remove(size_t start, size_t length) {
	if(start+length>n) {
		if(start > n) {
			return;
		}
		length = n-start;
	}
	memmove(bytes+start, bytes+start+length, n-start-length);
	bytes = (uint8_t *)realloc(bytes,n-length);
	n -= length;
}

uint8_t& QByteArray::operator[] (const int nIndex) {
	if(nIndex >= n) resize(nIndex+1);
	return bytes[nIndex];
}

int QByteArray::write(FILE * fh) {
	size_t count;
	count = fwrite(bytes, sizeof(uint8_t), n, fh);
	return count==n;
}

void QByteArray::print_hex(FILE * fh) {
	size_t i;
	for(i=0; i<n; i++) {
		fprintf(fh, "%02x", bytes[i]);
		if(i%0x10==0x10-1) {
			fputc('\n',fh);
		} else if(i%0x4==0x4-1) {
			fputc(' ', fh);
		}
	}
	fprintf(fh,"\n");
}
