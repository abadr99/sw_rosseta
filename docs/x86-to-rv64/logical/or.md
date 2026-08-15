# OR
## OR AL, imm8 (opcode 0C ib)
```asm
# Inputs:   a0 = al
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
andi    t0, a0, 0xff          # t0 = AL
ori     t0, t0, imm8          # t0 = AL OR imm8
andi    a0, a0, -256          # clear AL
or      a0, a0, t0            # write back

# Flags (independent, removable)
li      t4, 0                 # CF = 0
li      t5, 0                 # OF = 0
li      t8, 0                 # AF = 0
srli    t6, t0, 7             # SF = bit 7
seqz    t7, t0                # ZF
mv      t9, t0                # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR AX, imm16 (opcode 0D iw)
```asm
# Inputs:   a0 = ax
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2
li      t1, imm16
or      t0, t0, t1
and     t0, t0, t2
li      t2, -65536
and     a0, a0, t2
or      a0, a0, t0

# Flags (independent, removable)
li      t4, 0                 # CF = 0
li      t5, 0                 # OF = 0
li      t8, 0                 # AF = 0
srli    t6, t0, 15            # SF = bit 15
seqz    t7, t0                # ZF
andi    t9, t0, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR EAX, imm32 (opcode 0D id)
```asm
# Inputs:   a0 = eax
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
slli    a0, a0, 32
srli    a0, a0, 32
li      t1, imm32
or      a0, a0, t1
slli    a0, a0, 32
srli    a0, a0, 32

# Flags (independent, removable)
li      t4, 0                 # CF = 0
li      t5, 0                 # OF = 0
li      t8, 0                 # AF = 0
srli    t6, a0, 31            # SF = bit 31
seqz    t7, a0                # ZF
andi    t9, a0, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR RAX, imm32 (opcode REX.W + 0D id)
```asm
# Inputs:   a0 = rax
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t1, imm32
or      a0, a0, t1

