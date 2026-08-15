# TEST
## TEST AL, imm8 (opcode A8 ib)
```asm
# Inputs:   a0 = al
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: TEST does not modify the destination register.

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # t0 = AL
andi    t0, t0, imm8          # t0 = AL AND imm8

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
## TEST AX, imm16 (opcode 66 A9 iw)
```asm
# Inputs:   a0 = ax
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result
li      t2, 0xffff
and     t0, a0, t2
li      t1, imm16
and     t0, t0, t1

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
## TEST EAX, imm32 (opcode A9 id)
```asm
# Inputs:   a0 = eax
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # isolate EAX
li      t1, imm32
and     t0, t0, t1            # t0 = EAX AND imm32

# Flags (independent, removable)
li      t4, 0                 # CF = 0
li      t5, 0                 # OF = 0
li      t8, 0                 # AF = 0
srli    t6, t0, 31            # SF = bit 31
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
## TEST RAX, imm32 (opcode REX.W + A9 id)
```asm
# Inputs:   a0 = rax
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: TEST does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t1, imm32             # RISC-V automatically sign-extends to 64 bits
and     t0, a0, t1            # t0 = RAX AND imm32 (Full 64-bit math)

# Flags (independent, removable)
li      t4, 0                 # CF = 0
li      t5, 0                 # OF = 0
li      t8, 0                 # AF = 0
srli    t6, t0, 63            # SF = bit 63 (Sign bit moves to the top)
seqz    t7, t0                # ZF
andi    t9, t0, 0xff          # PF (Always calculated on the lowest byte)
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## TEST r/m8, imm8 (opcode F6 /0 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # isolate dest low byte
andi    t0, t0, imm8          # t0 = dest AND imm8

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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
lbu     t0, 0(a0)             # load byte from RAM
andi    t0, t0, imm8          # t0 = RAM AND imm8

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
## TEST r/m16, imm16 (opcode 66 F7 /0 iw)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # isolate 16-bit dest
li      t1, imm16
and     t0, t0, t1            # t0 = dest AND imm16

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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
lhu     t0, 0(a0)             # load 16-bit from RAM
li      t1, imm16
and     t0, t0, t1            # t0 = RAM AND imm16

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
## TEST r/m32, imm32 (opcode F7 /0 id)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
li      t1, imm32
and     t0, t0, t1            # t0 = dest AND imm32

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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
lw      t0, 0(a0)             # load 32-bit from RAM (no scrub needed)
li      t1, imm32
and     t0, t0, t1            # t0 = RAM AND imm32

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
## TEST r/m64, imm32 (opcode REX.W + F7 /0 id)
### Register Destination
```asm
# Inputs:   a0 = x86 register (full 64-bit)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
li      t1, imm32             # RISC-V sign-extends 32-bit immediate
and     t0, a0, t1            # full 64-bit math

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
### Memory Destination
```asm
# Inputs:   a0 = effective address
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
ld      t0, 0(a0)             # load 64-bit from RAM
li      t1, imm32
and     t0, t0, t1            # full 64-bit math

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
## TEST r/m8, r8 (opcode 84 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register 1 (dest), a1 = x86 register 2 (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # isolate dest low byte
andi    t1, a1, 0xff          # isolate src low byte
and     t0, t0, t1

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
# Inputs:   a0 = effective address, a1 = x86 register (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
lbu     t0, 0(a0)             # load byte from RAM
andi    t1, a1, 0xff          # isolate src low byte
and     t0, t0, t1

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
## TEST r/m16, r16 (opcode 66 85 /r)
### Register Destination 
```asm
# Inputs:   a0 = x86 register 1 (dest), a1 = x86 register 2 (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # isolate 16-bit dest
and     t1, a1, t2            # isolate 16-bit src
and     t0, t0, t1

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
# Inputs:   a0 = effective address, a1 = x86 register (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
lhu     t0, 0(a0)             # load 16-bit from RAM
li      t2, 0xffff
and     t1, a1, t2            # isolate 16-bit src
and     t0, t0, t1

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
## TEST r/m32, r32 (opcode 85 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register 1 (dest), a1 = x86 register 2 (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended src
and     t0, t0, t1

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31            # SF = bit 31
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
# Inputs:   a0 = effective address, a1 = x86 register (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
lw      t0, 0(a0)             # load 32-bit from RAM
slli    t0, t0, 32
srli    t0, t0, 32            # scrub sign-extension
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended src
and     t0, t0, t1

# Flags
li      t4, 0
li      t5, 0
li      t8, 0
srli    t6, t0, 31            # SF = bit 31
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
## TEST r/m64, r64 (opcode REX.W + 85 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register 1 (dest), a1 = x86 register 2 (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
and     t0, a0, a1            # full 64-bit math

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
### Memory Destination
```asm
# Inputs:   a0 = effective address, a1 = x86 register (src)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
ld      t0, 0(a0)             # load 64-bit from RAM
and     t0, t0, a1            # full 64-bit math

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