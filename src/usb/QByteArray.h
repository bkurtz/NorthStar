// Quick replacement class for QT's QByteArray so that we don't need a QT requirement...
#ifndef QBYTEARRAY_H
#define QBYTEARRAY_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

class QByteArray
{
public:
	QByteArray ();
	int length();
	void resize(size_t length); 
	void fill(char v);
	void prepend(QByteArray otherpacket);
	void prepend(uint8_t * data, size_t length);
	void append(QByteArray otherpacket);
	void append(uint8_t * data, size_t length);

	void * constData(); // pointer to current buffer

private:
	uint8_t * bytes;
	size_t n;
};
#endif // QBYTEARRAY_H
