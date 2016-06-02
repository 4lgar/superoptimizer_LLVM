#include <stdio.h>
#include <stdlib.h>
#include "array_util.h"

#define data_len 100000

int main(void){
	uint64_t *array = array_init(data_len);

	if(!array){
		fprintf(stderr, "Error creating array.\n");
		return EXIT_FAILURE;
	}

	array_sort(array, data_len);

	array_clear(array, data_len);

	return EXIT_SUCCESS;
}