/* Production variant MUST reject the two scientific controls. */
#include "nf_embody18a6.h"
#include <stdio.h>
#include <stdlib.h>
static int check_model(Nf18a6Model model){
  NfWorld *w=malloc(sizeof(*w));Nf18a6Runtime *r=malloc(sizeof(*r));
  if(!w||!r)exit(2);
  nf_world_init(w,42);
  nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
  const NfEntityId id=nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1});
  Nf18a4Body box={0};box.id=2001;box.center=(NfVec3){2,.9f,1};
  box.inverse_mass=1.f/24.f;box.inverse_inertia=(NfVec3){.3f,.3f,.3f};
  box.box_half=(NfVec3){.2f,.5f,.35f};
  const int valid=id!=0&&nf18a6_init(r,w,id,box,3,model);
  free(w);free(r);return valid;
}
int main(void){
 const int a=!check_model(NF18A6_LEGACY_REFERENCE);
 const int b=!check_model(NF18A6_LAB_ONLY);
 const int c=check_model(NF18A6_WORLD_BRIDGE);
 const int d=check_model(NF18A6_WORLD_CAMERA);
 printf("P01_legacy_control_disabled,%s\n",a?"PASS":"FAIL");
 printf("P02_lab_only_control_disabled,%s\n",b?"PASS":"FAIL");
 printf("P03_world_bridge_enabled,%s\n",c?"PASS":"FAIL");
 printf("P04_world_camera_enabled,%s\n",d?"PASS":"FAIL");
 printf("TOTAL,%d,%d\n",a+b+c+d,4-a-b-c-d);
 return a&&b&&c&&d?0:1;
}
