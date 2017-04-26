/*******************************************************************************
* 函数名  : 字符串，10进制，16进制互相转换函数库
* 描述    :
* 说明    : 无
*******************************************************************************/

#include "radix.h"
#include "string.h"
/*******************************************************************************
* 函数名 : string_to_double
* 描述   : 字符串转double
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

double string_to_double(char *str)
{
double ans;
double k;
ans=0;
while (*str && *str!='.')
{
ans=ans*10+*str-'0';
str++;
}
if (*str=='.')
{
*str++;
k=0.1;
while (*str)
{
ans+=(*str-'0')*k;
k/=10;
str++;
}
}
return ans;
}
/*******************************************************************************
* 函数名 : dec_to_hex
* 描述   : 字符串转16进制
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

char *Change(char *s) {
    char i,chs;
		char *t = NULL;
    for(i = 0; s[i]; ++i) {
        chs = s[i] & 0X0F;
        t[3 * i + 2] = ' ';
        if(chs > 9) t[3 * i + 1] = chs - 10 + 'A';
        else t[3 * i + 1] = chs + '0';
        chs = (s[i] & 0XF0) >> 4;
        if(chs > 9) t[3 * i] = chs - 10 + 'A';
        else t[3 * i] = chs + '0';
    }
    t[3 * i] = '\0';
    return t;
}
/*******************************************************************************
* 函数名 : dec_to_hex
* 描述   : 整型转16进制
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

void data2hex(int n,char *buf)
{
	
	int i=0;
	int mod;
	int j,k;
	memset(buf,0,sizeof(buf));
	while(n)
	{
		mod = n%16;
		buf[i++]=tohex(mod);
		n=n/16;
		
	}
	if(i == 1)
		buf[i++] = tohex(0);
		if(i == 3)
		buf[i++] = tohex(0);
			if(i == 5)
		buf[i++] = tohex(0);
	
//得进行反序。
	
	for(j=0,k=i-1;j<i/2;j++,k--)
	{
		char temp;
		temp = 0;
		temp = buf[j];
		buf[j] = buf[k];
		buf[k] = temp;
	}
	
	buf[i]='\0';

}

void expand2hex(int n,char *buf)
{
	
	int i=0;
	int mod;
	int j,k;
	memset(buf,0,sizeof(buf));
	while(n)
	{
		mod = n%16;
		buf[i++]=tohex(mod);
		n=n/16;
		
	}
	if(i == 1)
		buf[i++] = tohex(0);
		if(i == 3)
		buf[i++] = tohex(0);
			if(i == 5)
		buf[i++] = tohex(0);
	
  //进行反序。
	
	for(j=0,k=i-1;j<i/2;j++,k--)
	{
		char temp;
		temp = 0;
		temp = buf[j];
		buf[j] = buf[k];
		buf[k] = temp;
	}
	
	buf[i]='\0';

}

u8 tohex(int n)
{

    if(n>=10 && n<=15)
    {
    	return 'A'+n-10;
    }
    return '0'+n;
}

/*******************************************************************************
* 函数名 : hex_to_dec
* 描述   : 字符串转整型
* 输入   : 
* 输出   : 待测试
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 hex_to_int(char *m)
{
u8 i,t;
u8 sum=0;
for(i=0;m[i];i++)
{
if(m[i]<='9')t=m[i]-'0';
else  t=m[i]-'a'+10;
sum=sum*16+t;
}
return sum;
}


/*******************************************************************************
* 函数名 : change_str_Data
* 描述   : 字符串转整型
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 change_str_Data(u8 *p,u8 len)
{
  u8 i=0;
	u8 value=0;
	for(i=0;i<len;i++)
	{
    value=value*10;
		value+=(*(p++)-'0');
	}
	return value;
}
/*******************************************************************************
* 函数名 : Swap
* 描述   : 交换
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Swap(char *ch1, char *ch2)
{
	char tmp = *ch1;
	*ch1 = *ch2;
	*ch2 = tmp;
}
/*******************************************************************************
* 函数名 : Convert
* 描述   : 转化成整型
* 输入   : n:要转换的整型数据 ,str:保存的字符
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void change_Data_str(int n, char str[])
{
	int i, len;
	for(i = 0; n != 0; ++i)
	{
		str[i] = n % 10 + '0';
		n /= 10;
	}
	str[i] = '\0';
	len = i;
	/* 翻转 */
	for(i = 0; i < len/2; ++i)
		Swap(str+i, str+len-i-1);
	str[len] = '\0';
}

/*******************************************************************************
* 函数名 : change_hex_str
* 描述   : 十六进制转换成字符串 如0xAC->'A''C';
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void change_hex_str(u8 dest[],u8 src[],u8 len)
{
    u8 i=0;
    u8 temp_h;
    u8 temp_l;
    for(i=0;i<len;i++)
    {
        temp_h=src[i]>>4;
        temp_l=src[i]&0X0F;
		 if(src[i] != NULL || src[i] == 0x00){
        if(temp_h>9)
            dest[2*i]=(src[i]>>4)+55;
        else
            dest[2*i]=(src[i]>>4)+48;
        if(temp_l>9)
            dest[2*i+1]=(src[i]&0X0F)+55;
        else
            dest[2*i+1]=(src[i]&0X0F)+48;
			}
			
		
    }
		dest[2*i+2]='\0';
}

/*******************************************************************************
* 函数名 : change_hex_str
* 描述   : 字符串转换成16进制字符串 
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

int strToHex(char *ch, char *hex)
{
  int high,low;
  int tmp = 0;
  if(ch == NULL || hex == NULL){
    return -1;
  }

  if(strlen(ch) == 0){
    return -2;
  }

  while(*ch){
    tmp = (int)*ch;
    high = tmp >> 4;
    low = tmp & 15;
    *hex++ = valueToHexCh(high); //先写高字节
    *hex++ = valueToHexCh(low); //其次写低字节
    ch++;
  }
  *hex = '\0';
  return 0;
}

char valueToHexCh(const int value)
{
  char result = '\0';
  if(value >= 0 && value <= 9){
    result = (char)(value + 48); //48为ascii编码的‘0’字符编码值
  }
  else if(value >= 10 && value <= 15){
    result = (char)(value - 10 + 65); //减去10则找出其在16进制的偏移量，65为ascii的'A'的字符编码值
  }
  else{
    ;
  }

  return result;
}


char* itoa(int num,char*str,int radix)
{/*索引表*/
char index[]="0123456789ABCDEF";
unsigned unum;/*中间变量*/
int i=0,j,k;
char temp;
/*确定unum的值*/
if(radix==10&&num<0)/*十进制负数*/
{
unum=(unsigned)-num;
str[i++]='-';
}
else unum=(unsigned)num;/*其他情况*/
/*转换*/
do{
str[i++]=index[unum%(unsigned)radix];
unum/=radix;
}while(unum);
str[i]='\0';
/*逆序*/
if(str[0]=='-')k=1;/*十进制负数*/
else k=0;

for(j=k;j<=(i-1)/2;j++)
{
temp=str[j];
str[j]=str[i-1+k-j];
str[i-1+k-j]=temp;
}
return str;
}
