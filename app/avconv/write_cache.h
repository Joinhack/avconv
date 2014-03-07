#ifndef WRITE_CACHE_H
#define WRITE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

//actually FILE already do the cache. so this is not nessary.

typedef struct write_cache {
	FILE *file;
	char *cache;
	int  cap;
	char *ptr;
} write_cache;

int write_cache_init(write_cache *c, FILE *file, int cache_len);

void write_cache_close(write_cache *c);

void write_cache_write(write_cache *c, const void *data, int len);

void write_cache_flush(write_cache *c);

#ifdef __cplusplus
}
#endif
#endif
