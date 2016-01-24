#include "QByteArray.h"
#include <string.h>
#include <stdio.h>

int dotc(int i);
int tc1();
int tc2();
int tc3();
int tc4();
int tc5();
int tc6();
int tc7();
int tc8();
int tc9();
int tc10();
int tc11();
int tc12();
int tc13();

int main () {
	int failures = 0;
	int N = 13;
	int i;

	for(i=1; i<=N; i++) {
		printf("Running test case %i: ",i);
		if(dotc(i)) {
			printf("success\n");
		} else {
			printf("FAIL!!\n");
			failures++;
		}
	}

	return failures;
}

int dotc(int i) {
	switch(i) {
	case 1:
		return tc1();
	case 2:
		return tc2();
	case 3:
		return tc3();
	case 4:
		return tc4();
	case 5:
		return tc5();
	case 6:
		return tc6();
	case 7:
		return tc7();
	case 8:
		return tc8();
	case 9:
		return tc9();
	case 10:
		return tc10();
	case 11:
		return tc11();
	case 12:
		return tc12();
	case 13:
		return tc13();
	default:
		printf("unknown test case\n");
		return 0;
	}
}

int tc1() {
	QByteArray a,b;
	a.append((uint8_t *)"abc", 4);
	b.append((uint8_t *)"abc", 3);
	return (strcmp("abc", (char*) a.constData())==0 && strncmp("abc", (char*) b.constData(), 3)==0) && a.length()==4 && b.length()==3;
}

int tc2() {
	QByteArray a,b;
	a.append((uint8_t *)"abc", 4);
	b.append((uint8_t *)"abc", 3);
	a.append((uint8_t *)"def", 4);
	b.append((uint8_t *)"def", 4);
	return (strcmp("abc", (char*) a.constData())==0 && strncmp("abcdef", (char*) b.constData(), 6)==0) && a.length()==8 && b.length()==7;
}

int tc3() {
	QByteArray a,b;
	a.append((uint8_t *)"abc", 4);
	b.append((uint8_t *)"abc", 3);
	a.prepend((uint8_t *)"def", 4);
	b.prepend((uint8_t *)"def", 3);
	return (strcmp("def", (char*) a.constData())==0 && strncmp("defabc", (char*) b.constData(), 6)==0) && a.length()==8 && b.length()==6;
}

int tc4() {
	QByteArray a,b;
	a.append((uint8_t *)"abc", 4);
	b.append((uint8_t *)"def", 3);
	a.prepend(b);
	return (strcmp("defabc", (char*) a.constData())==0) && (strncmp("def", (char*) b.constData(), 3)==0) && a.length()==7;
}

int tc5() {
	QByteArray a,b;
	a.append((uint8_t *)"abcdef", 7);
	a.remove(1,2);
	return strcmp("adef", (char*) a.constData())==0 && a.length()==5;
}

int tc6() {
	QByteArray a,b;
	a.append((uint8_t *)"abc", 4);
	b.append((uint8_t *)"def", 3);
	a.clear();
	if(a.length()) { return 0; }
	a.append(b);
	return (strncmp("def", (char*) a.constData(), 3)==0 && a.length()==3);
}

int tc7() {
	QByteArray a;
	a.append((uint8_t *)"abcdef", 7);
	a.resize(5);
	return (strncmp("abcde", (char*) a.constData(), 5)==0 && a.length()==5);
}

int tc8() {
	QByteArray a;
	a.append((uint8_t *)"abcdef", 7);
	a.resize(10);
	return (strncmp("abcdef", (char*) a.constData(), 7)==0 && a.length()==10);
}

int tc9() {
	QByteArray a;
	a.resize(10);
	if(a.length()!=10) { return 0; }
	a.append((uint8_t *)"xyza", 5);
	return strncmp("xyza", (char*)a.constData()+10, 5)==0 && a.length()==15;
}

int tc10() {
	QByteArray a;
	a.resize(5);
	a.fill('v');
	return strncmp("vvvvv", (char*)a.constData(), 5)==0 && a.length()==5;
}

int tc11() {
	QByteArray a;
	a[0] = 'v'; a[1] = 'a'; a[2] = 'n'; a[3] = 'x'; a[4] = '\0';
	return a.length()==5 && strncmp("vanx", (char*)a.constData(), 5)==0;
}

int tc12() {
	QByteArray a;
	a[6] = 'h';
	return a.length()==7 && strncmp("h", (char*)a.constData()+6, 1)==0;
}

int tc13() {
	QByteArray a;
	a.append((uint8_t *)"I'm an elf", 11);
	return a[2]=='m' && a[9]=='f' && a[10]=='\0';
}
