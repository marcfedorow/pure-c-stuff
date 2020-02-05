#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<time.h>
typedef unsigned char test_t; //if yout block is >1 byte use the other type
#define bitsize (sizeof(test_t) * 8)

inline int pow2(int p){
	return 1 << p;
}

void freq_test(test_t* buffer, int len){
	printf("Freq test:\n");
	int count = 0;
	for (int j = 0; j < len; ++j){
		for (test_t temp = buffer[j]; temp; ++count)
			temp &= (temp - 1);
	}
	printf("%d ones vs %d zeros\n", count, len * sizeof(buffer[0]) * 8 - count);
	float f = 2. / sqrt((float)len * 8) * (count - len * (sizeof(buffer[0]) * 8) / 2);
	float t1 = -3, t2 = 3; //2.5 to 3
	printf("f = %f\nFreq test %s\n\n", f, ((f >= t1) && (f <= t2)) ? "passed" : "failed");
}

void seq_test(test_t* buffer, int len, int len_of_block = 5, bool mode = 0){
	printf("Seq test:\n");
	int current;
	int pow2L = pow2(len_of_block);
	int* freqs = (int*)calloc(pow2L, sizeof(int));
	for (int ptr = 0; ptr < len * bitsize - len_of_block; ptr += mode ? len_of_block : 1){
		current = 0;
		for (int i = len_of_block - 1; i >= 0; --i){
			current *= 2;
			if ((buffer[(ptr + i) / bitsize] / pow2((ptr + i) % bitsize) % 2)) ++current;
		}
		++freqs[current];
	}
	for (int i = 0; i < pow2L; ++i){
		printf("%d: %d times\n", i, freqs[i]);
	}
	int sum = 0;
	int exp = len * bitsize / (mode? len_of_block: 1) / pow2L;
	for (int i = 0; i < pow2L; ++i){
		sum += (freqs[i] - exp) * (freqs[i] - exp);
	}
	float f = (float)sum / exp;
	float sigma = sqrt((float)2 * pow2L - 1);
	float t1 = (float)pow2L - 1 - 2 * sigma;
	float t2 = (float)pow2L - 1 + 2 * sigma;
	printf("exp = %d\nt1 = %f, t2 = %f\n", exp, t1, t2);
	printf("f = %f\nSeq test %s\n\n", f, (f >= t1) && (f <= t2) ? "passed" : "failed");
}

void ser_test(test_t* buffer, int len, int len_of_block = 10){
	printf("Ser test:\n");
	bool cur_bit = buffer[0] / pow2(bitsize - 1);
	int cur;
	int* ones = (int*)calloc(len_of_block, sizeof(int));
	int* zeros = (int*)calloc(len_of_block, sizeof(int));
	int overflow = 0;
	for (int i = 0; i < len * bitsize - len_of_block; ++i){
		for (int j = 0; j < len_of_block; ++j){
			if ((buffer[i / bitsize] / pow2(i % bitsize) % 2) != cur_bit){
				cur_bit? ++ones[j] : ++zeros[j];
				cur_bit = !cur_bit;
				break;
			} else if (j == len_of_block - 1) cur_bit? ++ones[j] : ++zeros[j];
			++i;
		}
	}
	
	for (int i = 0; i < len_of_block; ++i){
		printf("zeros[%d] = %d; ones[%d] = %d\n", i+1, zeros[i], i+1, ones[i]);
	}
	float f = 0;
	for (int i = 0; i < len_of_block; ++i){
		f += (float) (zeros[i] - (float)len * bitsize / pow2(i+3)) * (zeros[i] - (float)len * bitsize / pow2(i+3)) / ((float)len * bitsize / pow2(i+2));
		f += (float) ( ones[i] - (float)len * bitsize / pow2(i+3)) * ( ones[i] - (float)len * bitsize / pow2(i+3)) / ((float)len * bitsize / pow2(i+2));
	}
	
	float sigma = sqrt((float)2 * len_of_block);
	float t1 = len_of_block - 2 * sigma;
	float t2 = len_of_block + 2 * sigma;
	printf("t1 = %f, t2 = %f\n", t1, t2);
	printf("f = %f\nSer test %s\n\n", f, (f >= t1) && (f <= t2) ? "passed" : "failed");
}

void ac_test(test_t* buffer, int len, int max_tau = 10){
	printf("Autocorr test:\n");
	int count;
	int flag = 0;
	float f, t1 = -3, t2 = 3;
	for (int tau = 1; tau <= max_tau; ++tau){
		count = 0;
		for (int i = 0; i < len * bitsize; ++i)
			if ((buffer[i / bitsize] / pow2(i % bitsize)) % 2 == (buffer[(i+tau) / bitsize] / (pow2((i+tau) % bitsize))) % 2) ++count;
		f = 2. / sqrt((float)len * 8) * ((float)count - len * bitsize / 2);
		printf("f = %f\nAutocorr test (tau = %d) is %s\n", f, tau, ((f >= t1) && (f <= t2)) ? "passed" : "failed");
		if (!((f >= t1) && (f <= t2))) ++flag;
	}
	printf("Autocorr test %s (%d/%d)\n\n", flag ? "failed" : "passed", flag? flag: max_tau, max_tau);
}

void uni_test(test_t* buffer, int len, int len_of_block = 8){
	if (len_of_block <= 0) len_of_block = 1;
	if (len_of_block > 16) len_of_block = 16;
	printf("Uni test:\n");
	float E[16] =
		{0.732, 1.537, 2.401, 3.311, 4.253, 5.217, 6.196, 7.183,
		 8.176, 9.172, 10.17, 11.17, 12.17, 13.17, 14.17, 15.17};
	float D[16] =
		{0.690, 1.338, 1.901, 2.358, 2.705, 2.954, 3.125, 3.238,
		 3.311, 3.356, 3.384, 3.401, 3.410, 3.416, 3.419, 3.421};
	int v = pow2(len_of_block);
	float sum = 0; int current;
	int* tab = (int*)calloc(v, sizeof(int));
	int i;
	for (i = 0; i < 2000; i += len_of_block){
		current = 0;
		for (int ptr = 0; ptr < len_of_block; ++ptr){
			current *= 2;
			if ((buffer[(i + ptr) / bitsize] / pow2((i + ptr) % bitsize)) % 2) ++current;
		}
		tab[current] = i / len_of_block;
	}
	for (; i < len * (sizeof(buffer[0]) * 8); ++i){
		current = 0;
		for (int ptr = 0; ptr < len_of_block; ++ptr){
			current *= 2;
			if ((buffer[i / bitsize] / pow2(i % bitsize)) % 2) ++current; ++i;
		}
		sum += log((float)i - tab[current]);
		tab[current] = i;
	}
	float f = sum / ((len * bitsize - 2000) / len_of_block) / log(2.);
	--len_of_block;
	float simga = sqrt(D[len_of_block]);
	float t1 = E[len_of_block] - simga;
	float t2 = E[len_of_block] + simga;
	printf("t1 = %f, t2 = %f\n", t1, t2);
	printf("f = %f\nUni test %s\n\n", f, ((f >= t1) && (f <= t2)) ? "passed" : "failed");
}
