/* Xavier Ruppen / REDS / HEIG-VD
 * HPC - 17.02.16
 * array_util.h
 *
 * Array API
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/* Allocate an array of size "len" and fill it
 * with random data.
 * Return the array pointer */
uint64_t *array_init(const size_t len);

/* Liberate array memory */
void array_clear(uint64_t *data, const size_t len);

/* Arrange a array in increasing order of value */
void array_sort(uint64_t *data, const size_t len);
