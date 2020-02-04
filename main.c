/*Display adapter specifications:
 * Basic information:
 *  -used to draw pixels with no special effects
 * Instructions:
 *  -set A, B: set vram page A address B to the data sent next cycle
 *  -dsp:	   render the selected pixel buffer
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <ini.h>

SDL_Window *win;
SDL_Renderer *ren;
SDL_Texture *display;
SDL_Event ev;

unsigned int dioAddress;
unsigned int resW = 800;
unsigned int resH = 600;
unsigned int vramSize = 5767080;
unsigned int regCount = 16;
unsigned short *regs;
unsigned short pgeTarget, adrTarget;
unsigned short imm;
unsigned char mode, opcode, arg1, arg2, lByte, rByte;
unsigned char *vram;

void __declspec(dllexport) init(ini_t *cfg, unsigned int dioAddr) {
	dioAddress = dioAddr;

	unsigned int tmp;
	ini_sget(cfg, "HamX16DA", "vram", "%i", &tmp);
	if(tmp) vramSize = tmp;

	vram = calloc(vramSize, 1);
	regs = calloc(regCount, 2);

	SDL_Init(SDL_INIT_VIDEO);

	win = SDL_CreateWindow("HamX16", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	display = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, resW, resH);

	for(unsigned int i = 0; i < vramSize; i++) vram[i] = 0;
}

void __declspec(dllexport) update(unsigned int *dio, int *execute) {
	//Get events
	SDL_PollEvent(&ev);
	switch(ev.type) {
		case SDL_QUIT: {
			*execute = 0;
			break;
		}
	}

	//Get isa components
	lByte = *dio >> 8;
	rByte = *dio;
	mode = lByte >> 6;
	opcode = (lByte & 0b00111110) >> 1;
	arg1 = rByte >> 4;
	arg2 = rByte & 0b00001111;
	imm = ((lByte & 0b00000001) << 8) | rByte;

	//Process instruction
	if(*dio >> 16) {
		switch(opcode) {
			case 0x01: { //
				break;
			}
		}
	}

	*dio = 0;
}

void __declspec(dllexport) unload() {
	SDL_DestroyTexture(display);

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	free(vram);
	free(regs);
}
