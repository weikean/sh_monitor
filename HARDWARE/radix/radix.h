#include "stm32f10x.h"


char *Change(char *s);
void data2hex(int n,char *buf);
void expand2hex(int n,char *buf);
u8 tohex(int n);
u8 hex_to_int(char *m);
u8 change_str_Data(u8 *p,u8 len);
void Swap(char *ch1, char *ch2);
void change_Data_str(int n, char str[]);
void change_hex_str(u8 dest[],u8 src[],u8 len);
int strToHex(char *ch, char *hex);
char valueToHexCh(const int value);
char* itoa(int num,char*str,int radix);
double string_to_double(char *str);
