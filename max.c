#include <stdarg.h>
#include <stdio.h>
#define MAX(...) max(__VA_ARGS__, -1./0)

float max(float f, ...) {
	va_list valist;
	va_start(valist, f);
	float max = -1./0, cur = f;
	do {
		if (max < cur) max = cur;
		cur = va_arg(valist, double); //float converts to double when goes through ...
	} while (cur > (-1./0));
	va_end(valist);
	return max;
}

int main() {
	float m = MAX(1, 2., 3.); //should be float. Corrupts if integer
	printf("max = %f", m);
	return 0;
}
