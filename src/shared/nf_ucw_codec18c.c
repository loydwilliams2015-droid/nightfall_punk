#include "nf_ucw_codec18c.h"
#include <string.h>
static void wr(unsigned char *p,uint32_t n){
 for(int i=3;i>=0;--i){p[i]=(unsigned char)n;n>>=8;}
}
static uint32_t rd(const unsigned char *p){
 return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
}
bool nfuc_codec_register(NfucRegistry *r,NfucCodec c){
 if(!r||!c.id||!c.version||!c.state||!c.encode||!c.decode||
    r->count>=NFUC_CODEC_MAX)return false;
 for(size_t i=0;i<r->count;i++)if(r->items[i].id==c.id)return false;
 r->items[r->count++]=c;return true;
}
NfucCodecStatus nfuc_codec_write(const NfucRegistry *r,unsigned char *buf,
 size_t cap,size_t *used){
 if(!r||!buf||!used||r->count>NFUC_CODEC_MAX||cap>NFUC_CODEC_BYTES)
  return NFUC_CODEC_CORRUPT;
 *used=0;
 for(size_t i=0;i<r->count;i++){
  const NfucCodec *c=&r->items[i];
  if(cap-*used<16u)return NFUC_CODEC_FULL;
  size_t length=0;
  if(!c->encode(c->state,buf+*used+16u,cap-*used-16u,&length)||
      length>cap-*used-16u||length>UINT32_MAX)return NFUC_CODEC_FULL;
  wr(buf+*used,c->id);wr(buf+*used+4,c->version);
  wr(buf+*used+8,c->required?NFUC_CODEC_REQUIRED:0u);
  wr(buf+*used+12,(uint32_t)length);
  *used+=16u+length;
 }
 return NFUC_CODEC_OK;
}
NfucCodecStatus nfuc_codec_read(const NfucRegistry *r,const unsigned char *b,size_t n){
 if(!r||(!b&&n)||n>NFUC_CODEC_BYTES||r->count>NFUC_CODEC_MAX)
  return NFUC_CODEC_CORRUPT;
 size_t seen=0;
 for(size_t off=0;off<n;){
  if(n-off<16u)return NFUC_CODEC_CORRUPT;
  uint32_t id=rd(b+off),ver=rd(b+off+4),flags=rd(b+off+8),sz=rd(b+off+12);
  if(!id||flags&~NFUC_CODEC_REQUIRED||sz>n-off-16u)return NFUC_CODEC_CORRUPT;
  const NfucCodec *codec=NULL;size_t slot=0;
  for(size_t i=0;i<r->count;i++)if(r->items[i].id==id){codec=&r->items[i];slot=i;break;}
  if(!codec){if(flags&NFUC_CODEC_REQUIRED)return NFUC_CODEC_UNKNOWN_REQUIRED;}
  else{
   if(seen&(size_t)(1u<<slot))return NFUC_CODEC_CORRUPT;
   if(ver!=codec->version)return NFUC_CODEC_BAD_VERSION;
   if(!codec->decode(codec->state,b+off+16u,sz))return NFUC_CODEC_CORRUPT;
   seen|=(size_t)(1u<<slot);
  }
  off+=16u+sz;
 }
 for(size_t i=0;i<r->count;i++)if(r->items[i].required&&!(seen&(size_t)(1u<<i)))
  return NFUC_CODEC_UNKNOWN_REQUIRED;
 return NFUC_CODEC_OK;
}
