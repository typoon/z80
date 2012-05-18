#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "general.h"

#include "z80.h"

uchar fetch_opcode(z80 *z) {
    
    return z->m[z->pc++].data;

}

inline void set_flags_inc_8bits(z80 *z, uchar r) {

    uchar old = r - 1;

    if((r & 0x80) == 1) { // Negative number
        z->f.s = 1;
    } else {
        z->f.s = 0;
    }

    if(r == 0) {
        z->f.z = 1;
    } else {
        z->f.z = 0;
    }

    if(((old & 0x07) + (0x01)) > 0x07) { // Half Carry
        z->f.h = 1;
    } else {
        z->f.h = 0;
    }

    if(old == 0x80) {
        z->f.pv = 1;
    } else {
        z->f.pv = 0;
    }

    z->f.n = 1;

}

inline void set_flags_dec_8bits(z80 *z, uchar r) {

    uchar old = r + 1;

    if((r & 0x80) == 1) { // Negative number
        z->f.s = 1;
    } else {
        z->f.s = 0;
    }

    if(r == 0) {
        z->f.z = 1;
    } else {
        z->f.z = 0;
    }


    // TODO: Not sure... will there ever be a borrow from a dec?
    z->f.h = 0;

    if(old == 0x7F) {
        z->f.pv = 1;
    } else {
        z->f.pv = 0;
    }

    z->f.n = 0;

}

inline void set_flags_add_16bits(z80 *z, unsigned short r1, unsigned short r2) {

    unsigned int uiTmp = r1 + r2;

    if((uiTmp & 0x1000) == 1) {
        z->f.c = 1;
    } else {
        z->f.c = 0;
    }

    uiTmp = (r1 & 0x7FF) + (r2 & 0x7FF);

    if((uiTmp & 0x800) == 1) {
        z->f.h = 1;
    } else {
        z->f.h = 0;
    }

    z->f.n = 0;

}

/**
  * Checks if any flags need to be setup for this sum and changes it.
  * It should be called before the sum is made
  */

inline void set_flags_add_8bits(z80 *z, uchar r1, uchar r2) {

    unsigned short usTmp = r1 + r2;

    if(usTmp > 0x7F) {
        z->f.s = 1;
    } else {
        z->f.s = 0;
    }

    if((usTmp & 0x00FF) == 0) {
        z->f.z = 1;
    } else {
        z->f.z = 0;
    }

    if(((r1 & 0x07) + (r2 & 0x07)) > 0x07) { // Half Carry
        z->f.h = 1;
    } else {
        z->f.h = 0;
    }

    if(usTmp > 0xFF) {
        z->f.pv = 1;
    } else {
        z->f.pv = 0;
    }

    z->f.n = 0;

    if(usTmp > 0xFF) {
        z->f.c = 1;
    } else {
        z->f.c = 0;
    }


}

