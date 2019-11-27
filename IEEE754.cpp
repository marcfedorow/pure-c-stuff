#include <stdio.h>
#include <stdint.h>
#define DOUBLE 1 // 0 if you want 32-bit float
#if DOUBLE
	#define SIGN_BITS 1
	#define EXP_BITS 11
	#define MAN_BITS 52
	#define TOT_BITS 64
	#define IEEE754_t double
	#define UINT_t uint64_t
	#define INT_t int64_t
#else
	#define SIGN_BITS 1
	#define EXP_BITS 8
	#define MAN_BITS 23
	#define TOT_BITS 32
	#define IEEE754_t float
	#define UINT_t uint32_t
	#define INT_t int32_t
#endif

typedef union {
	IEEE754_t f;
	UINT_t i;
	struct {
		UINT_t man : MAN_BITS, exp : EXP_BITS, sign : SIGN_BITS;
	} s;
} IEEE_t;


IEEE754_t zero = 0;
INT_t one = 1;

void print_binary(unsigned number, unsigned bits = TOT_BITS){
	do printf("%d", number&(one<<(--bits))?1:0);
	while(bits);
}

void test(IEEE_t &u){
	printf("%c %*.*s %-*s\n", 'S', EXP_BITS, EXP_BITS, "EXPONENT", MAN_BITS, "MANTISSA");
	printf("%d ", u.s.sign); print_binary(u.s.exp, EXP_BITS); printf(" "); print_binary(u.s.man, MAN_BITS); printf("\n");
	char sign = u.s.sign?'-':'+';
	int exp = u.s.exp - (one<<(EXP_BITS-1)) + 1;
	
	bool denormal = !u.s.exp | !(~u.s.exp & ((one<<EXP_BITS)-1)); //u.s.exp == 0 || u.s.exp == (one<<EXP_BITS) - 1;
	if (denormal){
		printf("Denormal number\n");
		if (u.s.exp){
			if (!u.s.man){ //mantissa == 0
				printf("%cInfinity\n", sign);
			} else {
				printf("Not a number\n");
			}
		} else {
			printf("%c1 * 2^(%d) * ", sign, exp + 1);
			printf(DOUBLE?"%llf\n":"%f\n", (IEEE754_t)u.s.man / (one<<MAN_BITS));

			printf("%c1 * 2^(%d) * ", sign, exp + 1 - MAN_BITS);
			printf(DOUBLE?"%lld\n":"%d\n", u.s.man);
		}
	} else {
		INT_t man = (one<<MAN_BITS) | u.s.man;

		printf("%c1 * 2^(%d) * ", sign, exp);
		printf(DOUBLE?"%llf\n":"%f\n", (IEEE754_t)man / (one<<MAN_BITS));

		printf("%c1 * 2^(%d) * ", sign, exp - MAN_BITS);
		printf(DOUBLE?"%lld\n":"%d\n", man);
	}
	printf("\n\n");
}

int main(){
	IEEE754_t f[] = {1/zero, -1/zero, 0/zero, 0, 1, -1, 2, -2, 3, -3};
	IEEE_t t;
	for (INT_t i = 0; i < sizeof(f)/sizeof(f[0]); ++i){
		t.f = f[i];
		test(t);
	}
	return 0;
}
