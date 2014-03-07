#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <opencore-amrnb/interf_dec.h>
#include "decode_amrnb.h"

/* From WmfDecBytesPerFrame in dec_input_format_tab.cpp */
const int sizes[] = { 12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0 };


int decode_amrnb(const char *amrpath, encode_ctx *ctx) {
	FILE* in;
	char header[6];
	int n;
	void *wav, *amr;

	in = fopen(amrpath, "rb");
	if (!in) {
		perror(amrpath);
		return -1;
	}
	n = fread(header, 1, 6, in);
	if (n != 6 || memcmp(header, "#!AMR\n", 6)) {
		fclose(in);
		fprintf(stderr, "Bad header\n");
		return -1;
	}
	amr = Decoder_Interface_init();
	int cache_cap = 1024*24, cache_len = 0;
	char cache[cache_cap], *r_ptr = NULL;
	
	while (1) {
		uint8_t  littleendian[320], *ptr;
		int i, size, left;
		int16_t outbuffer[160];
		/* Read the mode byte */
		if(r_ptr == NULL) {
			int rn = fread(cache + cache_len, sizeof(char), cache_cap - cache_len, in);	
			if (rn <= 0)
				break;
			cache_len += rn;
			r_ptr = cache;
		}
		size = sizes[(*r_ptr >> 3) & 0x0f];
		left = cache_len - (r_ptr-cache);
		if(left < size) {
			memmove(cache, r_ptr, left);
			cache_len = left;
			r_ptr = NULL;
			continue;
		}
		
		/* Decode the packet */
		Decoder_Interface_Decode(amr, (uint8_t*)r_ptr, outbuffer, 0);
		r_ptr += 1 + size;
		if(r_ptr - cache == cache_len) {
			r_ptr = NULL;
			cache_len = 0;	
		}
		
		/* Convert to little endian and write to wav */
		ptr = littleendian;
		for (i = 0; i < 160; i++) {
			*ptr++ = (outbuffer[i] >> 0) & 0xff;
			*ptr++ = (outbuffer[i] >> 8) & 0xff;
		}
		ctx->write(ctx, littleendian, 320);
	}
	fclose(in);
	Decoder_Interface_exit(amr);
	ctx->flush(ctx);
	ctx->close(ctx);
	return 0;
}

