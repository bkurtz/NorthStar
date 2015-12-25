#include "QByteArray.h"

QByteArray::QByteArray () {
	bytes = NULL;
	n = 0;
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
