# NOT
## NOT r/m8 (opcode F6 /2)
### Register
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
andi    t0, a0, 0xff
xori    t0, t0, 0xff
andi    a0, a0, -256
or      a0, a0, t0
```
### Memory
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
lbu     t0, 0(a0)
xori    t0, t0, 0xff
sb      t0, 0(a0)
```
## NOT r/m16 (opcode F7 /2)
### Register
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
li      t1, 0xffff
and     t0, a0, t1
xor     t0, t0, t1 
li      t1, -65536              # 0xFFFFFFFFFFFF0000
and     a0, a0, t1
or      a0, a0, t0
```
### Memory
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
lhu     t0, 0(a0)
li      t1, 0xffff
xor     t0, t0, t1
sh      t0, 0(a0)
```
## NOT r/m32 (opcode F7 /2)
### Register
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
slli    a0, a0, 32
srli    a0, a0, 32
li      t0, 0xffffffff
xor     a0, a0, t0
slli    a0, a0, 32
srli    a0, a0, 32
```
### Memory
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
lwu     t0, 0(a0)
li      t1, 0xffffffff
xor     t0, t0, t1
sw      t0, 0(a0)
```
## NOT r/m32 (opcode REX.W + F7 /2)
### Register
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
not     a0, a0
```
### Memory
```asm
# Inputs:   a0 = x86 register
# Outputs:  a0 = result
ld      t0, 0(a0)
not     t0, t0
sd      t0, 0(a0)
```
