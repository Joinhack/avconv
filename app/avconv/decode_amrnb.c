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
	int cache_cap = 1024*10, cache_len = 0;
	char cache[cache_cap], *r_ptr = NULL;
	uint8_t buffer[500];
	int size = 0;
	int left = 0;
	while (1) {
		uint8_t  littleendian[320], *ptr;
		int i;
		int16_t outbuffer[160];
		/* Read the mode byte */
		if(r_ptr == NULL || r_ptr - cache == cache_len) {
			cache_len = fread(cache, sizeof(char), cache_cap, in);	
			if (cache_len <= 0)
				break;
			r_ptr = cache;
		}
		if(size == 0) {
			buffer[0] = *(uint8_t*)r_ptr;
			r_ptr++;
			size = sizes[(buffer[0] >> 3) & 0x0f];
		}
		left = cache_len - (r_ptr-cache);
		if(left < size) {
			memcpy(buffer+1, r_ptr, left);
			r_ptr += left;
			size -= left;
			continue;
		} else {
			memcpy(buffer+1, r_ptr, size);
			r_ptr += size;
		}
		size = 0;

		/* Decode the packet */
		Decoder_Interface_Decode(amr, buffer, outbuffer, 0);
		
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

