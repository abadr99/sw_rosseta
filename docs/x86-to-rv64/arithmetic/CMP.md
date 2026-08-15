# CMP
## CMP AL, imm8 (opcode 3C ib)
```asm
# Inputs:   a0 = al
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # t0 = AL
li      t1, imm8              # raw encoded immediate
andi    t1, t1, 0xff          # CMP imm8 is an unsigned 8-bit operand
sub     t3, t0, t1            # t3 = AL - imm8
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if AL < imm8 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP AX, imm16 (opcode 66 3D iw)
```asm
# Inputs:   a0 = ax
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # t0 = 16-bit AX
li      t1, imm16
and     t1, t1, t2            # t1 = 16-bit imm16
sub     t3, t0, t1            # t3 = AX - imm16
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if AX < imm16 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP EAX, imm32 (opcode 3D id)
```asm
# Inputs:   a0 = eax
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # t0 = zero-extended EAX
li      t1, imm32
slli    t1, t1, 32
srli    t1, t1, 32            # scrub RISC-V sign-extension for unsigned math
sub     t3, t0, t1            # t3 = EAX - imm32
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if EAX < imm32 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP RAX, imm32 (opcode REX.W + 3D id)
```asm
# Inputs:   a0 = rax
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}

# Main result (Calculated but NOT written back)
li      t1, imm32             # raw encoded imm32
slli    t1, t1, 32
srai    t1, t1, 32            # sign-extend imm32 to the 64-bit operand width
sub     t3, a0, t1            # t3 = RAX - imm32 (full 64-bit math)

# Flags (Arithmetic Simulation)
sltu    t4, a0, t1            # CF: Set if RAX < imm32 (unsigned borrow)

