#include <stdio.h>

#define bpick ((a - b) >> (sizeof(a) * 8 - 1))
#define max(a, b) bpick & b | ~bpick & a

int main() {
	int a, b;
	return scanf("%d %d", &a, &b), printf("%d\n", max(a, b)), 0;
}
