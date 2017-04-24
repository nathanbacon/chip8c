
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "sdl_helpers.h"
#include "chip8.h"

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint32_t display[32][64];
unsigned short opcode;

 unsigned short stack[16];
 //stack pointer
 unsigned char sp;
 //program counter, used to store the currently executing address
 unsigned short pc;

 unsigned short I;
 // 1 byte registers
 unsigned char V[16];
 unsigned char mem[4096];

event* event_tracker;



//functions

void print_screen(){
	for(int row = 0; row < 32; row++){
		for(int column = 0; column < 64; column++)
			if(display[column][row]>0)
				printf(" ");
			else
				printf("0");
		printf("\n");
	}
}

void CLR(void){
	//clear the display
	memset((uint8_t*)display, 0x0, 64*32);
    for(int i = 0; i < 32; i++)
        for(int j = 0; j < 64; j++)
            display[i][j] = 0;
}

void RET(void){
	//set pc to the top of stack then decrement sp
	pc = stack[sp--];
}

void JP(unsigned short addr){
	//set pc to the address
	pc = addr;
}

void CALL(unsigned short addr){
	//increment the stack pointer, then place pc at sp + 1, then set pc to the subroutine at addr
	stack[++sp] = pc;
	pc = addr;
}

void SE_0x3xkk(unsigned char x, unsigned char kk){
	if(V[x] == kk)
		pc += 2;
}

void SNE_0x4xkk(unsigned char x, unsigned char kk){
	if(V[x] != kk)
		pc += 2;
}

void SE_0x5xy0(unsigned char x, unsigned char y){
	if(V[x] == V[y])
		pc += 2;
}

void SE_0x6xkk(unsigned char x, unsigned char kk){
	V[x] = kk;
}

void ADD_0x7xkk(unsigned char x, unsigned char kk){
	V[x] += kk;
}

void LD_0x8xy0(unsigned char x, unsigned char y){
	V[x] = V[y];
}

void OR_0x8xy1(unsigned char x, unsigned char y){
	//bitwise OR
	V[x] |= V[y];
}

void AND_0x8xy2(unsigned char x, unsigned char y){
	V[x] &= V[y];
}

void XOR_0x8xy3(unsigned char x, unsigned char y){
	V[x] ^= V[y];
}

void ADD_0x8xy4(unsigned char x, unsigned char y){
	if ((int)V[x] + (int)V[y] > 255)
		V[0xF] = 0x1;
	else
		V[0xF] = 0x0;
	V[x] += V[y];
}

void SUB_0x8xy5(unsigned char x, unsigned char y){
	
	if(V[x]>V[y])
		V[0xF] = 0x1;
	else
		V[0xF] = 0x0;

	V[x] -= V[y];
}

void SHR_0x8xy6(unsigned char x){

	V[0xF] = V[x] & 0x01;
	//divide by 2
	V[x] = V[x] >> 1;
}

void SUBN_0x8xy7(unsigned char x, unsigned char y){
	if(V[y] > V[x])
		V[0xF] = 0x1;
	else
		V[0xF] = 0x0;

	V[x] = V[y] - V[x];
}

void SHL_0x8xyE(unsigned char x){
	V[0xF] = V[x] >> 7;
	//multiply V[x] by two
	V[x] = V[x] << 1;
}

void SNE_0x9xy0(unsigned char x, unsigned char y){
	if(V[x] != V[y])
		pc += 2;
}

void LD_I_0xAnnn(unsigned short nnn){
	I = nnn;
}

void JP_0xBnnn(unsigned short nnn){
	pc = V[0x0] + nnn;
}

void RND_0xCxkk(unsigned char x, unsigned char kk){
	srand(time(NULL));
	V[x] = (unsigned char) rand() & kk;
    
}

