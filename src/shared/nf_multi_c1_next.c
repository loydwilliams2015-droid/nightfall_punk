#include "nf_multi_c1_next.h"
#include <math.h>
#include <string.h>
NfMultiCcd nf_multi_capsule_ccd18c1(const Nf18a4Body *a,const Nf18a4Body *b,float dt){
 NfMultiCcd r={0};r.status=NF_NARROW_PENDING;
 if(!a||!b||!nf18a4_body_valid(a)||!nf18a4_body_valid(b)||a->id==b->id||
    !isfinite(dt)||dt<=0||a->radius<=0||b->radius<=0||
    a->height<2*a->radius||b->height<2*b->radius)return r;
 /* Horizontal only, same height; general capsule CCD remains future work. */
 if(fabsf(a->center.y-b->center.y)>1.e-5f||
    fabsf(a->velocity.y)>1.e-5f||fabsf(b->velocity.y)>1.e-5f)return r;
 const double px=(double)a->center.x-b->center.x,pz=(double)a->center.z-b->center.z;
 const double vx=(double)a->velocity.x-b->velocity.x,vz=(double)a->velocity.z-b->velocity.z;
 const double radius=(double)a->radius+b->radius;
 const double cc=px*px+pz*pz-radius*radius;
 if(cc<=0)return r; /* initial overlap not silently healed */
 const double aa=vx*vx+vz*vz,bb=2*(px*vx+pz*vz);
 if(aa<1.e-15){r.status=NF_NARROW_OK;return r;}
 const double discr=bb*bb-4*aa*cc;
 if(discr<0){r.status=NF_NARROW_OK;return r;}
 const double t=(-bb-sqrt(discr))/(2*aa);
 if(!isfinite(t)||t<0||t>dt){r.status=NF_NARROW_OK;return r;}
 const double x=px+vx*t,z=pz+vz*t,length=sqrt(x*x+z*z);
 if(length<1.e-12)return r;
 const NfVec3 normal={(float)(x/length),0,(float)(z/length)};
 r.status=NF_NARROW_OK;r.hit=true;r.toi=(float)(t/dt);
 r.contact=(Nf18a4Constraint){.body_a=a->id,.body_b=b->id,
   .feature_id=a->id^b->id,.normal_b_to_a=normal,
   .contact_point={(float)((double)b->center.x+b->velocity.x*t+normal.x*b->radius),
                   a->center.y,(float)((double)b->center.z+b->velocity.z*t+normal.z*b->radius)},
   .restitution=0,.friction=0};
 return r;
}
NfMa02Result nf_ma02_eligibility(uint32_t actor_id,uint32_t revision,const NfMa02Witness *w){
 if(!actor_id||!revision||!w||!w->authenticated||!w->actor_id||
    !w->contract_id||w->actor_id!=actor_id||w->revision!=revision)return NF_MA02_DENIED;
 if(!w->material_known)return NF_MA02_PENDING;
 return w->material_free&&w->contract_grants?NF_MA02_ALLOWED:NF_MA02_DENIED;
}
