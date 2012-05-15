#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "general.h"

#include "z80.h"

uchar fetch_opcode(z80 *z) {
    
    return z->m[z->pc++].data;

}

char decode_exec_opcode(z80 *z, uchar opcode) {

    char ret = SUCCESS;
    uchar ucTmp;
    char  cTmp;
    unsigned int uiTmp;

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

        case 0x03:
            // INC BC
            // TODO: What happens if BC = 0xFFFF?
            z->m[z->bc].data++;
            z->cycles += 6;
        break;

        case 0x04:
            // INC B
            // TODO: What happens if B = 0xFF?
            z->m[z->b].data++;
            z->cycles += 4;
        break;

        case 0x05:
            // DEC B
            // TODO: What happens if B = 0x00?
            z->m[z->b].data--;
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

            uiTmp = z->hl + z->bc;

            if((uiTmp & 0x1000) == 1) {
                z->f.c = 1;
            } else {
                z->f.c = 0;
            }

            uiTmp = (z->hl & 0x7FF) + (z->bc & 0x7FF);

            if((uiTmp & 0x800) == 1) {
                z->f.h = 1;
            } else {
                z->f.h = 0;
            }

            z->f.n = 0;


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
            // TODO: What happens if C = 0xFF?
            z->c++;
            z->cycles += 4;
        break;

        case 0x0D:
            // DEC C
            // TODO: What happens if C = 0x00?
            z->c--;
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
                z->pc += 2;
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




/*
13		INC DE			6	1	1
14		INC D			4	1	1
15		DEC D			4	1	1
16 n		LD D,n			7	2	1
17		RLA			4	1	1
18 e		JR (PC+e)		12	3	1
19		ADD HL,DE		11	3	1
1A		LD A,(DE)		7	2	1
1B		DEC DE			6	1	1
1C		INC E			4	1	1
1D		DEC E			4	1	1
1E n		LD E,n			7	2	1
1F		RRA			4	1	1
20 e		JR NZ,(PC+e)		12/7	3/2	1/1	(met/not met)
21 n n		LD HL,nn		10	3	1
22 n n		LD (nn),HL		16	5	3
23		INC HL			6	1	1
24		INC H			4	1	1
25		DEC H			4	1	1
26 n		LD H,n			7	2	1
27		DAA			4	1	1
28 e		JR Z,(PC+e)		12/7	3/2	1/1	(met/not met)
29		ADD HL,HL		11	3	1
2A n n		LD HL,(nn)		16	5	1
2B		DEC HL			6	1	1
2C		INC L			4	1	1
2D		DEC L			4	1	1
2E n		LD L,n			7	2	1
2F		CPL			4	1	1
30 e		JR NC,(PC+e)		12/7	3/2	1/1	(met/not met)
31 n n		LD SP,nn		10	3	1
32 n n		LD (nn),A		13	4	1
33		INC SP			6	1	1
34		INC (HL)		11	3	1
35		DEC (HL)		11	3	1
36 n		LD (HL),n		10	3	1
37		SCF			4	1	1
38 e		JR C,(PC+e)		12/7	3/2	1/1	(met/not met)
39		ADD HL,SP		11	3	1
3A n n		LD A,(nn)		13	4	1
3B		DEC SP			6	1	1
3C		INC A			4	1	1
3D		DEC A			4	1	1
3E n		LD A,n			7	2	1
3F		CCF			4	1	1
40		LD B,B			4	1	1
41		LD B,C			4	1	1
42		LD B,D			4	1	1
43		LD B,E			4	1	1
44		LD B,H			4	1	1
45		LD B,L			4	1	1
46		LD B,(HL)		7	2	1
47		LD B,A			4	1	1
48		LD C,B			4	1	1
49		LD C,C			4	1	1
4A		LD C,D			4	1	1
4B		LD C,E			4	1	1
4C		LD C,H			4	1	1
4D		LD C,L			4	1	1
4E		LD C,(HL)		7	2	1
4F		LD C,A			4	1	1
50		LD D,B			4	1	1
51		LD D,C			4	1	1
52		LD D,D			4	1	1
53		LD D,E			4	1	1
54		LD D,H			4	1	1
55		LD D,L			4	1	1
56		LD D,(HL)		7	2	1
57		LD D,A			4	1	1
58		LD E,B			4	1	1
59		LD E,C			4	1	1
5A		LD E,D			4	1	1
5B		LD E,E			4	1	1
5C		LD E,H			4	1	1
5D		LD E,L			4	1	1
5E		LD E,(HL)		7	2	1
5F		LD E,A			4	1	1
60		LD H,B			4	1	1
61		LD H,C			4	1	1
62		LD H,D			4	1	1
63		LD H,E			4	1	1
64		LD H,H			4	1	1
65		LD H,L			4	1	1
66		LD H,(HL)		7	2	1
67		LD H,A			4	1	1
68		LD L,B			4	1	1
69		LD L,C			4	1	1
6A		LD L,D			4	1	1
6B		LD L,E			4	1	1
6C		LD L,H			4	1	1
6D		LD L,L			4	1	1
6E		LD L,(HL)		7	2	1
6F		LD L,A			4	1	1
70		LD (HL),B		7	2	1
71		LD (HL),C		7	2	1
72		LD (HL),D		7	2	1
73		LD (HL),E		7	2	1
74		LD (HL),H		7	2	1
75		LD (HL),L		7	2	1
76		HALT			4	1	1	(repeated till next int)
77		LD (HL),A		7	2	1
78		LD A,B			4	1	1
79		LD A,C			4	1	1
7A		LD A,D			4	1	1
7B		LD A,E			4	1	1
7C		LD A,H			4	1	1
7D		LD A,L			4	1	1
7E		LD A,(HL)		7	2	1
7F		LD A,A			4	1	1
80		ADD A,B			4	1	1
81		ADD A,C			4	1	1
82		ADD A,D			4	1	1
83		ADD A,E			4	1	1
84		ADD A,H			4	1	1
85		ADD A,L			4	1	1
86		ADD A,(HL)		7	2	1
87		ADD A,A			4	1	1
88		ADC A,B			4	1	1
89		ADC A,C			4	1	1
8A		ADC A,D			4	1	1
8B		ADC A,E			4	1	1
8C		ADC A,H			4	1	1
8D		ADC A,L			4	1	1
8E		ADC A,(HL)		7	2	1
8F		ADC A,A			4	1	1
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
    
    return z;
}

