#include "nf_ucw_codec18c.h"
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static bool write_u32(const void *p,unsigned char *out,size_t cap,size_t *n){
 if(cap<4)return false;
 uint32_t v=*(const uint32_t *)p;
 out[0]=(unsigned char)(v>>24);out[1]=(unsigned char)(v>>16);
 out[2]=(unsigned char)(v>>8);out[3]=(unsigned char)v;*n=4;return true;
}
static bool read_u32(void *p,const unsigned char *b,size_t n){
 if(n!=4)return false;
 *(uint32_t *)p=(uint32_t)b[0]<<24|(uint32_t)b[1]<<16|(uint32_t)b[2]<<8|b[3];return true;
}
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<256;seed++){
  uint32_t original=0x18c00000u+family*4096u+seed,readback=0;
  NfucRegistry a={0},b={0};
  NfucCodec v={.id=7,.version=1,.required=true,.state=&original,
   .encode=write_u32,.decode=read_u32};
  CK(nfuc_codec_register(&a,v));CK(!nfuc_codec_register(&a,v));
  v.state=&readback;CK(nfuc_codec_register(&b,v));
  unsigned char buffer[64]={0};size_t bytes=0;
  CK(nfuc_codec_write(&a,buffer,sizeof buffer,&bytes)==NFUC_CODEC_OK);
  CK(bytes==20);
  CK(nfuc_codec_read(&b,buffer,bytes)==NFUC_CODEC_OK);
  CK(readback==original);
  buffer[7]=2;
  CK(nfuc_codec_read(&b,buffer,bytes)==NFUC_CODEC_BAD_VERSION);
  buffer[7]=1;
  buffer[15]=200;
  CK(nfuc_codec_read(&b,buffer,bytes)==NFUC_CODEC_CORRUPT);
  buffer[15]=4;
  buffer[3]=8;
  CK(nfuc_codec_read(&b,buffer,bytes)==NFUC_CODEC_UNKNOWN_REQUIRED);
  buffer[11]=0;
  CK(nfuc_codec_read(&b,buffer,bytes)==NFUC_CODEC_UNKNOWN_REQUIRED);
  buffer[11]=1;buffer[3]=7;
  CK(nfuc_codec_read(&b,buffer,bytes-1)==NFUC_CODEC_CORRUPT);
  CK(nfuc_codec_read(&b,buffer,0)==NFUC_CODEC_UNKNOWN_REQUIRED);
 }
 printf("UCW_CODEC_H0 cases=1280 checks=%u PASS\n",checks);return 0;
}
