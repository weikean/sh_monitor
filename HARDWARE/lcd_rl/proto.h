#ifndef _PROTO_H_
#define _PROTO_H_


typedef int (*proto_read_fn)(void *ctx, unsigned char *ch, unsigned int timeoutms);
typedef int (*proto_write_fn)(void *ctx, unsigned char ch);

void proto_set_fn(void *ctx, proto_read_fn read_fn, proto_write_fn write_fn);

/* the frame buffer is from cmd to data */
int proto_read_frame(unsigned char *buf, unsigned int buf_size, unsigned int timeoutms);
int proto_write_frame(unsigned char *buf, unsigned int buf_size);

#define PROTO_STX  (0x02)

#define CMD_STATE  (0x01)
#define CMD_LED    (0x02)
#define CMD_BEEP   (0x03)
#define CMD_CLED   (0x04)

/*
common req & rep:
[STX:1][CMD:1][DATALEN:1][DATA:N][LRC:1]
LRC is from CMD to DATA
DATALEN is length DATA

CMD_STATE REQ
[STX:1][0x01:1][DATALEN:1][PRESS1:2][PRESS2:2][PRESS3:2][TEMP1:2][TEMP2:2][TEMP3:2][LRC:1]
for DELTAPRESS/PRESS1/PRESS2/TEMP1/TEMP2/TEMP3 
endian big-endian
empty   0xFFFF
invalid 0xFFFE

CMD_STATE REP
[STX:1][0x01:1][DATALEN:1][ACK:1][LRC:1]
for ACK
OK   0x00
ERROR  other

CMD_LED REQ
[STX:1][0x02:1][DATALEN:1][LED_SET:1][LRC:1]
for LED_SET
mask for 8 leds, 1 for on, 0 for off

CMD_LED REP
[STX:1][0x02:1][DATALEN:1][ACK:1][LRC:1]

CMD_BEEP REQ
[STX:1][0x03:1][DATALEN:1][BEEP_TYPE:1][LRC:1]
for BEEP_TYPE
on   0xFF
off  0x00
oneshot 0x01

CMD_BEEP REP
[STX:1][0x03:1][DATALEN:1][ACK:1][LRC:1]

CMD_CLED REQ
[STX:1][0x04:1][DATALEN:1][LED_COLOR:1][LRC:1]
for LED_COLOR
off 0x00 
red 0x01 
yellow 0x02
green 0x03

CMD_CLED REP
[STX:1][0x04:1][DATALEN:1][ACK:1][LRC:1]

*/


#endif
