/* This file was automatically generated.  Do not edit! */
#ifndef CHIP8_8
#define CHIP8_8
#include "sdl_helpers.h"
void emulateCycle();
void initialize(event*, const char*);
void load(char *name);
unsigned int fetch();
void decode(unsigned short op);
void illegal();
void LD_0xFx65(unsigned char x);
void LD_I_0xFx55(unsigned char x);
void LD_B_0xFx33(unsigned char x);
void LD_F_0xFx29(unsigned char x);
void ADD_I_0xFx1E(unsigned char x);
void LD_ST_0xFx18(unsigned char x);
void LD_DT_0xFx15(unsigned char x);
void LD_0xFx0A(unsigned char x);
void LD_0xFx07(unsigned char x);
void SKNP_0xExA1(unsigned char x);
void SKP_0xEx9E(unsigned char x);
void DRW_0xDxyn(unsigned char x,unsigned char y,unsigned char n);
void RND_0xCxkk(unsigned char x,unsigned char kk);
void JP_0xBnnn(unsigned short nnn);
void LD_I_0xAnnn(unsigned short nnn);
void SNE_0x9xy0(unsigned char x,unsigned char y);
void SHL_0x8xyE(unsigned char x);
void SUBN_0x8xy7(unsigned char x,unsigned char y);
void SHR_0x8xy6(unsigned char x);
void SUB_0x8xy5(unsigned char x,unsigned char y);
void ADD_0x8xy4(unsigned char x,unsigned char y);
void XOR_0x8xy3(unsigned char x,unsigned char y);
void AND_0x8xy2(unsigned char x,unsigned char y);
void OR_0x8xy1(unsigned char x,unsigned char y);
void LD_0x8xy0(unsigned char x,unsigned char y);
void ADD_0x7xkk(unsigned char x,unsigned char kk);
void SE_0x6xkk(unsigned char x,unsigned char kk);
void SE_0x5xy0(unsigned char x,unsigned char y);
void SNE_0x4xkk(unsigned char x,unsigned char kk);
void SE_0x3xkk(unsigned char x,unsigned char kk);
void CALL(unsigned short addr);
void JP(unsigned short addr);
void RET(void);
void CLR(void);
void print_screen();
void epoll(void);

extern uint32_t display[32][64];
extern unsigned short opcode;

extern unsigned short stack[16];
extern unsigned char sp;
extern unsigned short pc;

extern unsigned short I;
extern unsigned char V[16];
extern unsigned char mem[4096];


#endif
