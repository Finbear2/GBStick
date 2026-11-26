Z Set if result is zero
N Always set It's subtraction
H Set if old lower nibble was zer
C Carry

```
if ((registers.e & 0x0F) == 0) {
  registers.f |= FLAG_H;
}
```