xor     t5, a0, t1
xor     t2, a0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, a0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m8, imm8 (opcode 80 /7 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # isolate dest low byte
li      t1, imm8              # raw encoded immediate
andi    t1, t1, 0xff          # CMP imm8 is an unsigned 8-bit operand
sub     t3, t0, t1            # t3 = dest - imm8
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < imm8 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lbu     t0, 0(a0)             # load byte from RAM
li      t1, imm8              # raw encoded immediate
andi    t1, t1, 0xff          # CMP imm8 is an unsigned 8-bit operand
sub     t3, t0, t1            # t3 = RAM - imm8
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm8 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m16, imm16 (opcode 66 81 /7 iw)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # isolate 16-bit dest
li      t1, imm16
and     t1, t1, t2            # isolate 16-bit imm16
sub     t3, t0, t1            # t3 = dest - imm16
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < imm16 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lhu     t0, 0(a0)             # load 16-bit word from RAM
li      t2, 0xffff
li      t1, imm16
and     t1, t1, t2            # isolate 16-bit imm16
sub     t3, t0, t1            # t3 = RAM - imm16
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm16 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m32, imm32 (opcode 81 /7 id)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
li      t1, imm32
slli    t1, t1, 32
srli    t1, t1, 32            # scrub RISC-V sign-extension for unsigned math
sub     t3, t0, t1            # t3 = dest - imm32
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < imm32 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lw      t0, 0(a0)             # load 32-bit dword from RAM
slli    t0, t0, 32
srli    t0, t0, 32            # zero-extend before unsigned comparison
li      t1, imm32
slli    t1, t1, 32
srli    t1, t1, 32            # scrub RISC-V sign-extension for unsigned math
sub     t3, t0, t1            # t3 = RAM - imm32
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm32 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m64, imm32 (opcode REX.W + 81 /7 id)
### Register Destination
```asm
# Inputs:   a0 = x86 register (full 64-bit)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t1, imm32             # raw encoded imm32
slli    t1, t1, 32
srai    t1, t1, 32            # sign-extend imm32 to the 64-bit operand width
sub     t3, a0, t1            # t3 = dest - imm32 (full 64-bit math)

# Flags (Arithmetic Simulation)
sltu    t4, a0, t1            # CF: Set if dest < imm32 (unsigned borrow)

xor     t5, a0, t1
xor     t2, a0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, a0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
ld      t0, 0(a0)             # load 64-bit qword from RAM
li      t1, imm32             # raw encoded imm32
slli    t1, t1, 32
srai    t1, t1, 32            # sign-extend imm32 to the 64-bit operand width
sub     t3, t0, t1            # t3 = RAM - imm32 (full 64-bit math)

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm32 (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m16, imm8 (opcode 66 83 /7 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # t0 = 16-bit dest
li      t1, imm8              # raw encoded imm8
slli    t1, t1, 56
srai    t1, t1, 56            # sign-extend imm8 to the operand width
and     t1, t1, t2            # mask imm8 to 16 bits
sub     t3, t0, t1            # t3 = dest - imm8
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < imm8 (unsigned borrow)
xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lhu     t0, 0(a0)             # load 16-bit word from RAM
li      t2, 0xffff
li      t1, imm8              # raw encoded imm8
slli    t1, t1, 56
srai    t1, t1, 56            # sign-extend imm8 to the operand width
and     t1, t1, t2            # mask imm8 to 16 bits
sub     t3, t0, t1            # t3 = RAM - imm8
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm8 (unsigned borrow)
xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m32, imm8 (opcode 83 /7 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
li      t1, imm8              # raw encoded imm8
slli    t1, t1, 56
srai    t1, t1, 56            # sign-extend imm8 to the operand width
slli    t1, t1, 32
srli    t1, t1, 32            # represent the sign-extended value as unsigned 32-bit
sub     t3, t0, t1            # t3 = dest - imm8
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < imm8 (unsigned borrow)
xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lw      t0, 0(a0)             # load 32-bit dword from RAM
slli    t0, t0, 32
srli    t0, t0, 32            # zero-extend before unsigned comparison
li      t1, imm8              # raw encoded imm8
slli    t1, t1, 56
srai    t1, t1, 56            # sign-extend imm8 to the operand width
slli    t1, t1, 32
srli    t1, t1, 32            # represent the sign-extended value as unsigned 32-bit
sub     t3, t0, t1            # t3 = RAM - imm8
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm8 (unsigned borrow)
xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m64, imm8 (opcode REX.W + 83 /7 ib)
### Register Destination
```asm
# Inputs:   a0 = x86 register (full 64-bit)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t1, imm8              # raw encoded imm8
slli    t1, t1, 56
srai    t1, t1, 56            # sign-extend imm8 to the 64-bit operand width
sub     t3, a0, t1            # full 64-bit math (dest - imm8)

# Flags (Arithmetic Simulation)
sltu    t4, a0, t1            # CF: Set if dest < imm8 (unsigned borrow)
xor     t5, a0, t1
xor     t2, a0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, a0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
ld      t0, 0(a0)             # load 64-bit qword from RAM
li      t1, imm8              # raw encoded imm8
slli    t1, t1, 56
srai    t1, t1, 56            # sign-extend imm8 to the 64-bit operand width
sub     t3, t0, t1            # full 64-bit math (RAM - imm8)

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < imm8 (unsigned borrow)
xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m8, r8 (opcode 38 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # isolate dest low byte
andi    t1, a1, 0xff          # isolate src low byte
sub     t3, t0, t1            # t3 = dest - src
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lbu     t0, 0(a0)             # load byte from RAM
andi    t1, a1, 0xff          # isolate src low byte
sub     t3, t0, t1            # t3 = RAM - src
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m16, r16 (opcode 66 39 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # isolate 16-bit dest
and     t1, a1, t2            # isolate 16-bit src
sub     t3, t0, t1            # t3 = dest - src
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lhu     t0, 0(a0)             # load 16-bit word from RAM
li      t2, 0xffff
and     t1, a1, t2            # isolate 16-bit src
sub     t3, t0, t1            # t3 = RAM - src
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m32, r32 (opcode 39 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended src
sub     t3, t0, t1            # t3 = dest - src
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
lw      t0, 0(a0)             # load 32-bit dword from RAM
slli    t0, t0, 32
srli    t0, t0, 32            # zero-extend before unsigned comparison
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended src
sub     t3, t0, t1            # t3 = RAM - src
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if RAM < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r/m64, r64 (opcode REX.W + 39 /r)
### Register Destination
```asm
# Inputs:   a0 = x86 register (destination, full 64-bit), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
sub     t3, a0, a1            # full 64-bit math (dest - src)

# Flags (Arithmetic Simulation)
sltu    t4, a0, a1            # CF: Set if dest < src (unsigned borrow)

xor     t5, a0, a1
xor     t2, a0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, a0, a1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify memory.

# Main result (Calculated but NOT written back)
ld      t0, 0(a0)             # load 64-bit qword from RAM
sub     t3, t0, a1            # full 64-bit math (RAM - src)

# Flags (Arithmetic Simulation)
sltu    t4, t0, a1            # CF: Set if RAM < src (unsigned borrow)

xor     t5, t0, a1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, t0, a1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r8, r/m8 (opcode 3A /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # isolate dest low byte
andi    t1, a1, 0xff          # isolate src low byte
sub     t3, t0, t1            # t3 = dest - src
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
andi    t0, a0, 0xff          # isolate dest low byte
lbu     t1, 0(a1)             # load src byte from RAM
sub     t3, t0, t1            # t3 = dest - RAM
andi    t3, t3, 0xff          # mask result to 8 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < RAM (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 7             # OF: 8-bit Signed overflow (bit 7)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 7             # SF = bit 7
seqz    t7, t3                # ZF

mv      t9, t3                # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r16, r/m16 (opcode 66 3B /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # isolate 16-bit dest
and     t1, a1, t2            # isolate 16-bit src
sub     t3, t0, t1            # t3 = dest - src
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
li      t2, 0xffff
and     t0, a0, t2            # isolate 16-bit dest
lhu     t1, 0(a1)             # load src 16-bit word from RAM
sub     t3, t0, t1            # t3 = dest - RAM
and     t3, t3, t2            # mask result to 16 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < RAM (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 15            # OF: 16-bit Signed overflow (bit 15)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 15            # SF = bit 15
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r32, r/m32 (opcode 3B /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
slli    t1, a1, 32
srli    t1, t1, 32            # load zero-extended src
sub     t3, t0, t1            # t3 = dest - src
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < src (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
slli    t0, a0, 32
srli    t0, t0, 32            # load zero-extended dest
lw      t1, 0(a1)             # load src 32-bit dword from RAM
slli    t1, t1, 32
srli    t1, t1, 32            # zero-extend before unsigned comparison
sub     t3, t0, t1            # t3 = dest - RAM
slli    t3, t3, 32
srli    t3, t3, 32            # mask result to 32 bits

# Flags (Arithmetic Simulation)
sltu    t4, t0, t1            # CF: Set if dest < RAM (unsigned borrow)

xor     t5, t0, t1
xor     t2, t0, t3
and     t5, t5, t2
srli    t5, t5, 31            # OF: 32-bit Signed overflow (bit 31)
andi    t5, t5, 1             

xor     t8, t0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 31            # SF = bit 31
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
## CMP r64, r/m64 (opcode REX.W + 3B /r)
### Register Source
```asm
# Inputs:   a0 = x86 register (destination, full 64-bit), a1 = x86 register (source)
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
sub     t3, a0, a1            # full 64-bit math (dest - src)

# Flags (Arithmetic Simulation)
sltu    t4, a0, a1            # CF: Set if dest < src (unsigned borrow)

xor     t5, a0, a1
xor     t2, a0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, a0, a1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
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
# Outputs:  {t4-t9} = {CF, OF, AF, SF, ZF, PF}
# Note: CMP does not modify the destination register.

# Main result (Calculated but NOT written back)
ld      t1, 0(a1)             # load src 64-bit qword from RAM
sub     t3, a0, t1            # full 64-bit math (dest - RAM)

# Flags (Arithmetic Simulation)
sltu    t4, a0, t1            # CF: Set if dest < RAM (unsigned borrow)

xor     t5, a0, t1
xor     t2, a0, t3
and     t5, t5, t2
srli    t5, t5, 63            # OF: 64-bit Signed overflow (bit 63)
andi    t5, t5, 1             

xor     t8, a0, t1
xor     t8, t8, t3
srli    t8, t8, 4
andi    t8, t8, 1             # AF: Half-carry (borrow out of bit 3)

srli    t6, t3, 63            # SF = bit 63
seqz    t7, t3                # ZF

andi    t9, t3, 0xff          # PF
srli    t2, t9, 4
xor     t9, t9, t2
srli    t2, t9, 2
xor     t9, t9, t2
srli    t2, t9, 1
xor     t9, t9, t2
andi    t9, t9, 1
xori    t9, t9, 1
```
