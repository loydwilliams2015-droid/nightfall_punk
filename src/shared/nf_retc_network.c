#include "nf_retc_network.h"
#include <string.h>
/* Header is the existing NF04 v6 framing, not a separate protocol version.
   The payload is the H52 NFEX format, not a replacement for NfSnapshotMessage. */
static void put32(uint8_t *b,uint32_t v){
 b[0]=(uint8_t)v;b[1]=(uint8_t)(v>>8);
 b[2]=(uint8_t)(v>>16);b[3]=(uint8_t)(v>>24);
}
size_t nf_retc_network_emit(const NfRetcPublisher *publisher,
                           uint8_t *out,size_t cap){
 if(!out||cap<NF_RETC_NETWORK_PACKET_BYTES||
    NF_RETC_NETWORK_PACKET_BYTES>NF_NET_MAX_PACKET_BYTES)return 0;
 NfRetcExternalRecord record={0};
 if(!nf_retc_external_capture(publisher,&record))return 0;
 uint8_t staged[NF_RETC_NETWORK_PACKET_BYTES]={0};
 put32(staged,NF_PROTOCOL_MAGIC);
 staged[4]=(uint8_t)NF_PROTOCOL_VERSION;staged[5]=(uint8_t)(NF_PROTOCOL_VERSION>>8);
 staged[6]=(uint8_t)NF_MSG_RETC_OBSERVATION;staged[7]=0;
 if(nf_retc_external_encode(staged+8u,NF_RETC_EXTERNAL_WIRE_BYTES,&record)
     !=NF_RETC_EXTERNAL_WIRE_BYTES)return 0;
 memcpy(out,staged,sizeof staged);
 return sizeof staged;
}
bool nf_retc_network_consume(const uint8_t *packet,size_t size,
                            NfRetcExternalCursor *cursor,
                            NfRetcExternalRecord *out){
 if(!out)return false;
 *out=(NfRetcExternalRecord){0};
 if(!packet||!cursor||size!=NF_RETC_NETWORK_PACKET_BYTES||
    packet[7]!=0||
    nf_protocol_peek_type(packet,size)!=NF_MSG_RETC_OBSERVATION)return false;
 return nf_retc_external_consume(cursor,packet+8u,
                                NF_RETC_EXTERNAL_WIRE_BYTES,out);
}