# Flags (independent, removable)
li      t4, 0                 # CF = 0
li      t5, 0                 # OF = 0
li      t8, 0                 # AF = 0
srli    t6, a0, 63            # SF = bit 63
seqz    t7, a0                # ZF
andi    t9, a0, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m8, imm8 (opcode 80 /1 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
andi    t0, a0, 0xff          # t0 = reg
ori     t0, t0, imm8          # t0 = reg OR imm8
andi    a0, a0, -256          # clear low byte
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 7
seqz    t7, t0
mv      t9, t0
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated byte, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lbu     t0, 0(a0)             # load byte
ori     t0, t0, imm8          # OR
sb      t0, 0(a0)             # store byte

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 7
seqz    t7, t0
mv      t9, t0
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m16, imm16 (opcode 81 /1 iw)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2            # load 16-bit
li      t1, imm16
or      t0, t0, t1
and     t0, t0, t2            # mask
li      t2, -65536
and     a0, a0, t2            # clear low 16
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated 16-bit word, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lhu     t0, 0(a0)             # load 16-bit
li      t1, imm16
or      t0, t0, t1
li      t2, 0xffff
and     t0, t0, t2            # mask
sh      t0, 0(a0)             # store 16-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m32, imm32 (opcode 81 /1 id)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result (zero-extended 32-bit), {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended 32-bit
li      t1, imm32
or      t0, t0, t1
slli    t0, t0, 32
srli    t0, t0, 32            # mask to 32 bits
mv      a0, t0                # write back (zeroes upper 32 bits)

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated 32-bit dword, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lw      t0, 0(a0)             # load 32-bit
li      t1, imm32
or      t0, t0, t1
slli    t0, t0, 32
srli    t0, t0, 32            # mask to 32 bits
sw      t0, 0(a0)             # store 32-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m64, imm32 (opcode REX.W + 81 /1 id)
### Register Destination
```asm
# Inputs:   a0 = x86 register (full 64-bit)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t1, imm32             # sign-extended
or      a0, a0, t1            # 64-bit result

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, a0, 63
seqz    t7, a0
andi    t9, a0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated 64-bit qword, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
ld      t0, 0(a0)             # load 64-bit
li      t1, imm32             # sign-extended
or      t0, t0, t1
sd      t0, 0(a0)             # store 64-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 63
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m16, imm8 (opcode 83 /1 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2            # load 16-bit
ori     t0, t0, imm8          # sign-extended imm8
and     t0, t0, t2            # mask
li      t2, -65536
and     a0, a0, t2            # clear low 16
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated 16-bit word, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lhu     t0, 0(a0)             # load 16-bit
ori     t0, t0, imm8          # sign-extended imm8
li      t2, 0xffff
and     t0, t0, t2            # mask
sh      t0, 0(a0)             # store 16-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m32, imm8 (opcode 83 /1 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result (zero-extended 32-bit), {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended 32-bit
ori     t0, t0, imm8          # sign-extended imm8
slli    t0, t0, 32
srli    t0, t0, 32            # mask to 32 bits
mv      a0, t0                # write back (zeroes upper 32 bits)

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated 32-bit dword, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lw      t0, 0(a0)             # load 32-bit
ori     t0, t0, imm8          # sign-extended imm8
slli    t0, t0, 32
srli    t0, t0, 32            # mask to 32 bits
sw      t0, 0(a0)             # store 32-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m64, imm8 (opcode REX.W + 83 /1 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register (full 64-bit)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
ori     a0, a0, imm8          # sign-extended imm8, 64-bit result

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, a0, 63
seqz    t7, a0
andi    t9, a0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  memory[a0] = updated 64-bit qword, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
ld      t0, 0(a0)             # load 64-bit
ori     t0, t0, imm8          # sign-extended imm8
sd      t0, 0(a0)             # store 64-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 63
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m8, r8 (opcode 08 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
andi    t0, a0, 0xff          # t0 = dest low byte
andi    t1, a1, 0xff          # t1 = src low byte
or      t0, t0, t1
andi    a0, a0, -256          # clear low byte
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 7
seqz    t7, t0
mv      t9, t0
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address (destination), a1 = x86 register (source)
# Outputs:  memory[a0] = updated byte, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lbu     t0, 0(a0)             # load byte
andi    t1, a1, 0xff          # t1 = src low byte
or      t0, t0, t1
sb      t0, 0(a0)             # store byte

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 7
seqz    t7, t0
mv      t9, t0
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m16, r16 (opcode 09 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2            # load 16-bit dest
and     t1, a1, t2            # load 16-bit src
or      t0, t0, t1
li      t2, -65536
and     a0, a0, t2            # clear low 16
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address (destination), a1 = x86 register (source)
# Outputs:  memory[a0] = updated 16-bit word, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lhu     t0, 0(a0)             # load 16-bit
li      t2, 0xffff
and     t1, a1, t2            # load 16-bit src
or      t0, t0, t1
sh      t0, 0(a0)             # store 16-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m32, r32 (opcode 09 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  a0 = result (zero-extended 32-bit), {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended 32-bit dest
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended 32-bit src
or      t0, t0, t1
mv      a0, t0                # write back (zeroes upper 32 bits)

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address (destination), a1 = x86 register (source)
# Outputs:  memory[a0] = updated 32-bit dword, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
lw      t0, 0(a0)             # load 32-bit
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended 32-bit src
or      t0, t0, t1
slli    t0, t0, 32
srli    t0, t0, 32            # mask to 32 bits
sw      t0, 0(a0)             # store 32-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r/m64, r64 (opcode REX.W + 09 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination, full 64-bit), a1 = x86 register (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
or      a0, a0, a1            # 64-bit result

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, a0, 63
seqz    t7, a0
andi    t9, a0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Destination
```asm
# Inputs:   a0 = effective address (destination), a1 = x86 register (source)
# Outputs:  memory[a0] = updated 64-bit qword, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
ld      t0, 0(a0)             # load 64-bit
or      t0, t0, a1
sd      t0, 0(a0)             # store 64-bit

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 63
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r8, r/m8 (opcode 0A /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
andi    t0, a0, 0xff          # t0 = dest low byte
andi    t1, a1, 0xff          # t1 = src low byte
or      t0, t0, t1
andi    a0, a0, -256          # clear low byte
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 7
seqz    t7, t0
mv      t9, t0
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = effective address (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
andi    t0, a0, 0xff          # t0 = dest low byte
lbu     t1, 0(a1)             # load src byte
or      t0, t0, t1
andi    a0, a0, -256          # clear low byte
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 7
seqz    t7, t0
mv      t9, t0
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r16, r/m16 (opcode 0B /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2            # load 16-bit dest
and     t1, a1, t2            # load 16-bit src
or      t0, t0, t1
li      t2, -65536
and     a0, a0, t2            # clear low 16
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = effective address (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2            # load 16-bit dest
lhu     t1, 0(a1)             # load src 16-bit
or      t0, t0, t1
li      t2, -65536
and     a0, a0, t2            # clear low 16
or      a0, a0, t0            # write back

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 15
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r32, r/m32 (opcode 0B /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  a0 = result (zero-extended 32-bit), {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended 32-bit dest
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended 32-bit src
or      t0, t0, t1
mv      a0, t0                # write back (zeroes upper 32 bits)

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = effective address (source)
# Outputs:  a0 = result (zero-extended 32-bit), {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended 32-bit dest
lw      t1, 0(a1)             # load src 32-bit
or      t0, t0, t1
slli    t0, t0, 32
srli    t0, t0, 32            # mask to 32 bits
mv      a0, t0                # write back (zeroes upper 32 bits)

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31
seqz    t7, t0
andi    t9, t0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## OR r64, r/m64 (opcode REX.W + 0B /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination, full 64-bit), a1 = x86 register (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
or      a0, a0, a1            # 64-bit result

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, a0, 63
seqz    t7, a0
andi    t9, a0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
### Memory Source
```asm
# Inputs:   a0 = x86 register (destination, full 64-bit), a1 = effective address (source)
# Outputs:  a0 = result, {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
ld      t1, 0(a1)             # load src 64-bit
or      a0, a0, t1            # 64-bit result

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, a0, 63
seqz    t7, a0
andi    t9, a0, 0xff
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
