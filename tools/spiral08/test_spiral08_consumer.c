#include "spiral08_consumer.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    const NfS08Frame a={7,501,1,11,0,0,0};
    const NfS08Frame b={7,501,1,12,1,0,0};
    const NfS08Frame c={7,502,1,11,1,0,0};
    const NfS08Frame different={8,502,1,11,1,0,0};
    const NfS08Frame batch[]={a,b};
    const NfS08Frame duplicate[]={a,a};
    assert(nf_s08_render_batch(batch,2));
    assert(!nf_s08_render_batch(duplicate,2));
    assert(nf_s08_animation_pair(a,c,2.0f,2));
    assert(!nf_s08_animation_pair(a,different,2.0f,2));
    const uint64_t events[]={101,102};
    const uint64_t bad_events[]={101,101};
    assert(nf_s08_audio_events(events,2,100));
    assert(!nf_s08_audio_events(bad_events,2,100));
    const size_t tiles[]={120,130};
    assert(nf_s08_tiles(tiles,2,2,250));
    assert(!nf_s08_tiles(tiles,2,2,249));
    const NfS08Cursor old={0x1234,7,500,0},newer={0x1234,7,501,1},wrong={0x1234,8,501,1};
    assert(nf_s08_consume(&old,&newer,0x1234));
    assert(!nf_s08_consume(&old,&wrong,0x1234));
    puts("Spiral 08: 10 native C consumer assertions PASS (5 positive/5 negative)");
    return 0;
}
