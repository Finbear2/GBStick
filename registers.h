#ifndef REGISTERS
#define REGISTERS

struct Registers {
  struct {
    // Using unions as they can write separately but share the same memory and
    // registers are often combined to make a 16bit register
    union {
      struct {
        unsigned char f;
        unsigned char a;
      };
      unsigned short af;
    };
  };

  struct {
    union {
      struct {
        unsigned char c;
        unsigned char b;
      };
      unsigned short bc;
    };
  };

  struct {
    union {
      struct {
        unsigned char e;
        unsigned char d;
      };
      unsigned short de;
    };
  };

  struct {
    union {
      struct {
        unsigned char l;
        unsigned char h;
      };
      unsigned short hl;
    };
  };

  unsigned short sp;
  unsigned short pc;
} extern registers;

extern struct Registers registers;

// The F register holds four flags in its top 4 bits
#define FLAG_Z 0x80 // result was zero      
#define FLAG_N 0x40 // last op was subtract 
#define FLAG_H 0x20 // half carry           
#define FLAG_C 0x10 // carry                

#endif