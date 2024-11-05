# jit-assistant
This project uses Keystone Assembler to assemble instructions by leaving "holes" in certain operands, to be replaced by a variable, which is specified along with its size using a special syntax. At the end, a C code is generated, intended to be the call to a code emitter, such as those commonly used in JIT compilers.

Example:

```$ ./ajit "x64" "mov rdi, |var1,64|" "emit_8"
emit_8(0x48);
emit_8(0xbf);
emit_64(var1);
```

```$ ./ajit "ppc64be" "addi |var1,5|,|var2,5|,|var3, 16|" "emit_8" "emit_16"
emit_8(0x38 | (var1 & 0x03));
emit_8(0x00 | (((var1 >> 2) & 0x07) << 5) | (var2 & 0x1f));
emit_16(var3);
```
The user must specify the maximum operand size, in bits. If there is a minimum size below which the instruction structure changes, it is the user's responsibility to ensure the specified variable does not fall below this size.

# Integration with text editors
