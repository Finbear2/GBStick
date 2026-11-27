- Z Set if result is zero
- N Always set It's subtraction
- H Set if old lower nibble was zer
- C Carry

```cpp
// DEC rule
if ((registers.e & 0x0F) == 0) {
  registers.f |= FLAG_H;
}

// INC rule
if ((registers.e & 0x0F) == 0x0F) {
  registers.f |= FLAG_H;
}
```

- |= Set Bits
- &= Keep Bits
- Registers: Small, fast storage locations inside the CPU.
- Memory: Where instructions and data are stored and retrieved.
- The Program Counter: Keeps track of which instruction the CPU should execute next.
- The Stack Pointer: Tracks the current position in the stack.
- Flags: Bits that record things like zero results, subtraction and carry.
- Opcodes: Instructions that tell the CPU what operation to perform.
- Clock cycles: How much simulated time an instruction takes.
