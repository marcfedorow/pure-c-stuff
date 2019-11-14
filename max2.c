#include <stdio.h>
#define max(a, b) (((a-b)>>(sizeof(a)*8-1))&1)*b + ((((a-b)>>(sizeof(a)*8-1))&1)^1)*a
int main() {
	int a, b;
	return scanf("%d %d", &a, &b), printf("%d\n", max(a, b)), 0;
}
