#include "proto.h"
#include <stdio.h>

static void *s_ctx;
static proto_read_fn s_proto_read;
static proto_write_fn s_proto_write;

void proto_set_fn(void *ctx, proto_read_fn read_fn, proto_write_fn write_fn)
{
	s_ctx = ctx;
	s_proto_read = read_fn;
	s_proto_write = write_fn;
}

static unsigned char proto_calc_lrc(unsigned char lrc, unsigned char *buf, unsigned int buf_size)
{
	unsigned int i;
	for (i=0; i<buf_size; i++) {
		lrc ^= buf[i];
	}
	return lrc;
}

int proto_read_frame(unsigned char *buf, unsigned int buf_size, unsigned int timeoutms)
{
	int b;
	int len;
	int i;
	unsigned char mylrc = 0;
	unsigned char lrc = 0;
	unsigned char ch = 0;
	int times;
	
	times = timeoutms / 100 + 1;
	do {
		// STX
		do {
			b = s_proto_read(s_ctx, &ch, 100);
			if (b && ch == PROTO_STX) {
				break;
			}
			times--;
		} while(times);
		if (times <= 0) {
			break;
		}
		
		// CMD
		b = s_proto_read(s_ctx, &ch, 100);
		if (!b) {
			break;
		}
		buf[0] = ch;
		
		// DATALEN
		b = s_proto_read(s_ctx, &ch, 100);
		if (!b) {
			break;
		}
		buf[1] = ch;
		len = ch;

		// DATA
		for (i=0; i<len; i++) {
			b = s_proto_read(s_ctx, &ch, 100);
			buf[2+i] = ch;
		}

		mylrc = proto_calc_lrc(0x00, buf, len+2);
		// LRC
		b = s_proto_read(s_ctx, &lrc, 200);
		if (!(b && mylrc == lrc)) {
			break;
		}
		return 1;
	} while(0);
	return 0;
}

int proto_write_frame(unsigned char *buf, unsigned int buf_size)
{
	unsigned int i;
	unsigned char lrc;
	s_proto_write(s_ctx, PROTO_STX);
	for (i=0; i<buf_size; i++) {
		s_proto_write(s_ctx, buf[i]);
	}
	lrc = proto_calc_lrc(0x00, buf, buf_size);
	s_proto_write(s_ctx, lrc);
	return 1;
}
