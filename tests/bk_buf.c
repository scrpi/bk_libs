#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../bk_vec.h"

int main(int argc, char *argv[])
{
	int *vec = NULL;
	uint8_t buf[bk_vec_size_for(vec, 300)];

	/* Yes these tests are really crude, it will do for now */

	printf("-- Pushing 500 elements\n");
	for (int i = 0; i < 500; ++i)
		bk_vec_push(vec, i);

	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

#if 0
	for (int i = 0; i < bk_vec_size(vec); ++i)
		printf("vec[%d]: %d\n", i, vec[i]);
#endif

	bk_vec_free(vec);
	vec = NULL;


	printf("-- Using buf of size %zu\n", sizeof(buf));
	bk_vec_use_buf(vec, buf, sizeof(buf));
	printf("buf:%p, vec:%p\n", buf, vec);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Pushing 200 elements\n");
	for (int i = 0; i < 200; ++i)
		bk_vec_push(vec, i);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Reserving 50 elements\n");
	bk_vec_reserve(vec, 50);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Reserving 200 elements\n");
	bk_vec_reserve(vec, 200);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Squishing vec\n");
	bk_vec_squish(vec);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Popping vec 20 x 1\n");
	for (int i = 0; i < 20; ++i)
		bk_vec_pop(vec, 1);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Squishing vec\n");
	bk_vec_squish(vec);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Popping vec 10 x 3\n");
	for (int i = 0; i < 10; ++i)
		bk_vec_pop(vec, 3);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));

	printf("-- Squishing vec\n");
	bk_vec_squish(vec);
	printf("bk_vec capacity: %zu\n", bk_vec_capacity(vec));
	printf("bk_vec size: %zu\n", bk_vec_size(vec));
#if 0
	for (int i = 0; i < bk_vec_size(vec); ++i)
		printf("vec[%d]: %d\n", i, vec[i]);
#endif

	bk_vec_free(vec);
}
