#include "nf_governor18b7.h"
#include <string.h>
void nf4g_init(Nf4gState *s){
 if(s){memset(s,0,sizeof *s);s->mode=NF4G_READ_ONLY;}
}
Nf4gMode nf4g_step(Nf4gState *s,Nf4gMetrics m){
 if(!s)return NF4G_READ_ONLY;
 s->granted_optional_us=0;
 /* Insufficient authoritative evidence and malformed performance measurements
    never certify that background work may safely proceed. */
 if(!m.journal_verified||m.uncertain_disk||!m.budget_us||
    m.used_us>m.budget_us){
  s->mode=NF4G_READ_ONLY;s->healthy_ticks=0;return s->mode;
 }
 if(!m.telemetry_valid||m.critical_backlog||
    m.budget_us-m.used_us < m.budget_us/5u){
  s->mode=NF4G_THROTTLED;s->healthy_ticks=0;return s->mode;
 }
 /* Four consecutive healthy observed owner ticks required after degradation,
    preventing oscillation when load fluctuates near the threshold. */
 if(s->healthy_ticks<4)s->healthy_ticks++;
 if(s->healthy_ticks<4)return s->mode;
 s->mode=NF4G_NORMAL;
 if(m.optional_backlog){
  uint32_t available=m.budget_us-m.used_us;
  uint32_t allotment=available/4u;
  s->granted_optional_us=allotment<250u?allotment:250u;
 }
 return s->mode;
}
