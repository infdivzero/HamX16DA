/*Display adapter specifications:
 * Basic information:
 *  -monochrome renderer
 *  -used to draw white characters. 1 byte is processed by the renderer at a time
 *  -a "vrom.bin" file containing raw character bitmaps is loaded into the vram
 *  -to render a character, copy the eight bytes from the character page in vram to the selected render page at the desired offset
 * Instructions (m specifies alternate mode):
 *  -set    A, B: set vram page A address B to the data sent next cycle. Two bytes are handled at a time this way
 *  -chr(m) A, B: (m0) copy the selected character in vram to page A address B in vram. The next 7 bytes "down" are copied along with the first byte at the selected page and address
 * 				  (m1) copy character at page A address B to the next available page/address offset. Increment the offset by eight
 *  -dsp:	      render the selected pixel buffer
 * ISA:
 *  -0 0000000 0000 0000 - mode instr argA argB
 */

//perhaps reduce this to an 8 bit device?
//implement a slightly more advanced instruction set, the program vrom, and other features for further low level customization? Currently vrom font shadowing is handled by emulation code and could be
//	"soft programmed" using the program vrom. Allowing this second rom to be reprogrammed could allow users to add even further functionality, perhaps primitive shaders
//if the previous idea is implemented, make the dio register accessible as an emulated register?

#include <stdio.h>
#include <SDL2/SDL.h>
#include <ini.h>

SDL_Window *win;
SDL_Renderer *ren;
SDL_Texture *display;
SDL_Event ev;

//organize these variables
unsigned int dioAddress;
unsigned int resW = 800;
unsigned int resH = 600;
unsigned int vramSize = 128000; //128kb
unsigned int regCount = 16;
unsigned short *regs;
unsigned short pgeTarget, adrTarget;
unsigned short imm;
unsigned char mode, opcode, arg1, arg2, lByte, rByte;
unsigned char *vram;

void __declspec(dllexport) init(ini_t *cfg, unsigned int dioAddr) {
	dioAddress = dioAddr;

	//Load module settings
	unsigned int tmp;
	ini_sget(cfg, "HamX16DA", "vram", "%i", &tmp);
	if(tmp) vramSize = tmp;
	ini_sget(cfg, "HamX16DA", "displayResWidth", "%i", &tmp);
	if(tmp) resW = tmp;
	ini_sget(cfg, "HamX16DA", "displayResHeight", "%i", &tmp);
	//add fullscreen setting
	if(tmp) resH = tmp;

	//Load vrom into vram <--- fix this
	FILE *data = fopen("vrom.bin", "rb");
	fseek(data, 0, SEEK_END);
	unsigned int dataSize = ftell(data);
	fseek(data, 0, SEEK_SET);
	unsigned char *vrom = calloc(dataSize, 1);
	fread(vrom, dataSize, 1, data);
	fclose(data);
	for(unsigned int i = resW * resH / 8 * 2; i < vramSize; i++) vram[i] = vrom[i - resW * resH / 8 * 2];
	free(vrom);

	//Adjust memory
	vram = calloc(vramSize, 1);
	regs = calloc(regCount, 2);

	for(unsigned int i = 0; i < vramSize; i++) vram[i] = 0;

	//Set up renderer
	SDL_Init(SDL_INIT_VIDEO);

	win = SDL_CreateWindow("HamX16", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resW, resH, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	display = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, resW, resH);
}

void __declspec(dllexport) update(unsigned int *dio, int *execute) {
	printf("%i\n", (int)vram[vramSize - 1]);
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
