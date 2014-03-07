#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "write_cache.h"



int write_cache_init(write_cache *c, FILE *file, int cache_len) {
	if(cache_len <= 0)
		return -1;
	memset(c, 0, sizeof(*c));
	c->file = file;
	c->cache = malloc(cache_len);
	c->ptr = c->cache;
	c->cap = cache_len;
	return 0;
}

void write_cache_close(write_cache *c) {
	write_cache_flush(c);
	if(c->cache)
		free(c->cache);
}

void write_cache_flush(write_cache *c) {
	if(c->file == NULL || c->ptr == NULL)
		return;
	int n = c->ptr - c->cache;
	if(n > 0) {
		fwrite(c->cache, n, 1, c->file);
		c->ptr = c->cache;
	}
}

void write_cache_write(write_cache *c, const void *data, int len) {
	if(len >= c->cap) {
		int n = c->ptr - c->cache;
		if(n > 0)
			fwrite(c->cache, n, 1, c->file);
		c->ptr = c->cache;
		fwrite(data, len, 1, c->file);
	} else {
		int left = c->cap - (c->ptr - c->cache);
		if(left >= len) {
			memcpy(c->ptr, data, len);
			c->ptr += len;
		} else {
			int n = len - left;
			memcpy(c->ptr, data, left);
			fwrite(c->cache, c->cap, 1, c->file);

			c->ptr = c->cache;
			memcpy(c->ptr, data+left, n);
			c->ptr += n;
		}
	}
}