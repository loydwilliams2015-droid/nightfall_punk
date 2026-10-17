#include "nf_hf18b4.h"
#include <assert.h>
#include <stdio.h>
int main(void){unsigned n=0;NfHfClient c={.actor=1,.action=9};
#define CHECK(x) do{assert(x);++n;}while(0)
 CHECK(nf_hf_input(&c,NF_HF_INPUT_PRESS,1));
 CHECK(c.state==NF_HF_ATTEMPT&&!c.confirmed);
 CHECK(!nf_hf_server(&c,NF_HF_SERVER_APPLIED,0));
 CHECK(nf_hf_server(&c,NF_HF_SERVER_PENDING,1));
 CHECK(nf_hf_server(&c,NF_HF_SERVER_CONTESTED,1));
 CHECK(!nf_hf_input(&c,NF_HF_INPUT_PRESS,1));
 CHECK(nf_hf_server(&c,NF_HF_SERVER_REFUSED,1));
 CHECK(!c.confirmed);
 CHECK(nf_hf_input(&c,NF_HF_INPUT_PRESS,2));
 CHECK(!nf_hf_server(&c,NF_HF_SERVER_APPLIED,1));
 CHECK(nf_hf_server(&c,NF_HF_SERVER_APPLIED,2));
 CHECK(c.confirmed&&c.state==NF_HF_APPLIED);
 CHECK(!nf_hf_server(&c,NF_HF_SERVER_PENDING,2));
 CHECK(!nf_hf_server(&c,NF_HF_SERVER_CONTESTED,2));
 CHECK(c.state==NF_HF_APPLIED);
 CHECK(nf_hf_server(&c,NF_HF_SERVER_INTERRUPTED,2));
 CHECK(c.confirmed);
 c.toggle_mode=1;c.holding=0;
 CHECK(nf_hf_input(&c,NF_HF_INPUT_PRESS,3));
 CHECK(nf_hf_input(&c,NF_HF_INPUT_PRESS,4));
 CHECK(!c.holding);
 CHECK(!nf_hf_input(&c,NF_HF_INPUT_RELEASE,4));
 CHECK(nf_hf_label(NF_HF_CONTESTED)!=0);
 printf("b4_hf_unit=%u PASS\n",n);return 0;}