void DRW_0xDxyn(unsigned char x, unsigned char y, unsigned char n){
	//x and y specify the x y coordinates of the screen, so it'll print starting at (V[x],V[y])
	//the value of I at the time of calling is the position from which we'll start reading the data to be printed
	
	V[0xF]=0x0;
	//start on top row, move down n rows
	for(unsigned char row = 0; row < n; row++){

		//each time col is incremented, move to the next column to the right
	
		for(unsigned char col = 0; col < 8; col++){

			//right shift the memory address n times and AND the byte with 0x00000001, which gives the nth least significant bit
			//since we draw in XOR mode, check and see if the sprite to be drawn collides with a pixel that is on

			if((mem[I+row] & (0x80 >> col)) != 0){ // 0x80 = 1000 0000, check to see if the the colth pixel from the left will be lit
				
				if(display[(row+V[y])%32][(col+V[x])%64] != 0){ //checks to see if the destination pixel is already lit
					V[0xF]=0x1; //if it is then by this conditional, the pixel is lit on screen and on the sprite so set the collision flag
                    display[(row+V[y])%32][(col+V[x])%64] = 0x00000000;
				}
                else
                    display[(row+V[y])%32][(col+V[x])%64] = 0xFFFFFFFF;
			}

		}
        
	}
	event_tracker->draw_flag = true;
}

void SKP_0xEx9E(unsigned char x){
    if(event_tracker->key[V[x]])
        pc += 2;

}

void SKNP_0xExA1(unsigned char x){

    if(!event_tracker->key[V[x]])
        pc += 2;
    
}

void LD_0xFx07(unsigned char x){
	V[x] = event_tracker->delay;
}

void LD_0xFx0A(unsigned char x){
	//stop execution until a key is pressed
    event_tracker->is_pressed = false;
	while(!event_tracker->is_pressed){
		//gotta poll events here until a key is pressed
		//need to remember to work on this once keys are implemented
        ePoll(event_tracker);
        usleep(10000);
	}
	//once key is pressed set V[x] to the first key in the key array that's true
    for(int i = 0; i < 16; i++) {
        if(event_tracker->key[i]){
            V[x] = i;
            break;
        }
    }
}

void LD_DT_0xFx15(unsigned char x){
	event_tracker->delay = V[x];
}

void LD_ST_0xFx18(unsigned char x){
	event_tracker->sound = 2*V[x];
}

void ADD_I_0xFx1E(unsigned char x){
	I += V[x];
}

void LD_F_0xFx29(unsigned char x){
	//originally I = V[x], this isn't right. This function set I = the location of the sprite of the number in V[x]
	//ex, V[x] = 1, should point to 0x0005, the location of the 1 sprite
	//each sprite is 5 bytes long so multiply the digit to be rendered by 5 to get its location
	I = V[x]*5;
	
}

void LD_B_0xFx33(unsigned char x){
	//ones digit
	mem[I+2] = V[x]%10;
	//tens digit
	mem[I+1] = (V[x]%100)/10;
	//hundreds digit
	mem[I] = V[x]/100;
}

void LD_I_0xFx55(unsigned char x){
	
	for(int i = 0; i <= x; i++){
		mem[I++] = V[i];
	}
}

void LD_0xFx65(unsigned char x){
	for(int i = 0; i <= x; i++){
		V[i] = mem[I++];
	}
	//I = I + x + 1;
}

void illegal(){
	printf("illegal opcode\n");
}

// give this an opcode, it'll return a function pointer of the next instruction
//it's type void until I figure out what the fuck a function pointer is

