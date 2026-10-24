#include <threads.h>
#include <stdatomic.h>
#include <stdio.h>
static atomic_uint total;
static int worker(void *arg){(void)arg;for(unsigned i=0;i<10000;i++)atomic_fetch_add(&total,1);return 0;}
int main(void){
 thrd_t a[4];
 for(int i=0;i<4;i++)if(thrd_create(&a[i],worker,NULL)!=thrd_success)return 1;
 for(int i=0;i<4;i++){int status=-1;if(thrd_join(a[i],&status)!=thrd_success||status)return 2;}
 if(atomic_load(&total)!=40000)return 3;
 puts("TSAN runtime 4-thread atomic smoke PASS");return 0;
}
