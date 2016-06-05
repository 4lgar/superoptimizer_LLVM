#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

uint32_t foo(uint32_t a, uint32_t b){
	return a*8 + b;
}

uint32_t bar(uint32_t a, uint32_t b){
	uint32_t c = a * b;

	if(a < b)
		if(b < c)
			if(a > c)
				return foo(a, b);

	return 0;
}

void main(){
	uint32_t a = 8;
	uint32_t b = 16;

	uint32_t r = foo(a, b);
	printf("r value is: %d\n", r);

	r = bar(a, b);
	printf("r value is: %d\n", r);
}