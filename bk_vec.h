/**
 * bk_vec v0.2
 * 2017-2018 Ben Kelly <ben@benjii.net>
 * github.com/scrpi/bk_libs
 *
 * For LICENCE see end of file.
 *
 * Changelog
 * =========
 * v0.2 (May 11, 2018)
 *   - Move licence information to end of file
 *   - Added option to provide buffer for optimising/negating allocs (bk_vec_use_buf)
 *   - Added bk_vec_pop(), bk_vec_squish()
 *
 * v0.1 (Jan 20, 2018)
 *   - Initial version
 */

#ifndef _BK_VEC_H_
#define _BK_VEC_H_

/*
 * Usage Example 1:
 * ================
 *
 * int *vec = NULL;
 *
 * for (int i = 0; i < 20; ++i)
 *     bk_vec_push(vec, i);
 *
 * for (int i = 0; i < bk_vec_size(vec); ++i)
 *     printf("val: %d\n", vec[i]);
 *
 * bk_vec_free(vec);
 *
 *
 * Usage Example 2:
 * ================
 *
 * struct thing **things = NULL;
 * bk_vec_reserve(things, 8);
 *
 * while (some_condition) {
 *     struct thing *t = malloc(sizeof(*t));
 *     t->foo = a;
 *     t->bar = b;
 *     bk_vec_push(things, t);
 * }
 *
 * bk_vec_free(things);
 *
 *
 * Usage Example 3:
 * ================
 * (small buffer optimisation - no allocs happen in this example)
 *
 * int *vec = NULL;
 * int8_t buf[bk_vec_size_for(vec, 30)];
 *
 * bk_vec_use_buf(vec, buf, sizeof(buf));
 *
 * for (int i = 0; i < 20; ++i)
 *     bk_vec_push(vec, i);
 *
 * for (int i = 0; i < bk_vec_size(vec); ++i)
 *     printf("val: %d\n", vec[i]);
 *
 * bk_vec_free(vec);
 *
 **/

struct bk_vec_hdr {
	/* MSB of capacity is used as a flag indicating small buf is in use.
	 * This effectively limits the max size of a bk_vec to half the maximum
	 * value of size_t. */
	size_t capacity;
	size_t size;
};

#define BK_CAP_BUF_BIT              (1ull << (sizeof(size_t) * 8 - 1))

#define bk_vec_capacity(a)          (size_t)(bk_vec__base(a)->capacity & ~BK_CAP_BUF_BIT)
#define bk_vec_size(a)              bk_vec__base(a)->size
#define bk_vec_push(a, v)           (bk_vec__grow_maybe(a, 1), (a)[bk_vec_size(a)++] = (v))
#define bk_vec_reserve(a, len)      bk_vec__grow_maybe(a, len)
#define bk_vec_free(a)              do { if (!bk_vec__is_buf(a)) free(bk_vec__base(a)); } while (0)
#define bk_vec_use_buf(a, buf, len) ((a) = _bk_vec_use_buf((buf), (len), sizeof(*(a))))
#define bk_vec_size_for(a, len)     ((sizeof(*(a)) * (len)) + sizeof(struct bk_vec_hdr))
#define bk_vec_pop(a, num)          do { bk_vec_size(a) = bk_vec_size(a) > (num) ? bk_vec_size(a) - (num) : 0; } while (0)
#define bk_vec_squish(a)            ((a) = _bk_vec_grow_func((a), 0, sizeof(*(a))))

#define bk_vec__base(a)             ((struct bk_vec_hdr*)((char *)(a) - sizeof(struct bk_vec_hdr)))
#define bk_vec__full(a, len)        ((a) == NULL || bk_vec_size(a) + len >= bk_vec_capacity(a))
#define bk_vec__grow_maybe(a, len)  (bk_vec__full(a, len) ? bk_vec__grow(a, len) : 0)
#define bk_vec__grow(a, len)        ((a) = _bk_vec_grow_func((a), (len), sizeof(*(a))))
#define bk_vec__is_buf(a)           ((bk_vec__base(a)->capacity & BK_CAP_BUF_BIT) == BK_CAP_BUF_BIT)


static inline void *_bk_vec_use_buf(void *buf, size_t len, size_t elem_size)
{
	struct bk_vec_hdr *hdr = buf;

	if (elem_size + sizeof(*hdr) > len)
		return NULL;

	hdr->capacity = ((len - sizeof(*hdr)) / elem_size) | BK_CAP_BUF_BIT;
	hdr->size = 0;

	return hdr + 1;
}

static inline void *_bk_vec_grow_func(void *a, size_t len, size_t elem_size)
{
	size_t cap = a ? bk_vec_capacity(a) : 0;
	size_t next_cap = cap + (cap / 2) + 1;
	size_t new_cap = next_cap > cap + len ? next_cap : cap + len;
	int    has_buf = a ? bk_vec__is_buf(a) : 0;
	struct bk_vec_hdr *hdr = a ? (has_buf ? NULL : bk_vec__base(a)) : NULL;

	/* len = 0 means squish */
	if (len == 0 && a) {
		if (has_buf) /* Can't squish a buf */
			return a;
		new_cap = bk_vec_size(a);
	}

	struct bk_vec_hdr *new_hdr = realloc(hdr, sizeof(*hdr) + elem_size * new_cap);
	if (!new_hdr)
		return a;  /* Return original vector unchanged on allocation failure */
	hdr = new_hdr;
	hdr->capacity = new_cap;

	if (!a) {
		hdr->size = 0;
		goto out;
	}

	/* Since we can't realloc a buf we have to do a manual memcpy */
	if (has_buf) {
		memcpy(hdr + 1, a, bk_vec_size(a) * elem_size);
		hdr->size = bk_vec_size(a);
	}

out:
	return hdr + 1;
}

#endif /* _BK_VEC_H_ */

/*
-------------------------------------------------------------------------------
This software is dual licenced and is available under both MIT and Public
Domain. Users may choose either licence according to their preference.

-------------------------------------------------------------------------------
MIT Licence

Copyright (c) 2018 Ben Kelly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------------------------------------------------------
Public Domain <http://unlicence.org>

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------------------------------------------------------
*/