void decode(unsigned short op){

	switch(op & 0xF000) {
		case 0x0000:
			switch(op) {
				case 0x00E0:
					CLR();
					break;
				case 0x00EE:
					RET();
					break;
				default:
					//0x0NNN, not an operation that's used
					illegal();
					break;
			}
			break;
		case 0x1000:
			//JP addr-0x1NNN- set the program counter to NNN
			JP(op & 0x0FFF);
			break;
		case 0x2000:
			//0x2nnn-CALL addr
			//increment stack pointer, save the current pc on the stack, then set pc to 0x0nnn
			CALL(op & 0x0FFF);
			break;
		case 0x3000:
			//SE Vx, byte, 0x3xkk
			//skips next instruction if vx=kk
			// if vx=kk, increment pc by 2
			SE_0x3xkk((op & 0x0F00) >> 8, op & 0x00FF);
			break;
		case 0x4000:
			//0x4xkk
			//skip next instruction if vx != kk
			SNE_0x4xkk((op & 0x0F00) >> 8, op & 0x00FF);
			break;
		case 0x5000:
			//0x5xy0
			//skip next instruction if vx = vy
			SE_0x5xy0((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
			break;
		case 0x6000:
			//0x6xkk
			//set Vx = kk
			SE_0x6xkk((op & 0x0F00) >> 8, op & 0x00FF);
			break;
		case 0x7000:
			//0x7xkk
			//Vx = Vx + kk
			ADD_0x7xkk((op & 0x0F00) >> 8, op & 0x00FF);
			break;
		case 0x8000:
			switch(op & 0x000F){
				case 0x0000:
					//0x8xy0
					//LD Vx, Vy
					//Vx = Vy
					LD_0x8xy0((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x0001:
					//0x8xy1
					//OR Vx, Vy
					//Vx = Vx | Vy
					OR_0x8xy1((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x0002:
					//0x8xy2
					//AND Vx, Vy
					//Vx = Vx & Vy
					AND_0x8xy2((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x0003:
					//0x8xy3
					//XOR Vx, Vy
					//Vx = Vx ^ Vy
					XOR_0x8xy3((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x0004:
					//0x8xy4
					//ADD Vx, Vy
					//Vx = Vx + Vy
					ADD_0x8xy4((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x0005:
					//0x8xy5
					//Vx = Vx - Vy
					//If Vx>Vy, set VF to 1, otherwise 0. Then perform the subtraction and store in Vx
					SUB_0x8xy5((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x0006:
					//0x8xy6
					//if least significant bit of Vx is 1, Set VF to 1, otherwise 0. then Vx is divided by two by right shifting
					SHR_0x8xy6((op & 0x0F00) >> 8);
					break;
				case 0x0007:
					//0x8xy7
					// Vx = Vy - Vx
					// if Vy > Vx, set VF to 1, else 0. 
					SUBN_0x8xy7((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
					break;
				case 0x000E:
					//0x8xyE
					//set VF to the value of the most significant bit in Vx, then left shift Vx once
					SHL_0x8xyE((op & 0x0F00) >> 8);
					break;
				default:
					//illegal opcode
					illegal();
					break;
			}
			break;
		case 0x9000:
			//0x9xy0
			//if Vx != Vy, increment pc by 2
			SNE_0x9xy0((op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
			break;
		case 0xA000:
			//0xAnnn
			//set I = nnn
			LD_I_0xAnnn(op & 0x0FFF);
			break;
		case 0xB000:
			//set pc = V0 + nnn
			JP_0xBnnn(op & 0x0FFF);
			break;
		case 0xC000:
			//0xCxkk
			// Vx = random byte & kk
			//random number from 0 to 255
			RND_0xCxkk((op & 0x0F00) >> 8, op & 0x00FF);
			break;
		case 0xD000:
			//0xDxyn
			//Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

			DRW_0xDxyn((op & 0x0F00) >> 8, (op & 0x00F0) >> 4, op & 0x000F);
			break;
		case 0xE000:
			switch(op & 0x00FF){
				case 0x009E:
					//Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
					SKP_0xEx9E((op & 0x0F00) >> 8);
					break;
				case 0x00A1:
					//Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
					SKNP_0xExA1((op & 0x0F00) >> 8);
					break;
				default:
					//illegal opcode
					illegal();
					break;
			}
			break;
		case 0xF000:
			switch(op & 0x00FF){
				case 0x0007:
					//0xFx07
					//Vx = delay timer value
					LD_0xFx07((op & 0x0F00) >> 8);
					break;
				case 0x000A:
					//0xFx0A
					//Wait for a key press, then store key in Vx
					LD_0xFx0A((op & 0x0F00) >> 8);
					break;
				case 0x0015:
					//0xFx15
					//set delay timer = Vx
					LD_DT_0xFx15((op & 0x0F00) >> 8);
					break;
				case 0x0018:
					//0xFx18
					//set sound timer = Vx
					LD_ST_0xFx18((op & 0x0F00) >> 8);
					break;
				case 0x001E:
					//0xFx1E
					//I = I + Vx
					ADD_I_0xFx1E((op & 0x0F00) >> 8);
					break;
				case 0x0029:
					//0xFx29
					//The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx
					LD_F_0xFx29((op & 0x0F00) >> 8);
					break;
				case 0x0033:
					//0xFx33
					//Store BCD representation of Vx in memory locations I, I+1, and I+2.
					LD_B_0xFx33((op & 0x0F00) >> 8);
					break;
				case 0x0055:
					//0xFx55
					//store registers V0 through Vx in memory starting at location I
					LD_I_0xFx55((op & 0x0F00) >> 8);
					break;
				case 0x0065:
					//0xFx65
					//read values from memory starting at location I into V0 through Vx
					LD_0xFx65((op & 0x0F00) >> 8);
					break;
				default:
				//illegal opcode
				illegal();
				break;
			}
			break;
		default:
		//illegal opcode
		illegal();
		break;					
	}
}



unsigned int fetch(){

	unsigned short op = 0;

	//merge the next two bytes of memory into the opcode
	op = mem[pc];
	//printf("%#06X\n",op);
	//left shift 8 bits
	//bitwise or with the next address in memory
	op = op << 8;
	//printf("%#06X\n",op);
	op = op | (mem[pc+1] & 0x00FF);
	pc += 2;
	return op;

}

void load(char* name){
	
	FILE* file = fopen(name, "rb");
    
	uint8_t * c = (uint8_t*)malloc(sizeof(uint8_t));
	unsigned int counter = 0x200;
	do {
		*c = fgetc(file);
		mem[counter] = *c;
		counter++;
	} while(!feof(file));
	free(c);
	fclose(file);
}

void initialize(event* events, const char* name){
    event_tracker = events;
	//set pc to the start of where the program is loaded
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0x0000;
	//printf("got the constants\n");
	//clear display
	CLR();
	//printf("cleared display\n");
	//clear stack
	memset(stack, 0, 16*sizeof(unsigned short));
	//printf("cleared stack\n");
	//clear V registers
	memset(V, 0, 16*sizeof(unsigned char));
	//printf("cleared V\n");
	//clear memory
	memset(mem, 0, 4096);
	//printf("cleared memory\n");
	//load fontset
	for(int i = 0; i < 80; i++){
		mem[i] = chip8_fontset[i];
	}
	//printf("set fonts\n");
    
	//load game
	load(name);
    
    
	//printf("loaded game\n");

	event_tracker->delay = 0;
	event_tracker->sound = 0;
	//printf("set timers\n");
}

void emulateCycle(){
	/*printf("pc is %#06X\n", pc);
	printf("I is %#06X\n",I);
	printf("sp is %#06X\n",sp);
	for(int i = 0; i < 16; i++){
		printf("%d: %#04X, ", i, V[i]);
	}*/
	unsigned short op = fetch();
	//printf("\nop %#06X\n\n",op);

	//execute op
	decode(op);
	
	//TODO update timers
	//I don't think this is a good way to do this but I'm just going to decrement timers each cycle
	//I'm guessing this is way too often
	if(event_tracker->delay > 0)
		event_tracker->delay--;
	if(event_tracker->sound > 0)
		event_tracker->sound--;
    
}



