#include "nf_hf18b4.h"
bool nf_hf_input(NfHfClient *c,NfHfEvent e,uint32_t seq){
 if(!c||!c->actor||!c->action)return false;
 if(e==NF_HF_INPUT_PRESS){
  if(c->toggle_mode&&c->holding){c->holding=0;return true;}
  if(seq<=c->seq)return false;
  c->seq=seq;c->holding=1;c->confirmed=0;c->state=NF_HF_ATTEMPT;return true;
 }
 if(e==NF_HF_INPUT_RELEASE){if(c->toggle_mode)return false;c->holding=0;return true;}
 return false;
}
bool nf_hf_server(NfHfClient *c,NfHfEvent e,uint32_t seq){
 if(!c||!c->actor||!c->action||!seq||seq!=c->seq)return false;
 switch(e){
 case NF_HF_SERVER_PENDING:c->state=NF_HF_PENDING;break;
 case NF_HF_SERVER_CONTESTED:c->state=NF_HF_CONTESTED;break;
 case NF_HF_SERVER_REFUSED:c->state=NF_HF_REFUSED;break;
 case NF_HF_SERVER_APPLIED:c->state=NF_HF_APPLIED;c->confirmed=1;break;
 case NF_HF_SERVER_INTERRUPTED:c->state=NF_HF_INTERRUPTED;break;
 default:return false;
 }
 return true;
}
const char *nf_hf_label(NfHfState s){
 static const char *labels[]={"Idle","Attempt sent","Waiting on world","Another actor contests this","Not permitted or physically blocked","Applied by server","Interaction interrupted"};
 return (unsigned)s<sizeof(labels)/sizeof(labels[0])?labels[s]:"Unknown";
}
