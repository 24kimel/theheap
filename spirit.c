#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void get_shell(char *str) {
    if(strncmp(str, "sudo ls", 7) == 0) {
        system("/bin/sh");
    }
    else{
        puts("You don't have the privilege to control this machine");
    }
}

int main() {
	setbuf(stdout, NULL);
	malloc(1);

	unsigned long long *a;
	unsigned long long fake_chunk[10] __attribute__ ((aligned (0x10)));

	fake_chunk[1] = 0x40; // size | AMP flags
	a = &fake_chunk[2]; // pointer to the payload
	free(a);

	void *perms = malloc(0x30);
    memset(perms, 0, 0x30);
    printf("Give me your input: ");
    read(0, (char *)a, 0x30);
    get_shell(perms);
    return 0;
}