char decode_exec_opcode(z80 *z, uchar opcode) {

    char ret = SUCCESS;
    uchar ucTmp;
    uchar ucTmp2;
    unsigned short usTmp;
    char  cTmp;

    switch(opcode) {
        case 0x00:
            // NOP
            z->cycles += 4;
        break;
        
        case 0x01:
            // LD BC,nn
            z->c = z->m[z->pc++].data;
            z->b = z->m[z->pc++].data;
            z->cycles += 10;
        break;
            
        case 0x02:
            // LD (BC),A
            z->m[z->bc].data = z->a;
            z->m[z->bc].type = R_WR;
            z->cycles += 7;
        break;

        case 0x03: // INC BC
            // TODO: What happens if BC = 0xFFFF?
            z->bc++;

            z->cycles += 6;
        break;

        case 0x04:
            // INC B
            // TODO: What happens if B = 0xFF?
            z->b++;
            set_flags_inc_8bits(z, z->b);

            z->cycles += 4;
        break;

        case 0x05:
            // DEC B
            // TODO: What happens if B = 0x00?
            z->b--;
            set_flags_dec_8bits(z, z->b);
            z->cycles += 4;
        break;

        case 0x06:
            // LD B,n
            z->b = z->m[z->pc++].data;
            z->cycles += 7;
        break;

        case 0x07:
            // RLCA

            ucTmp = z->a;
            z->a <<= 1;
            z->f.c = (ucTmp & 0x80) >> 7;

            if((ucTmp & 0x80) == 1)
                z->a |= 0x01; // Set least significant bit
            else
                z->a &= 0xFE; // Clear least significant bit

            z->cycles += 4;
        break;

        case 0x08:
            // EX AF,AF’
            ucTmp = z->f2.flags;
            z->f2.flags = z->f.flags;
            z->f.flags = ucTmp;
            z->cycles += 4;
        break;

        case 0x09:
            // ADD HL, BC
            z->hl = z->hl + z->bc;
            set_flags_add_16bits(z, z->hl, z->bc);

            z->cycles += 11;
        break;

        case 0x0A:
            // LD A,(BC)
            z->a = z->m[z->bc].data;
            z->cycles += 7;
        break;

        case 0x0B:
            // DEC BC
            // TODO: What happens if BC = 0x00?
            z->bc--;
            z->cycles += 6;
        break;

        case 0x0C:
            // INC C
            z->c++;
            set_flags_inc_8bits(z, z->c);
            z->cycles += 4;
        break;

        case 0x0D:
            // DEC C
            // TODO: What happens if C = 0x00?
            z->c--;
            set_flags_dec_8bits(z, z->c);
            z->cycles += 4;
        break;

        case 0x0E:
            // LD C, n
            z->c = z->m[z->pc++].data;
            z->cycles += 7;
        break;

        case 0x0F:
            // RRCA
            ucTmp = z->a;
            z->a >>= 1;
            z->f.c = ucTmp & 0x01;

            if((ucTmp & 0x01) == 1)
                z->a |= 0x80; // Set most significant bit
            else
                z->a &= 0x7F; // Clear most significant bit

            z->cycles += 4;
        break;

        case 0x10:
            // DJNZ (PC+e)

            z->b--;
            if(z->b != 0) {
                cTmp = z->m[z->pc++].data;
                z->pc = z->pc + cTmp; // Signed char for 2 complement's
                z->cycles += 13;
            } else {
                z->pc++; // Skip the jump offset
                z->cycles += 8;
            }

        break;

        case 0x11:
            // LD DE,nn
            z->e = z->m[z->pc++].data;
            z->d = z->m[z->pc++].data;
            z->cycles += 10;
        break;

        case 0x12:
            // LD (DE), A
            z->m[z->de].data = z->a;
            z->cycles += 7;

        break;

        case 0x13:
            // INC DE
            // TODO: What happens if DE = 0xFFFF?
            z->de++;
            z->cycles += 6;
        break;

        case 0x14:
            // INC D
            // TODO: What happens if D = 0xFF?
            z->d++;
            set_flags_inc_8bits(z, z->d);
            z->cycles += 4;
        break;

        case 0x15:
            // DEC D
            // TODO: What happens if D = 0x00?
            z->d--;
            set_flags_dec_8bits(z, z->d);
            z->cycles += 4;
        break;

        case 0x16:
            // LD D,n
            z->d = z->m[z->pc++].data;
            z->cycles += 7;
        break;

        case 0x17:
            // RLA
            z->f.h = 0;
            z->f.n = 0;

            ucTmp = z->c;
            z->f.c = (z->a & 0x01) >> 7; // Carry is now msb of accumulator
            z->a <<= 1;

            // Old carry is now least significant bit of accumulator
            if(ucTmp == 1)
                z->a |= 0x01; // Set last bit
            else
                z->a &= 0xFE; // Reset last bit

            z->cycles += 4;
        break;

        case 0x18:
            // JR (PC+e)

            cTmp = z->m[z->pc++].data;
            z->pc = z->pc + cTmp; // Sum with 2's complement
            z->cycles += 12;
        break;

        case 0x19:
            // ADD HL, DE

            z->hl = z->hl + z->de;
            set_flags_add_16bits(z, z->hl, z->de);

            z->cycles += 11;
        break;

        case 0x1A:
            // LD A,(DE)
            z->a = z->m[z->de].data;
            z->cycles += 7;
        break;

        case 0x1B:
            // DEC DE
            // TODO: What happens if DE = 0x0000?
            z->de--;
            z->cycles += 6;
        break;

        case 0x1C:
            // INC E
            // TODO: What happens if E = 0xFF?
            z->e++;
            set_flags_inc_8bits(z, z->e);
            z->cycles += 4;
        break;

        case 0x1D:
            // DEC E
            // TODO: What happens if E = 0x00?
            z->e--;
            set_flags_dec_8bits(z, z->e);
            z->cycles += 4;
        break;

        case 0x1E:
            // LD E,n
            z->e = z->m[z->pc++].data;
            z->cycles += 7;
        break;

        case 0x1F:
            // RRA
            z->f.h = 0;
            z->f.n = 0;

            ucTmp = z->c;
            z->f.c = z->a & 0x01; // Carry is now lsb of accumulator
            z->a >>= 1;

            // Old carry is now least significant bit of accumulator
            if(ucTmp == 1)
                z->a |= 0x80; // Set msb
            else
                z->a &= 0x7F; // Reset msb

            z->cycles += 4;
        break;

        case 0x20:
            // JR NZ,(PC+e)

            if(z->f.z == 0) {
                cTmp = z->m[z->pc++].data;
                z->pc = z->pc + cTmp;
                z->cycles += 12;
            } else {
                z->pc++;
                z->cycles += 7;
            }

        break;

        case 0x21:
            // LD HL,nn
            z->l = z->m[z->pc++].data;
            z->h = z->m[z->pc++].data;
            z->cycles += 10;
        break;

        case 0x22:
            // LD (nn), HL
            // TODO: Improve this thing... It looks terrible
            ucTmp  = z->m[z->pc++].data;   // Low  order byte
            ucTmp2 = z->m[z->pc++].data;   // High order byte
            usTmp = ucTmp2;
            usTmp <<= 8;
            usTmp |= ucTmp;

            z->m[usTmp].data = z->hl;
            z->cycles += 16;
        break;

        case 0x23:
            // INC HL
            // TODO: What happens when HL = 0x0FFFF?
            z->hl++;
            z->cycles += 6;
        break;

        case 0x24:
            // INC H
            // TODO: What happens when H = 0xFF?
            z->h++;
            set_flags_inc_8bits(z, z->h);
            z->cycles += 4;
        break;

        case 0x25:
            // DEC H
            // TODO: What happens when H = 0x00?
            z->h--;
            set_flags_dec_8bits(z, z->h);
            z->cycles += 4;
        break;

        case 0x26:
            // LD H,n
            z->h = z->m[z->pc++].data;
            z->cycles += 7;
        break;

        case 0x27:
            // DAA
            // TODO: Implement me
            printf("Opcode DAA: Implement me!");
            z->cycles += 4;
        break;

        case 0x28:
            // JR Z,(PC+e)
            if(z->f.z == 1) {
                cTmp = z->m[z->pc++].data;
                z->pc = z->pc + cTmp;
                z->cycles += 12;
            } else {
                z->pc++;
                z->cycles += 7;
            }
        break;

        case 0x29:
            // ADD HL,HL
            z->hl = z->hl + z->hl;
            set_flags_add_16bits(z, z->hl, z->hl);

            z->cycles += 11;
        break;

        case 0x2A:
            // LD HL, (nn)
            ucTmp  = z->m[z->pc++].data;   // Low  order byte
            z->hl = z->m[(z->m[z->pc++].data << 8) | ucTmp].data;

            z->cycles += 16;
        break;

        case 0x2B:
            // DEC HL
            // TODO: What happens when HL = 0x0000?
            z->hl--;

            z->cycles += 6;
        break;

        case 0x2C:
            // INC L
            // TODO: What happens when L = 0xFF?
            set_flags_inc_8bits(z, z->l);
            z->l++;

            z->cycles += 4;
        break;

        case 0x2D:
            // DEC L
            // TODO: What happens when L = 0x00?
            z->l--;
            set_flags_dec_8bits(z, z->l);
            z->cycles += 4;
        break;

        case 0x2E:
            // LD L, n
            z->l = z->m[z->pc++].data;

            z->cycles += 7;
        break;

        case 0x2F:
            // CPL
            z->a = ~z->a;
            z->f.h = 1;
            z->f.n = 1;

            z->cycles += 4;
        break;

        case 0x30:
            // JR NC, (PC+e)
            if(z->f.c == 0) {
                cTmp = z->m[z->pc++].data;
                z->pc = z->pc + cTmp;
                z->cycles += 12;
            } else {
                z->pc++;
                z->cycles += 7;
            }
        break;

        case 0x31:
            // LD SP,nn
            ucTmp = z->m[z->pc++].data; // low
            z->sp = (z->m[z->pc++].data << 8) | ucTmp;

            z->cycles += 10;
        break;

        case 0x32:
            // LD (nn), A
            ucTmp = z->m[z->pc++].data; // low
            z->m[(z->m[z->pc++].data << 8) | ucTmp].data = z->a;

            z->cycles += 13;
        break;

        case 0x33:
            // INC SP
            z->sp++;

            z->cycles += 6;
        break;

        case 0x34:
            // INC (HL)
            z->m[z->hl].data++;

            z->cycles += 11;
        break;

        case 0x35:
            // DEC (HL)
            z->m[z->hl].data--;

            z->cycles += 11;
        break;

        case 0x36:
            // LD (HL), n
            z->m[z->hl].data = z->m[z->pc++].data;

            z->cycles += 10;
        break;

        case 0x37:
            // SCF
            z->f.c = 1;
            z->f.h = 0;
            z->f.n = 0;
            z->cycles += 4;
        break;

        case 0x38:
            // JR C,(PC+e)
            if(z->f.c == 1) {
                cTmp = z->m[z->pc++].data;
                z->pc = z->pc + cTmp;
                z->cycles += 12;
            } else {
                z->pc++;
                z->cycles += 7;
            }
        break;

        case 0x39:
            // ADD HL,SP
            z->hl = z->hl + z->sp;
            set_flags_add_16bits(z, z->hl, z->sp);

            z->cycles += 11;
        break;

        case 0x3A:
            // LD A,(nn)
            ucTmp = z->m[z->pc++].data; // low
            z->a = z->m[(z->m[z->pc++].data << 8) | ucTmp].data;

            z->cycles += 13;
        break;

        case 0x3B:
            // DEC SP
            z->sp--;
            z->cycles += 6;
        break;

        case 0x3C:
            // INC A
            z->a++;
            set_flags_inc_8bits(z, z->a);

            z->cycles += 4;
        break;

        case 0x3D:
            // DEC A
            z->a--;
            set_flags_dec_8bits(z, z->a);

            z->cycles += 0;
        break;

        case 0x3E:
            // LD A,n
            z->a = z->m[z->pc++].data;
            z->cycles += 7;
        break;

        case 0x3F:
            // CCF
            z->f.h = z->f.c;
            z->f.c = 0;
            z->f.n = 0;
            z->cycles += 4;
        break;

        case 0x40:
            // LD B,B
            z->b = z->b;
            z->cycles += 4;
        break;

        case 0x41:
            // LD B,C
            z->b = z->c;
            z->cycles += 4;
        break;

        case 0x42:
            // LD B,D
            z->b = z->d;
            z->cycles += 4;
        break;

        case 0x43:
            // LD B,E
            z->b = z->e;
            z->cycles += 4;
        break;

        case 0x44:
            // LD B,H
            z->b = z->h;
            z->cycles += 4;
        break;

        case 0x45:
            // LD B,L
            z->b = z->l;
            z->cycles += 4;
        break;

        case 0x46:
            // LD B,(HL)
            z->b = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x47:
            // LD B,A
            z->b = z->a;
            z->cycles += 4;
        break;

        case 0x48:
            // LD C,B
            z->c = z->b;
            z->cycles += 4;
        break;

        case 0x49:
            // LD C,C
            z->c = z->c;
            z->cycles += 4;
        break;

        case 0x4A:
            // LD C,D
            z->c = z->d;
            z->cycles += 4;
        break;

        case 0x4B:
            // LD C,E
            z->c = z->e;
            z->cycles += 4;
        break;

        case 0x4C:
            // LD C,H
            z->c = z->h;
            z->cycles += 4;
        break;

        case 0x4D:
            // LD C,L
            z->c = z->l;
            z->cycles += 4;
        break;

        case 0x4E:
            // LD C,(HL)
            z->c = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x4F:
            // LD C,A
            z->c = z->a;
            z->cycles += 4;
        break;

        case 0x50:
            // LD D,B
            z->d = z->b;
            z->cycles += 4;
        break;

        case 0x51:
            // LD D,C
            z->d = z->c;
            z->cycles += 4;
        break;

        case 0x52:
            // LD D,D
            z->d = z->d;
            z->cycles += 4;
        break;

        case 0x53:
            // LD D,E
            z->d = z->e;
            z->cycles += 4;
        break;

        case 0x54:
            // LD D,H
            z->d = z->h;
            z->cycles += 4;
        break;

        case 0x55:
            // LD D,L
            z->d = z->l;
            z->cycles += 4;
        break;

        case 0x56:
            // LD D,(HL)
            z->d = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x57:
            // LD D,A
            z->d = z->a;
            z->cycles += 4;
        break;

        case 0x58:
            // LD E,B
            z->e = z->b;
            z->cycles += 4;
        break;

        case 0x59:
            // LD E,C
            z->e = z->c;
            z->cycles += 4;
        break;

        case 0x5A:
            // LD E,D
            z->e = z->d;
            z->cycles += 4;
        break;

        case 0x5B:
            // LD E,E
            z->e = z->e;
            z->cycles += 4;
        break;

        case 0x5C:
            // LD E,H
            z->e = z->h;
            z->cycles += 4;
        break;

        case 0x5D:
            // LD E,L
            z->e = z->l;
            z->cycles += 4;
        break;

        case 0x5E:
            // LD E,(HL)
            z->e = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x5F:
            // LD E,A
            z->e = z->a;
            z->cycles += 4;
        break;

        case 0x60:
            // LD H,B
            z->h = z->b;
            z->cycles += 4;
        break;

        case 0x61:
            // LD H,C
            z->h = z->c;
            z->cycles += 4;
        break;

        case 0x62:
            // LD H,D
            z->h = z->d;
            z->cycles += 4;
        break;

        case 0x63:
            // LD H,E
            z->h = z->e;
            z->cycles += 4;
        break;

        case 0x64:
            // LD H,H
            z->h = z->h;
            z->cycles += 4;
        break;

        case 0x65:
            // LD H,L
            z->h = z->l;
            z->cycles += 4;
        break;

        case 0x66:
            // LD H,(HL)
            z->h = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x67:
            // LD H,A
            z->h = z->a;
            z->cycles += 4;
        break;

        case 0x68:
            // LD L,B
            z->l = z->b;
            z->cycles += 4;
        break;

        case 0x69:
            // LD L,C
            z->l = z->c;
            z->cycles += 4;
        break;

        case 0x6A:
            // LD L,D
            z->l = z->d;
            z->cycles += 4;
        break;

        case 0x6B:
            // LD L,E
            z->l = z->e;
            z->cycles += 4;
        break;

        case 0x6C:
            // LD L,H
            z->l = z->h;
            z->cycles += 4;
        break;

        case 0x6D:
            // LD L,L
            z->l = z->l;
            z->cycles += 4;
        break;

        case 0x6E:
            // LD L,(HL)
            z->l = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x6F:
            // LD L,A
            z->l = z->a;
            z->cycles += 4;
        break;

        case 0x70:
            // LD (HL),B
            z->m[z->hl].data = z->b;
            z->cycles += 7;
        break;

        case 0x71:
            // LD (HL),C
            z->m[z->hl].data = z->c;
            z->cycles += 7;
        break;

        case 0x72:
            // LD (HL),D
            z->m[z->hl].data = z->d;
            z->cycles += 7;
        break;

        case 0x73:
            // LD (HL),E
            z->m[z->hl].data = z->e;
            z->cycles += 7;
        break;

        case 0x74:
            // LD (HL),H
            z->m[z->hl].data = z->h;
            z->cycles += 7;
        break;

        case 0x75:
            // LD (HL),L
            z->m[z->hl].data = z->l;
            z->cycles += 7;
        break;

        case 0x76:
            // HALT
            // TODO: Confirm how this works
            z->halt = 1;
            z->cycles += 4;
            z->pc--; // We repeat the halt until an interrupt happens
        break;

        case 0x77:
            // LD (HL),A
            z->m[z->hl].data = z->a;
            z->cycles += 7;
        break;

        case 0x78:
            // LD A,B
            z->a = z->b;
            z->cycles += 4;
        break;

        case 0x79:
            // LD A,C
            z->a = z->c;
            z->cycles += 4;
        break;

        case 0x7A:
            // LD A,D
            z->a = z->d;
            z->cycles += 4;
        break;

        case 0x7B:
            // LD A,E
            z->a = z->e;
            z->cycles += 4;
        break;

        case 0x7C:
            // LD A,H
            z->a = z->h;
            z->cycles += 4;
        break;

        case 0x7D:
            // LD A,B
            z->a = z->l;
            z->cycles += 4;
        break;

        case 0x7E:
            // LD A,(HL)
            z->a = z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x7F:
            // LD A,A
            z->a = z->a;
            z->cycles += 4;
        break;

        case 0x80:
            // ADD A,B
            set_flags_add_8bits(z, z->a, z->b);
            z->a += z->b;
            z->cycles += 4;
        break;

        case 0x81:
            // ADD A,C
            set_flags_add_8bits(z, z->a, z->c);
            z->a += z->c;
            z->cycles += 4;
        break;

        case 0x82:
            // ADD A,D
            set_flags_add_8bits(z, z->a, z->d);
            z->a += z->d;
            z->cycles += 4;
        break;

        case 0x83:
            // ADD A,E
            set_flags_add_8bits(z, z->a, z->e);
            z->a += z->e;
            z->cycles += 4;
        break;

        case 0x84:
            // ADD A,H
            set_flags_add_8bits(z, z->a, z->h);
            z->a += z->h;
            z->cycles += 4;
        break;

        case 0x85:
            // ADD A,L
            set_flags_add_8bits(z, z->a, z->l);
            z->a += z->l;
            z->cycles += 4;
        break;

        case 0x86:
            // ADD A,(HL)
            set_flags_add_8bits(z, z->a, z->m[z->hl].data);
            z->a += z->m[z->hl].data;
            z->cycles += 7;
        break;

        case 0x87:
            // ADD A,A
            set_flags_add_8bits(z, z->a, z->a);
            z->a += z->a;
            z->cycles += 4;
        break;

        case 0x88:
            // ADC A,B
            set_flags_add_8bits(z, z->a, z->b + z->f.c);
            z->a += z->b + z->f.c;
            z->cycles += 4;
        break;

        case 0x89:
            // ADC A,C
            set_flags_add_8bits(z, z->a, z->c + z->f.c);
            z->a += z->c + z->f.c;
            z->cycles += 4;
        break;

        case 0x8A:
            // ADC A,D
            set_flags_add_8bits(z, z->a, z->d + z->f.c);
            z->a += z->d + z->f.c;
            z->cycles += 4;
        break;

        case 0x8B:
            // ADC A,E
            set_flags_add_8bits(z, z->a, z->e + z->f.c);
            z->a += z->e + z->f.c;
            z->cycles += 4;
        break;

        case 0x8C:
            // ADC A,H
            set_flags_add_8bits(z, z->a, z->h + z->f.c);
            z->a += z->h + z->f.c;
            z->cycles += 4;
        break;

        case 0x8D:
            // ADC A,L
            set_flags_add_8bits(z, z->a, z->l + z->f.c);
            z->a += z->l + z->f.c;
            z->cycles += 4;
        break;

        case 0x8E:
            // ADC A,(HL)
            set_flags_add_8bits(z, z->a, z->m[z->hl].data + z->f.c);
            z->a += z->m[z->hl].data + z->f.c;
            z->cycles += 7;
        break;

        case 0x8F:
            // ADC A,A
            set_flags_add_8bits(z, z->a, z->a + z->f.c);
            z->a += z->a + z->f.c;
            z->cycles += 4;
        break;

        /*
        case 0x23:
            //
            z->cycles += 0;
        break;
        */

/*
90		SUB B			4	1	1
91		SUB C			4	1	1
92		SUB D			4	1	1
93		SUB E			4	1	1
94		SUB H			4	1	1
95		SUB L			4	1	1
96		SUB (HL)		7	2	1
97		SUB A			4	1	1
98		SBC A,B			4	1	1
99		SBC A,C			4	1	1
9A		SBC A,D			4	1	1
9B		SBC A,E			4	1	1
9C		SBC A,H			4	1	1
9D		SBC A,L			4	1	1
9E		SBC A,(HL)		7	2	1
9F		SBC A,A			4	1	1
A0		AND B			4	1	1
A1		AND C			4	1	1
A2		AND D			4	1	1
A3		AND E			4	1	1
A4		AND H			4	1	1
A5		AND L			4	1	1
A6		AND (HL)		7	2	1
A7		AND A			4	1	1
A8		XOR B			4	1	1
A9		XOR C			4	1	1
AA		XOR D			4	1	1
AB		XOR E			4	1	1
AC		XOR H			4	1	1
AD		XOR L			4	1	1
AE		XOR (HL)		7	2	1
AF		XOR A			4	1	1
B0		OR B			4	1	1
B1		OR C			4	1	1
B2		OR D			4	1	1
B3		OR E			4	1	1
B4		OR H			4	1	1
B5		OR L			4	1	1
B6		OR (HL)			7	2	1
B7		OR A			4	1	1
B8		CP B			4	1	1
B9		CP C			4	1	1
BA		CP D			4	1	1
BB		CP E			4	1	1
BC		CP H			4	1	1
BD		CP L			4	1	1
BE		CP (HL)			7	2	1
BF		CP A			4	1	1
C0		RET NZ			11/5	3/1	1/1	(met/not met)
C1		POP BC			10	3	1
C2 n n		JP NZ,(nn)		10	3	1	(met or not)
C3 n n		JP (nn)			10	3	1
C4 n n		CALL NZ,(nn)		17/10	5/3	1/1	(met/not met)
C5		PUSH BC			11	3	1
C6 n		ADD A,n			7	2	1
C7		RST 0H			11	3	1
C8		RET Z			11/5	3/1	1/1	(met/not met)
C9		RET			10	3	1
*/

/*
When a
Load Register A with Register I (LD A, I) instruction or a Load
Register A with Register R (LD A, R) instruction is executed, the state
of IFF2 is copied to the parity flag where it can be tested or stored.
*/

        default:
            ret = ERROR;
    }

    if(z->pc > z->allocated_memory-1) {
        z->halt = 1;
    }

    return ret;
}

z80* new_z80(double freq) {
    z80 *z;
    
    z = (z80 *)malloc(sizeof(z80));
    memset(z, 0, sizeof(z80));
    
    z->freq = freq;
    z->fetch = fetch_opcode;
    z->decode_exec = decode_exec_opcode;
    z->halt = 0;

    // Interrupts disabled when initialized
    // Use instruct EI to enable it
    z->iff1 = 0;
    z->iff2 = 0;
    z->im   = 0;
    
    return z;
}

