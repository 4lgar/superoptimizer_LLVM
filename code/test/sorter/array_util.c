#include "array_util.h"

/* Allocate an array of size "len" and fill it
 * with random data.
 * Return the array pointer */
uint64_t *array_init(const size_t len){
	uint64_t *array = malloc(len * sizeof(uint64_t));

	size_t i = 0;

	for(; i < len; ++i){
		array[i] = rand();
	}

	return array;
}

/* Liberate array memory */
void array_clear(uint64_t *data, const size_t len){
	free(data);
}

/* Arrange a array in increasing order of value */
void array_sort(uint64_t *data, const size_t len){
	int i 			= 0;
	size_t j 		= 0;
	bool sorted = true;

	if(data){
		for(i = len - 1; i >= 0; --i){
			sorted = true;

			for(j = 0; j < i; ++j){
				if(data[j + 1] < data[j]){
					/*Exchange*/
					data[j + 1] 	= data[j + 1] ^ data[j];
					data[j] 			= data[j + 1] ^ data[j];
					data[j + 1] 	= data[j + 1] ^ data[j];

					sorted = false;
				}
			}

			if(sorted){
				break;
			}
		}		
	}
}
