/* 
 * File:   z80.h
 * Author: gilgamesh
 *
 * Created on May 9, 2012, 3:35 PM
 */

#ifndef Z80_H
#define	Z80_H

#include "memory.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef unsigned char uchar;

typedef struct _z80 {

    // CPU Frequence in Hertz, represent the amount of T cycles
    double freq;
    
    // cycles that were already executed
    double cycles;
    
    
    uchar a; // accumulator

    union {
        struct { // flags
            unsigned c:1;             // Carry
            unsigned n:1;             // Add/Subtract
            unsigned pv:1;            // Parity/Overflow
            unsigned _not_used_1:1;
            unsigned h:1;             // Half Carry
            unsigned _not_used_2:1;
            unsigned z:1;            // Zero
            unsigned s:1;            // Sign
        };
        unsigned char flags;
    } f;

    uchar a2; // accumulator

    union {
        struct { // flags
            unsigned c:1;             // Carry
            unsigned n:1;             // Add/Subtract
            unsigned pv:1;            // Parity/Overflow
            unsigned _not_used_1:1;
            unsigned h:1;             // Half Carry
            unsigned _not_used_2:1;
            unsigned z:1;            // Zero
            unsigned s:1;            // Sign
        };
        unsigned char flags;
    } f2;
    
    // General purpose registers B and C
    union {
        struct {
            uchar b;
            uchar c;
        };
        short bc;
    };
    
    union {
        struct {
            uchar b2;
            uchar c2;
        };
        short bc2;
    };
    
    
    // General purpose registers D and E
    union {
        struct {
            uchar d;
            uchar e;
        };
        short de;
    };
    
    union {
        struct {
            uchar d2;
            uchar e2;
        };
        short de2;
    };
    
    // General purpose registers H and L
    union {
        struct {
            uchar h;
            uchar l;
        };
        short hl;
    };
    
    union {
        struct {
            uchar h2;
            uchar l2;
        };
        short hl2;
    };
    
    // Register IX
    union {
        struct {
            uchar ixh;
            uchar ixl;
        };
        short ix;
    };
    
    // Register IY
    union {
        struct {
            uchar iyh;
            uchar iyl;
        };
        short iy;
    };
    
    short pc; // program counter
    short sp; // stack pointer
    uchar i; // interrupt register
    uchar r; // refresh register

    // Not sure if this should be here
    short allocated_memory; // in bytes
    struct memory *m;
    
    uchar (*fetch)(struct _z80* z);
    char (*decode_exec)(struct _z80* z, uchar opcode);

    // Is CPU halted?
    char halt;
    
} z80;

z80* new_z80(double freq);

#ifdef	__cplusplus
}
#endif

#endif	/* Z80_H */

