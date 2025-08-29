/*
 * Copyright (c) 2021, SCNUCrypto, South China Normal University
 *
 * Apache License 2.0
 *
 */
#include "OptWBMatrix/OptWBMatrix.h"

unsigned int randseed;

// 8bit internal xor table
// The above explanation is confusing...
// In fact, this table calculates the F2-sum of bits for each 8-bit integer...
// For example, xor[3] = sum(00000011) % 2 = 0, xor[7] = sum(00000111) = 1;
static int xor[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1,
					1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0,
					1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0,
					0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1,
					1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1,
					1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1,
					0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0,
					0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
					1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0,
					0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0,
					1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1,
					1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};

// 8bit Hamming weight table
//  for what???
static int HW[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3,
				   3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4,
				   3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2,
				   2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5,
				   3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5,
				   5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3,
				   2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
				   4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
				   3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4,
				   4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6,
				   5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5,
				   5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

static uint8_t idM4[4] = {0x08, 0x04, 0x02, 0x01};
static uint8_t idM8[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static uint16_t idM16[16] = {0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400,
							 0x200, 0x100, 0x80, 0x40, 0x20, 0x10,
							 0x8, 0x4, 0x2, 0x1};
static uint32_t idM32[32] = {0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x8000000,
							 0x4000000, 0x2000000, 0x1000000, 0x800000, 0x400000,
							 0x200000, 0x100000, 0x80000, 0x40000, 0x20000,
							 0x10000, 0x8000, 0x4000, 0x2000, 0x1000,
							 0x800, 0x400, 0x200, 0x100, 0x80,
							 0x40, 0x20, 0x10, 0x8, 0x4,
							 0x2, 0x1};
static uint64_t idM64[64] = {0x8000000000000000,
							 0x4000000000000000,
							 0x2000000000000000,
							 0x1000000000000000,
							 0x800000000000000,
							 0x400000000000000,
							 0x200000000000000,
							 0x100000000000000,
							 0x80000000000000,
							 0x40000000000000,
							 0x20000000000000,
							 0x10000000000000,
							 0x8000000000000,
							 0x4000000000000,
							 0x2000000000000,
							 0x1000000000000,
							 0x800000000000,
							 0x400000000000,
							 0x200000000000,
							 0x100000000000,
							 0x80000000000,
							 0x40000000000,
							 0x20000000000,
							 0x10000000000,
							 0x8000000000,
							 0x4000000000,
							 0x2000000000,
							 0x1000000000,
							 0x800000000,
							 0x400000000,
							 0x200000000,
							 0x100000000,
							 0x80000000,
							 0x40000000,
							 0x20000000,
							 0x10000000,
							 0x8000000,
							 0x4000000,
							 0x2000000,
							 0x1000000,
							 0x800000,
							 0x400000,
							 0x200000,
							 0x100000,
							 0x80000,
							 0x40000,
							 0x20000,
							 0x10000,
							 0x8000,
							 0x4000,
							 0x2000,
							 0x1000,
							 0x800,
							 0x400,
							 0x200,
							 0x100,
							 0x80,
							 0x40,
							 0x20,
							 0x10,
							 0x8,
							 0x4,
							 0x2,
							 0x1};

void opt_SetRandSeed(unsigned int seed) { randseed = seed; }

// initial Matrix 4*4
void opt_initM4(M4 *Mat)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		(*Mat).M[i] = 0;
	}
}

// initial Matrix 8*8
void opt_initM8(M8 *Mat)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		(*Mat).M[i] = 0;
	}
}

// initial Matrix 16*16
void opt_initM16(M16 *Mat)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		(*Mat).M[i] = 0;
	}
}

// initial Matrix 32*32
void opt_initM32(M32 *Mat)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		(*Mat).M[i] = 0;
	}
}

// initial Matrix 64*64
void opt_initM64(M64 *Mat)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		(*Mat).M[i] = 0;
	}
}

// initial Matrix 128*128
void opt_initM128(M128 *Mat)
{
	int i;
	for (i = 0; i < 128; i++)
	{
		(*Mat).M[i][0] = 0;
		(*Mat).M[i][1] = 0;
	}
}

// initial Matrix 256*256
void opt_initM256(M256 *Mat)
{
	int i;
	for (i = 0; i < 256; i++)
	{
		(*Mat).M[i][0] = 0;
		(*Mat).M[i][1] = 0;
		(*Mat).M[i][2] = 0;
		(*Mat).M[i][3] = 0;
	}
}

// initial Vector 4*1
void opt_initV4(V4 *Vec) { (*Vec).V = 0; }

// initial Vector 8*1
void opt_initV8(V8 *Vec) { (*Vec).V = 0; }

// initial Vector 16*1
void opt_initV16(V16 *Vec) { (*Vec).V = 0; }

// initial Vector 32*1
void opt_initV32(V32 *Vec) { (*Vec).V = 0; }

// initial Vector 64*1
void opt_initV64(V64 *Vec) { (*Vec).V = 0; }

// initial Vector 128*1
void opt_initV128(V128 *Vec)
{
	(*Vec).V[0] = 0;
	(*Vec).V[1] = 0;
}

// initial Vector 256*1
void opt_initV256(V256 *Vec)
{
	(*Vec).V[0] = 0;
	(*Vec).V[1] = 0;
	(*Vec).V[2] = 0;
	(*Vec).V[3] = 0;
}

// randomize Matrix 4*4
void opt_randM4(M4 *Mat)
{
	int i;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 4; i++)
	{
		(*Mat).M[i] = cus_random() & 0x0f;
	}
}

// randomize Matrix 8*8
void opt_randM8(M8 *Mat)
{
	int i;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 8; i++)
	{
		(*Mat).M[i] = cus_random();
	}
}

// randomize Matrix 16*16
void opt_randM16(M16 *Mat)
{
	int i;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 16; i++)
	{
		(*Mat).M[i] = cus_random();
	}
}

// randomize Matrix 32*32
void opt_randM32(M32 *Mat)
{
	int i;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 32; i++)
	{
		(*Mat).M[i] = cus_random();
	}
}

// randomize Matrix 64*64
void opt_randM64(M64 *Mat)
{
	int i;
	uint32_t *m;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 64; i++)
	{
		m = (uint32_t *)&((*Mat).M[i]);
		*(m + 1) = cus_random();
		*m = cus_random();
	}
}

// randomize Matrix 128*128
void opt_randM128(M128 *Mat)
{
	int i;
	uint32_t *m;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 128; i++)
	{
		m = (uint32_t *)&((*Mat).M[i][0]);
		*(m + 1) = cus_random();
		*m = cus_random();
		m = (uint32_t *)&((*Mat).M[i][1]);
		*(m + 1) = cus_random();
		*m = cus_random();
	}
}

// randomize Matrix 256*256
void opt_randM256(M256 *Mat)
{
	int i;
	uint32_t *m;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	for (i = 0; i < 256; i++)
	{
		m = (uint32_t *)&((*Mat).M[i][0]);
		*(m + 1) = cus_random();
		*m = cus_random();
		m = (uint32_t *)&((*Mat).M[i][1]);
		*(m + 1) = cus_random();
		*m = cus_random();
		m = (uint32_t *)&((*Mat).M[i][2]);
		*(m + 1) = cus_random();
		*m = cus_random();
		m = (uint32_t *)&((*Mat).M[i][3]);
		*(m + 1) = cus_random();
		*m = cus_random();
	}
}

// identity matrix 4*4
void opt_identityM4(M4 *Mat)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		(*Mat).M[i] = idM4[i];
	}
}

// identity matrix 8*8
void opt_identityM8(M8 *Mat)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		(*Mat).M[i] = idM8[i];
	}
}

// identity matrix 16*16
void opt_identityM16(M16 *Mat)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		(*Mat).M[i] = idM16[i];
	}
}

// identity matrix 32*32
void opt_identityM32(M32 *Mat)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		(*Mat).M[i] = idM32[i];
	}
}

// identity matrix 64*64
void opt_identityM64(M64 *Mat)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		(*Mat).M[i] = idM64[i];
	}
}

// identity matrix 128*128
void opt_identityM128(M128 *Mat)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		(*Mat).M[i][0] = idM64[i];
		(*Mat).M[i][1] = 0;
	}
	for (i = 64; i < 128; i++)
	{
		(*Mat).M[i][0] = 0;
		(*Mat).M[i][1] = idM64[i - 64];
	}
}

// identity matrix 256*256
void opt_identityM256(M256 *Mat)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		(*Mat).M[i][0] = idM64[i];
		(*Mat).M[i][1] = 0;
		(*Mat).M[i][2] = 0;
		(*Mat).M[i][3] = 0;
	}
	for (i = 64; i < 128; i++)
	{
		(*Mat).M[i][0] = 0;
		(*Mat).M[i][1] = idM64[i - 64];
		(*Mat).M[i][2] = 0;
		(*Mat).M[i][3] = 0;
	}
	for (i = 128; i < 192; i++)
	{
		(*Mat).M[i][0] = 0;
		(*Mat).M[i][1] = 0;
		(*Mat).M[i][2] = idM64[i - 128];
		(*Mat).M[i][3] = 0;
	}
	for (i = 192; i < 256; i++)
	{
		(*Mat).M[i][0] = 0;
		(*Mat).M[i][1] = 0;
		(*Mat).M[i][2] = 0;
		(*Mat).M[i][3] = idM64[i - 192];
	}
}

// randomize Vector 4*1
void opt_randV4(V4 *Vec)
{
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	(*Vec).V = cus_random() & 0x0f;
}

// randomize Vector 8*1
void opt_randV8(V8 *Vec)
{
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	(*Vec).V = cus_random();
}

// randomize Vector 16*1
void opt_randV16(V16 *Vec)
{
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	(*Vec).V = cus_random();
}

// randomize Vector 32*1
void opt_randV32(V32 *Vec)
{
	uint16_t *v = (uint16_t *)&((*Vec).V);
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	*(v + 1) = cus_random();
	*v = cus_random();
}

// randomize Vector 64*1
void opt_randV64(V64 *Vec)
{
	uint16_t *v = (uint16_t *)&((*Vec).V);
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	*(v + 3) = cus_random();
	*(v + 2) = cus_random();
	*(v + 1) = cus_random();
	*v = cus_random();
}

// randomize Vector 128*1
void opt_randV128(V128 *Vec)
{
	uint16_t *v = (uint16_t *)&((*Vec).V[0]);
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	*(v + 3) = cus_random();
	*(v + 2) = cus_random();
	*(v + 1) = cus_random();
	*v = cus_random();
	v = (uint16_t *)&((*Vec).V[1]);
	*(v + 3) = cus_random();
	*(v + 2) = cus_random();
	*(v + 1) = cus_random();
	*v = cus_random();
}

void opt_randV256(V256 *Vec) // randomize Vector 256*1
{
	uint32_t *v = (uint32_t *)&((*Vec).V[0]);
	InitRandom((randseed++) ^ (unsigned int)time(NULL));
	*(v + 1) = cus_random();
	*v = cus_random();
	v = (uint32_t *)&((*Vec).V[1]);
	*(v + 1) = cus_random();
	*v = cus_random();
	v = (uint32_t *)&((*Vec).V[2]);
	*(v + 1) = cus_random();
	*v = cus_random();
	v = (uint32_t *)&((*Vec).V[3]);
	*(v + 1) = cus_random();
	*v = cus_random();
}

// printf Matrix 4*4
void opt_printM4(M4 Mat)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		printf("0x%x\n", Mat.M[i]);
	}
}

// printf Matrix 8*8
void opt_printM8(M8 Mat)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		printf("0x%x\n", Mat.M[i]);
	}
}

// printf Matrix 16*16
void opt_printM16(M16 Mat)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		printf("0x%x\n", Mat.M[i]);
	}
}

// printf Matrix 32*32
void opt_printM32(M32 Mat)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		printf("0x%x\n", Mat.M[i]);
	}
}

// printf Matrix 64*64
void opt_printM64(M64 Mat)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		printf("0x%llx\n", Mat.M[i]);
	}
}

// printf Matrix 128*128
void opt_printM128(M128 Mat)
{
	int i;
	for (i = 0; i < 128; i++)
	{
		printf("0x%llx ", Mat.M[i][0]);
		printf("0x%llx\n", Mat.M[i][1]);
	}
}

void opt_printM256(M256 Mat) // printf Matrix 256*256
{
	int i;
	for (i = 0; i < 256; i++)
	{
		printf("0x%llx ", Mat.M[i][0]);
		printf("0x%llx ", Mat.M[i][1]);
		printf("0x%llx ", Mat.M[i][2]);
		printf("0x%llx\n", Mat.M[i][3]);
	}
}

// printf Vector 4*1
void opt_printV4(V4 Vec) { printf("0x%x\n", Vec.V); }

// printf Vector 8*1
void opt_printV8(V8 Vec) { printf("0x%x\n", Vec.V); }

// printf Vector 16*1
void opt_printV16(V16 Vec) { printf("0x%x\n", Vec.V); }

// printf Vector 32*1
void opt_printV32(V32 Vec) { printf("0x%x\n", Vec.V); }

// printf Vector 64*1
void opt_printV64(V64 Vec) { printf("0x%llx\n", Vec.V); }

// printf Vector 128*1
void opt_printV128(V128 Vec)
{
	printf("0x%llx ", Vec.V[0]);
	printf("0x%llx\n", Vec.V[1]);
}

void opt_printV256(V256 Vec) // printf Vector 256*1
{
	printf("0x%llx ", Vec.V[0]);
	printf("0x%llx ", Vec.V[1]);
	printf("0x%llx ", Vec.V[2]);
	printf("0x%llx\n", Vec.V[3]);
}

void opt_copyM4(M4 Mat1, M4 *Mat2)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		(*Mat2).M[i] = Mat1.M[i];
	}
}

void opt_copyM8(M8 Mat1, M8 *Mat2)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		(*Mat2).M[i] = Mat1.M[i];
	}
}

void opt_copyM16(M16 Mat1, M16 *Mat2)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		(*Mat2).M[i] = Mat1.M[i];
	}
}

void opt_copyM32(M32 Mat1, M32 *Mat2)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		(*Mat2).M[i] = Mat1.M[i];
	}
}

void opt_copyM64(M64 Mat1, M64 *Mat2)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		(*Mat2).M[i] = Mat1.M[i];
	}
}

void opt_copyM128(M128 Mat1, M128 *Mat2)
{
	int i;
	for (i = 0; i < 128; i++)
	{
		(*Mat2).M[i][0] = Mat1.M[i][0];
		(*Mat2).M[i][1] = Mat1.M[i][1];
	}
}

void opt_copyM256(M256 Mat1, M256 *Mat2)
{
	int i;
	for (i = 0; i < 256; i++)
	{
		(*Mat2).M[i][0] = Mat1.M[i][0];
		(*Mat2).M[i][1] = Mat1.M[i][1];
		(*Mat2).M[i][2] = Mat1.M[i][2];
		(*Mat2).M[i][3] = Mat1.M[i][3];
	}
}

int opt_isequalM4(M4 Mat1, M4 Mat2)
{
	int i;
	int flag = 1;
	for (i = 0; i < 4; i++)
	{
		if (Mat1.M[i] != Mat2.M[i])
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

int opt_isequalM8(M8 Mat1, M8 Mat2)
{
	int i;
	int flag = 1;
	for (i = 0; i < 8; i++)
	{
		if (Mat1.M[i] != Mat2.M[i])
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

int opt_isequalM16(M16 Mat1, M16 Mat2)
{
	int i;
	int flag = 1;
	for (i = 0; i < 16; i++)
	{
		if (Mat1.M[i] != Mat2.M[i])
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

int opt_isequalM32(M32 Mat1, M32 Mat2)
{
	int i;
	int flag = 1;
	for (i = 0; i < 32; i++)
	{
		if (Mat1.M[i] != Mat2.M[i])
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

int opt_isequalM64(M64 Mat1, M64 Mat2)
{
	int i;
	int flag = 1;
	for (i = 0; i < 64; i++)
	{
		if (Mat1.M[i] != Mat2.M[i])
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

int opt_isequalM128(M128 Mat1, M128 Mat2)
{
	int i;
	int flag = 1;
	for (i = 0; i < 128; i++)
	{
		if (Mat1.M[i][0] != Mat2.M[i][0])
		{
			flag = 0;
			break;
		}
		if (Mat1.M[i][1] != Mat2.M[i][1])
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

int opt_isequalM256(M256 Mat1, M256 Mat2)
{
	int i;
	for (i = 0; i < 256; i++)
	{
		if (Mat1.M[i][0] != Mat2.M[i][0])
			return 0;
		if (Mat1.M[i][1] != Mat2.M[i][1])
			return 0;
		if (Mat1.M[i][2] != Mat2.M[i][2])
			return 0;
		if (Mat1.M[i][3] != Mat2.M[i][3])
			return 0;
	}
	return 1;
}

int opt_isequalV4(V4 Vec1, V4 Vec2)
{
	int flag = 1;
	if (Vec1.V != Vec2.V)
		flag = 0;
	return flag;
}

int opt_isequalV8(V8 Vec1, V8 Vec2)
{
	int flag = 1;
	if (Vec1.V != Vec2.V)
		flag = 0;
	return flag;
}

int opt_isequalV16(V16 Vec1, V16 Vec2)
{
	int flag = 1;
	if (Vec1.V != Vec2.V)
		flag = 0;
	return flag;
}
int opt_isequalV32(V32 Vec1, V32 Vec2)
{
	int flag = 1;
	if (Vec1.V != Vec2.V)
		flag = 0;
	return flag;
}

int opt_isequalV64(V64 Vec1, V64 Vec2)
{
	int flag = 1;
	if (Vec1.V != Vec2.V)
		flag = 0;
	return flag;
}

int opt_isequalV128(V128 Vec1, V128 Vec2)
{
	int flag = 1;
	if (Vec1.V[0] != Vec2.V[0])
		flag = 0;
	if (Vec1.V[1] != Vec2.V[1])
		flag = 0;
	return flag;
}

int opt_isequalV256(V256 Vec1, V256 Vec2)
{
	if (Vec1.V[0] != Vec2.V[0])
		return 0;
	if (Vec1.V[1] != Vec2.V[1])
		return 0;
	if (Vec1.V[2] != Vec2.V[2])
		return 0;
	if (Vec1.V[3] != Vec2.V[3])
		return 0;
	return 1;
}

// read one bit in a matrix, i in n rows, j in n columns, i,j: 0-3
int opt_readbitM4(M4 Mat, int i, int j)
{
	if ((Mat.M[i] & idM4[j]) == idM4[j])
		return 1;
	else
		return 0;
}

// read one bit in a matrix, i in n rows, j in n columns, i,j: 0-7
int opt_readbitM8(M8 Mat, int i, int j)
{
	if ((Mat.M[i] & idM8[j]) == idM8[j])
		return 1;
	else
		return 0;
}

// read one bit in a matrix, i in n rows, j in n columns, i,j: 0-15
int opt_readbitM16(M16 Mat, int i, int j)
{
	if ((Mat.M[i] & idM16[j]) == idM16[j])
		return 1;
	else
		return 0;
}

// read one bit in a matrix, i in n rows, j in n columns, i,j: 0-31
int opt_readbitM32(M32 Mat, int i, int j)
{
	if ((Mat.M[i] & idM32[j]) == idM32[j])
		return 1;
	else
		return 0;
}

// read one bit in a matrix, i in n rows, j in n columns, i,j: 0-63
int opt_readbitM64(M64 Mat, int i, int j)
{
	if ((Mat.M[i] & idM64[j]) == idM64[j])
		return 1;
	else
		return 0;
}

// read one bit in a matrix, i in n rows, j in n columns, i,j: 0-127
int opt_readbitM128(M128 Mat, int i, int j)
{
	if (j < 64)
	{
		if ((Mat.M[i][0] & idM64[j]) == idM64[j])
			return 1;
		else
			return 0;
	}
	else
	{
		if ((Mat.M[i][1] & idM64[j - 64]) == idM64[j - 64])
			return 1;
		else
			return 0;
	}
}

int opt_readbitM256(M256 Mat, int i, int j) // read one bit in a matrix, i in n rows, j in n columns, i,j: 0-255
{
	if (j < 64)
	{
		if ((Mat.M[i][0] & idM64[j]) == idM64[j])
			return 1;
		else
			return 0;
	}
	else if (j < 128)
	{
		if ((Mat.M[i][1] & idM64[j - 64]) == idM64[j - 64])
			return 1;
		else
			return 0;
	}
	else if (j < 192)
	{
		if ((Mat.M[i][2] & idM64[j - 128]) == idM64[j - 128])
			return 1;
		else
			return 0;
	}
	else
	{
		if ((Mat.M[i][3] & idM64[j - 192]) == idM64[j - 192])
			return 1;
		else
			return 0;
	}
}

// flip (i, j) bit in a matrix
void opt_flipbitM4(M4 *Mat, int i, int j) { (*Mat).M[i] ^= idM4[j]; }

// flip (i, j) bit in a matrix
void opt_flipbitM8(M8 *Mat, int i, int j) { (*Mat).M[i] ^= idM8[j]; }

// flip (i, j) bit in a matrix
void opt_flipbitM16(M16 *Mat, int i, int j) { (*Mat).M[i] ^= idM16[j]; }

// flip (i, j) bit in a matrix
void opt_flipbitM32(M32 *Mat, int i, int j) { (*Mat).M[i] ^= idM32[j]; }

// flip (i, j) bit in a matrix
void opt_flipbitM64(M64 *Mat, int i, int j) { (*Mat).M[i] ^= idM64[j]; }

// flip (i, j) bit in a matrix
void opt_flipbitM128(M128 *Mat, int i, int j)
{
	if (j < 64)
	{
		(*Mat).M[i][0] ^= idM64[j];
	}
	else
	{
		(*Mat).M[i][1] ^= idM64[j - 64];
	}
}

// flip (i, j) bit in a matrix
void opt_flipbitM256(M256 *Mat, int i, int j)
{
	if (j < 64)
	{
		(*Mat).M[i][0] ^= idM64[j];
	}
	else if (j < 128)
	{
		(*Mat).M[i][1] ^= idM64[j - 64];
	}
	else if (j < 192)
	{
		(*Mat).M[i][2] ^= idM64[j - 128];
	}
	else
	{
		(*Mat).M[i][3] ^= idM64[j - 192];
	}
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM4(M4 *Mat, int i, int j, int bit)
{
	if (opt_readbitM4(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM4(Mat, i, j);
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM8(M8 *Mat, int i, int j, int bit)
{
	if (opt_readbitM8(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM8(Mat, i, j);
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM16(M16 *Mat, int i, int j, int bit)
{
	if (opt_readbitM16(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM16(Mat, i, j);
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM32(M32 *Mat, int i, int j, int bit)
{
	if (opt_readbitM32(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM32(Mat, i, j);
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM64(M64 *Mat, int i, int j, int bit)
{
	if (opt_readbitM64(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM64(Mat, i, j);
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM128(M128 *Mat, int i, int j, int bit)
{
	if (opt_readbitM128(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM128(Mat, i, j);
}

// set (i, j) bit in a matrix, bit = 0/1
void opt_setbitM256(M256 *Mat, int i, int j, int bit)
{
	if (opt_readbitM256(*Mat, i, j) == bit)
		return;
	else
		opt_flipbitM256(Mat, i, j);
}

// Invertible Matrix?
int opt_isinvertM4(M4 Mat)
{
	int i, j, k;
	uint8_t temp;
	int flag;
	for (i = 0; i < 4; i++)
	{
		if ((Mat.M[i] & idM4[i]) == idM4[i])
		{
			for (j = i + 1; j < 4; j++)
			{
				if ((Mat.M[j] & idM4[i]) == idM4[i])
				{
					Mat.M[j] ^= Mat.M[i];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 4; j++)
			{
				if ((Mat.M[j] & idM4[i]) == idM4[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 4; k++)
			{
				if ((Mat.M[k] & idM4[i]) == idM4[i])
				{
					Mat.M[k] ^= Mat.M[i];
				}
			}
		}
	}
	if (Mat.M[3] == idM4[3])
		return 1;
	else
		return 0;
}

// Invertible Matrix?
int opt_isinvertM8(M8 Mat)
{
	int i, j, k;
	uint8_t temp;
	int flag;
	for (i = 0; i < 8; i++)
	{
		if ((Mat.M[i] & idM8[i]) == idM8[i])
		{
			for (j = i + 1; j < 8; j++)
			{
				if ((Mat.M[j] & idM8[i]) == idM8[i])
				{
					Mat.M[j] ^= Mat.M[i];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 8; j++)
			{
				if ((Mat.M[j] & idM8[i]) == idM8[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 8; k++)
			{
				if ((Mat.M[k] & idM8[i]) == idM8[i])
				{
					Mat.M[k] ^= Mat.M[i];
				}
			}
		}
	}
	if (Mat.M[7] == idM8[7])
		return 1;
	else
		return 0;
}

// Invertible Matrix?
int opt_isinvertM16(M16 Mat)
{
	int i, j, k;
	uint16_t temp;
	int flag;
	for (i = 0; i < 16; i++)
	{
		if ((Mat.M[i] & idM16[i]) == idM16[i])
		{
			for (j = i + 1; j < 16; j++)
			{
				if ((Mat.M[j] & idM16[i]) == idM16[i])
				{
					Mat.M[j] ^= Mat.M[i];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 16; j++)
			{
				if ((Mat.M[j] & idM16[i]) == idM16[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 16; k++)
			{
				if ((Mat.M[k] & idM16[i]) == idM16[i])
				{
					Mat.M[k] ^= Mat.M[i];
				}
			}
		}
	}
	if (Mat.M[15] == idM16[15])
		return 1;
	else
		return 0;
}

// Invertible Matrix?
int opt_isinvertM32(M32 Mat)
{
	int i, j, k;
	uint32_t temp;
	int flag;
	for (i = 0; i < 32; i++)
	{
		if ((Mat.M[i] & idM32[i]) == idM32[i])
		{
			for (j = i + 1; j < 32; j++)
			{
				if ((Mat.M[j] & idM32[i]) == idM32[i])
				{
					Mat.M[j] ^= Mat.M[i];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 32; j++)
			{
				if ((Mat.M[j] & idM32[i]) == idM32[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 32; k++)
			{
				if ((Mat.M[k] & idM32[i]) == idM32[i])
				{
					Mat.M[k] ^= Mat.M[i];
				}
			}
		}
	}
	if (Mat.M[31] == idM32[31])
		return 1;
	else
		return 0;
}

// Invertible Matrix?
int opt_isinvertM64(M64 Mat)
{
	int i, j, k;
	uint64_t temp;
	int flag;
	for (i = 0; i < 64; i++)
	{
		if ((Mat.M[i] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 64; j++)
			{
				if ((Mat.M[j] & idM64[i]) == idM64[i])
				{
					Mat.M[j] ^= Mat.M[i];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 64; j++)
			{
				if ((Mat.M[j] & idM64[i]) == idM64[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 64; k++)
			{
				if ((Mat.M[k] & idM64[i]) == idM64[i])
				{
					Mat.M[k] ^= Mat.M[i];
				}
			}
		}
	}
	if (Mat.M[63] == idM64[63])
		return 1;
	else
		return 0;
}

// Invertible Matrix?
int opt_isinvertM128(M128 Mat)
{
	int i, j, k;
	uint64_t temp[2];
	int flag;
	for (i = 0; i < 64; i++)
	{
		if ((Mat.M[i][0] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					Mat.M[j][0] ^= Mat.M[i][0];
					Mat.M[j][1] ^= Mat.M[i][1];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					temp[0] = Mat.M[i][0];
					Mat.M[i][0] = Mat.M[j][0];
					Mat.M[j][0] = temp[0];

					temp[1] = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp[1];
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 128; k++)
			{
				if ((Mat.M[k][0] & idM64[i]) == idM64[i])
				{
					Mat.M[k][0] ^= Mat.M[i][0];
					Mat.M[k][1] ^= Mat.M[i][1];
				}
			}
		}
	}
	for (i = 64; i < 128; i++)
	{
		if ((Mat.M[i][1] & idM64[i - 64]) == idM64[i - 64])
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[j][1] ^= Mat.M[i][1];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					temp[1] = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp[1];
					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 128; k++)
			{
				if ((Mat.M[k][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[k][1] ^= Mat.M[i][1];
				}
			}
		}
	}
	if (Mat.M[127][1] == idM64[63])
		return 1;
	else
		return 0;
}

int opt_isinvertM256(M256 Mat) // Invertible Matrix?
{
	int i, j, k;
	uint64_t temp;
	int flag;
	for (i = 0; i < 64; i++)
	{
		if ((Mat.M[i][0] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					Mat.M[j][0] ^= Mat.M[i][0];
					Mat.M[j][1] ^= Mat.M[i][1];
					Mat.M[j][2] ^= Mat.M[i][2];
					Mat.M[j][3] ^= Mat.M[i][3];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					temp = Mat.M[i][0];
					Mat.M[i][0] = Mat.M[j][0];
					Mat.M[j][0] = temp;

					temp = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp;

					temp = Mat.M[i][2];
					Mat.M[i][2] = Mat.M[j][2];
					Mat.M[j][2] = temp;

					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][0] & idM64[i]) == idM64[i])
				{
					Mat.M[k][0] ^= Mat.M[i][0];
					Mat.M[k][1] ^= Mat.M[i][1];
					Mat.M[k][2] ^= Mat.M[i][2];
					Mat.M[k][3] ^= Mat.M[i][3];
				}
			}
		}
	}
	for (i = 64; i < 128; i++)
	{
		if ((Mat.M[i][1] & idM64[i - 64]) == idM64[i - 64])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[j][1] ^= Mat.M[i][1];
					Mat.M[j][2] ^= Mat.M[i][2];
					Mat.M[j][3] ^= Mat.M[i][3];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					temp = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp;

					temp = Mat.M[i][2];
					Mat.M[i][2] = Mat.M[j][2];
					Mat.M[j][2] = temp;

					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[k][1] ^= Mat.M[i][1];
					Mat.M[k][2] ^= Mat.M[i][2];
					Mat.M[k][3] ^= Mat.M[i][3];
				}
			}
		}
	}
	for (i = 128; i < 192; i++)
	{
		if ((Mat.M[i][2] & idM64[i - 128]) == idM64[i - 128])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					Mat.M[j][2] ^= Mat.M[i][2];
					Mat.M[j][3] ^= Mat.M[i][3];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					temp = Mat.M[i][2];
					Mat.M[i][2] = Mat.M[j][2];
					Mat.M[j][2] = temp;

					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][2] & idM64[i - 128]) == idM64[i - 128])
				{
					Mat.M[k][2] ^= Mat.M[i][2];
					Mat.M[k][3] ^= Mat.M[i][3];
				}
			}
		}
	}
	for (i = 192; i < 256; i++)
	{
		if ((Mat.M[i][3] & idM64[i - 192]) == idM64[i - 192])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					Mat.M[j][3] ^= Mat.M[i][3];
				}
			}
		}
		else
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					flag = 0;
					break;
				}
			}
			if (flag)
				return 0;
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][3] & idM64[i - 192]) == idM64[i - 192])
				{
					Mat.M[k][3] ^= Mat.M[i][3];
				}
			}
		}
	}
	if (Mat.M[255][3] == idM64[63])
		return 1;
	else
		return 0;
}

// compute the 4*4 inverse matrix
void opt_invsM4(M4 Mat, M4 *Mat_inv)
{
	int i, j, k;
	uint8_t temp;
	opt_identityM4(Mat_inv);
	for (i = 0; i < 4; i++)
	{
		if ((Mat.M[i] & idM4[i]) == idM4[i])
		{
			for (j = i + 1; j < 4; j++)
			{
				if ((Mat.M[j] & idM4[i]) == idM4[i])
				{
					Mat.M[j] ^= Mat.M[i];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 4; j++)
			{
				if ((Mat.M[j] & idM4[i]) == idM4[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;
					break;
				}
			}
			for (k = i + 1; k < 4; k++)
			{
				if ((Mat.M[k] & idM4[i]) == idM4[i])
				{
					Mat.M[k] ^= Mat.M[i];
					(*Mat_inv).M[k] ^= (*Mat_inv).M[i];
				}
			}
		}
	}
	for (i = 3; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j] & idM4[i]) == idM4[i])
			{
				Mat.M[j] ^= Mat.M[i];
				(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
			}
		}
	}
}

// compute the 8*8 inverse matrix
void opt_invsM8(M8 Mat, M8 *Mat_inv)
{
	int i, j, k;
	uint8_t temp;
	opt_identityM8(Mat_inv);
	for (i = 0; i < 8; i++)
	{
		if ((Mat.M[i] & idM8[i]) == idM8[i])
		{
			for (j = i + 1; j < 8; j++)
			{
				if ((Mat.M[j] & idM8[i]) == idM8[i])
				{
					Mat.M[j] ^= Mat.M[i];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 8; j++)
			{
				if ((Mat.M[j] & idM8[i]) == idM8[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;
					break;
				}
			}
			for (k = i + 1; k < 8; k++)
			{
				if ((Mat.M[k] & idM8[i]) == idM8[i])
				{
					Mat.M[k] ^= Mat.M[i];
					(*Mat_inv).M[k] ^= (*Mat_inv).M[i];
				}
			}
		}
	}
	for (i = 7; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j] & idM8[i]) == idM8[i])
			{
				Mat.M[j] ^= Mat.M[i];
				(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
			}
		}
	}
}

// compute the 16*16 inverse matrix
void opt_invsM16(M16 Mat, M16 *Mat_inv)
{
	int i, j, k;
	uint16_t temp;
	opt_identityM16(Mat_inv);
	for (i = 0; i < 16; i++)
	{
		if ((Mat.M[i] & idM16[i]) == idM16[i])
		{
			for (j = i + 1; j < 16; j++)
			{
				if ((Mat.M[j] & idM16[i]) == idM16[i])
				{
					Mat.M[j] ^= Mat.M[i];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 16; j++)
			{
				if ((Mat.M[j] & idM16[i]) == idM16[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;
					break;
				}
			}
			for (k = i + 1; k < 16; k++)
			{
				if ((Mat.M[k] & idM16[i]) == idM16[i])
				{
					Mat.M[k] ^= Mat.M[i];
					(*Mat_inv).M[k] ^= (*Mat_inv).M[i];
				}
			}
		}
	}
	for (i = 15; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j] & idM16[i]) == idM16[i])
			{
				Mat.M[j] ^= Mat.M[i];
				(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
			}
		}
	}
}

// compute the 32*32 inverse matrix
void opt_invsM32(M32 Mat, M32 *Mat_inv)
{
	int i, j, k;
	uint32_t temp;
	opt_identityM32(Mat_inv);
	for (i = 0; i < 32; i++)
	{
		if ((Mat.M[i] & idM32[i]) == idM32[i])
		{
			for (j = i + 1; j < 32; j++)
			{
				if ((Mat.M[j] & idM32[i]) == idM32[i])
				{
					Mat.M[j] ^= Mat.M[i];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 32; j++)
			{
				if ((Mat.M[j] & idM32[i]) == idM32[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;
					break;
				}
			}
			for (k = i + 1; k < 32; k++)
			{
				if ((Mat.M[k] & idM32[i]) == idM32[i])
				{
					Mat.M[k] ^= Mat.M[i];
					(*Mat_inv).M[k] ^= (*Mat_inv).M[i];
				}
			}
		}
	}
	for (i = 31; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j] & idM32[i]) == idM32[i])
			{
				Mat.M[j] ^= Mat.M[i];
				(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
			}
		}
	}
}

// compute the 64*64 inverse matrix
void opt_invsM64(M64 Mat, M64 *Mat_inv)
{
	int i, j, k;
	uint64_t temp;
	opt_identityM64(Mat_inv);
	for (i = 0; i < 64; i++)
	{
		if ((Mat.M[i] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 64; j++)
			{
				if ((Mat.M[j] & idM64[i]) == idM64[i])
				{
					Mat.M[j] ^= Mat.M[i];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 64; j++)
			{
				if ((Mat.M[j] & idM64[i]) == idM64[i])
				{
					temp = Mat.M[i];
					Mat.M[i] = Mat.M[j];
					Mat.M[j] = temp;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;
					break;
				}
			}
			for (k = i + 1; k < 64; k++)
			{
				if ((Mat.M[k] & idM64[i]) == idM64[i])
				{
					Mat.M[k] ^= Mat.M[i];
					(*Mat_inv).M[k] ^= (*Mat_inv).M[i];
				}
			}
		}
	}
	for (i = 63; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j] & idM64[i]) == idM64[i])
			{
				Mat.M[j] ^= Mat.M[i];
				(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
			}
		}
	}
}

// compute the 128*128 inverse matrix
void opt_invsM128(M128 Mat, M128 *Mat_inv)
{
	int i, j, k;
	uint64_t temp[2];
	opt_identityM128(Mat_inv);
	for (i = 0; i < 64; i++)
	{
		if ((Mat.M[i][0] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					Mat.M[j][0] ^= Mat.M[i][0];
					Mat.M[j][1] ^= Mat.M[i][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					temp[0] = Mat.M[i][0];
					Mat.M[i][0] = Mat.M[j][0];
					Mat.M[j][0] = temp[0];

					temp[1] = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp[1];

					temp[0] = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp[0];

					temp[1] = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp[1];
					break;
				}
			}
			for (k = i + 1; k < 128; k++)
			{
				if ((Mat.M[k][0] & idM64[i]) == idM64[i])
				{
					Mat.M[k][0] ^= Mat.M[i][0];
					Mat.M[k][1] ^= Mat.M[i][1];

					(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
				}
			}
		}
	}
	for (i = 64; i < 128; i++)
	{
		if ((Mat.M[i][1] & idM64[i - 64]) == idM64[i - 64])
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[j][1] ^= Mat.M[i][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				}
			}
		}
		else
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					temp[1] = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp[1];

					temp[0] = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp[0];

					temp[1] = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp[1];
					break;
				}
			}
			for (k = i + 1; k < 128; k++)
			{
				if ((Mat.M[k][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[k][1] ^= Mat.M[i][1];

					(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
				}
			}
		}
	}
	for (i = 127; i >= 64; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
			{
				Mat.M[j][1] ^= Mat.M[i][1];
				(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
				(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
			}
		}
	}
	for (i = 63; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j][0] & idM64[i]) == idM64[i])
			{
				Mat.M[j][0] ^= Mat.M[i][0];
				(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
				(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
			}
		}
	}
}

void opt_invsM256(M256 Mat, M256 *Mat_inv) // compute the 256*256 inverse matrix
{
	int i, j, k;
	uint64_t temp;
	opt_identityM256(Mat_inv);
	for (i = 0; i < 64; i++) // diagonal = 1?
	{
		if ((Mat.M[i][0] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					Mat.M[j][0] ^= Mat.M[i][0];
					Mat.M[j][1] ^= Mat.M[i][1];
					Mat.M[j][2] ^= Mat.M[i][2];
					Mat.M[j][3] ^= Mat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		else // swap to find 1
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][0] & idM64[i]) == idM64[i])
				{
					temp = Mat.M[i][0];
					Mat.M[i][0] = Mat.M[j][0];
					Mat.M[j][0] = temp;

					temp = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp;

					temp = Mat.M[i][2];
					Mat.M[i][2] = Mat.M[j][2];
					Mat.M[j][2] = temp;

					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;
					break;
				}
			}
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][0] & idM64[i]) == idM64[i])
				{
					Mat.M[k][0] ^= Mat.M[i][0];
					Mat.M[k][1] ^= Mat.M[i][1];
					Mat.M[k][2] ^= Mat.M[i][2];
					Mat.M[k][3] ^= Mat.M[i][3];

					(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
	}
	for (i = 64; i < 128; i++) // diagonal = 1?
	{
		if ((Mat.M[i][1] & idM64[i - 64]) == idM64[i - 64])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[j][1] ^= Mat.M[i][1];
					Mat.M[j][2] ^= Mat.M[i][2];
					Mat.M[j][3] ^= Mat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		else // swap to find 1
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					temp = Mat.M[i][1];
					Mat.M[i][1] = Mat.M[j][1];
					Mat.M[j][1] = temp;

					temp = Mat.M[i][2];
					Mat.M[i][2] = Mat.M[j][2];
					Mat.M[j][2] = temp;

					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;
					break;
				}
			}
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][1] & idM64[i - 64]) == idM64[i - 64])
				{
					Mat.M[k][1] ^= Mat.M[i][1];
					Mat.M[k][2] ^= Mat.M[i][2];
					Mat.M[k][3] ^= Mat.M[i][3];

					(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
	}
	for (i = 128; i < 192; i++) // diagonal = 1?
	{
		if ((Mat.M[i][2] & idM64[i - 128]) == idM64[i - 128])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					Mat.M[j][2] ^= Mat.M[i][2];
					Mat.M[j][3] ^= Mat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		else // swap to find 1
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					temp = Mat.M[i][2];
					Mat.M[i][2] = Mat.M[j][2];
					Mat.M[j][2] = temp;

					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;
					break;
				}
			}
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][2] & idM64[i - 128]) == idM64[i - 128])
				{
					Mat.M[k][2] ^= Mat.M[i][2];
					Mat.M[k][3] ^= Mat.M[i][3];

					(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
	}
	for (i = 192; i < 256; i++) // diagonal = 1?
	{
		if ((Mat.M[i][3] & idM64[i - 192]) == idM64[i - 192])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					Mat.M[j][3] ^= Mat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		else // swap to find 1
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((Mat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					temp = Mat.M[i][3];
					Mat.M[i][3] = Mat.M[j][3];
					Mat.M[j][3] = temp;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;
					break;
				}
			}
			for (k = i + 1; k < 256; k++)
			{
				if ((Mat.M[k][3] & idM64[i - 192]) == idM64[i - 192])
				{
					Mat.M[k][3] ^= Mat.M[i][3];

					(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
	}
	for (i = 255; i >= 192; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
			{
				Mat.M[j][3] ^= Mat.M[i][3];

				(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
				(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
				(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
			}
		}
	}
	for (i = 191; i >= 128; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
			{
				Mat.M[j][2] ^= Mat.M[i][2];

				(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
				(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
				(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
			}
		}
	}
	for (i = 127; i >= 64; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
			{
				Mat.M[j][1] ^= Mat.M[i][1];

				(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
				(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
				(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
			}
		}
	}
	for (i = 63; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if ((Mat.M[j][0] & idM64[i]) == idM64[i])
			{
				Mat.M[j][0] ^= Mat.M[i][0];

				(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
				(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
				(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
			}
		}
	}
}

// 4bits affine transformation
uint8_t opt_affineU4(Aff4 aff, uint8_t arr)
{
	V4 mul_vec, ans_vec;
	mul_vec.V = arr;
	opt_MatMulVecM4(aff.Mat, mul_vec, &ans_vec); // mul
	return ans_vec.V ^ aff.Vec.V;				 // add
}

// 8bits affine transformation
uint8_t opt_affineU8(Aff8 aff, uint8_t arr)
{
	V8 mul_vec, ans_vec;
	mul_vec.V = arr;
	opt_MatMulVecM8(aff.Mat, mul_vec, &ans_vec); // mul
	return ans_vec.V ^ aff.Vec.V;				 // add
}

// 16bits affine transformation
uint16_t opt_affineU16(Aff16 aff, uint16_t arr)
{
	V16 mul_vec, ans_vec;
	mul_vec.V = arr;
	opt_MatMulVecM16(aff.Mat, mul_vec, &ans_vec); // mul
	return ans_vec.V ^ aff.Vec.V;				  // add
}

// 32bits affine transformation
uint32_t opt_affineU32(Aff32 aff, uint32_t arr)
{
	V32 mul_vec, ans_vec;
	mul_vec.V = arr;
	opt_MatMulVecM32(aff.Mat, mul_vec, &ans_vec); // mul
	return ans_vec.V ^ aff.Vec.V;				  // add
}

// 64bits affine transformation
uint64_t opt_affineU64(Aff64 aff, uint64_t arr)
{
	V64 mul_vec, ans_vec;
	mul_vec.V = arr;
	opt_MatMulVecM64(aff.Mat, mul_vec, &ans_vec); // mul
	return ans_vec.V ^ aff.Vec.V;				  // add
}

// 128bits affine transformation
void opt_affineU128(Aff128 aff, uint64_t arr[], uint64_t ans[])
{
	V128 mul_vec, ans_vec;
	mul_vec.V[0] = arr[0];
	mul_vec.V[1] = arr[1];
	opt_MatMulVecM128(aff.Mat, mul_vec, &ans_vec); // mul
	ans[0] = ans_vec.V[0] ^ aff.Vec.V[0];		   // add
	ans[1] = ans_vec.V[1] ^ aff.Vec.V[1];
}

// Original
// 4bits internal xor
int opt_parityU4(uint8_t n)
{
	if (xor[n])
		return 1;
	else
		return 0;
}

// Original
// uint8_t internal xor
int opt_parityU8(uint8_t n)
{
	if (xor[n])
		return 1;
	else
		return 0;
}

// Original
// uint16_t internal xor
int opt_parityU16(uint16_t n)
{
	uint8_t temp = 0;
	uint8_t *u = (uint8_t *)&n;
	temp = (*u) ^ (*(u + 1));
	if (opt_parityU8(temp))
		return 1;
	else
		return 0;
}

// // Original
// // uint32_t internal xor
// int xorU32(uint32_t n)
// {
// 	uint16_t temp = 0;
// 	uint16_t *u = (uint16_t *)&n;
// 	temp = (*u) ^ (*(u + 1));
// 	if (xorU16(temp))
// 		return 1;
// 	else
// 		return 0;
// }

// Optimized version
// Perform XOR operations to calculate the sum of bits modulo 2
int opt_parityU32(uint32_t n)
{
	n ^= n >> 16;
	n ^= n >> 8;
	n ^= n >> 4;
	n ^= n >> 2;
	n ^= n >> 1;
	// Return the least significant bit, which represents the F2-sum
	return n & 1;
}

// // Original
// // uint64_t internal xor
// int xorU64(uint64_t n)
// {
// 	uint32_t temp = 0;
// 	uint32_t *u = (uint32_t *)&n;
// 	temp = (*u) ^ (*(u + 1));
// 	if (xorU32(temp))
// 		return 1;
// 	else
// 		return 0;
// }

// Optimized version
// Calculate the parity (sum of bits modulo 2) for a 64-bit integer
int opt_parityU64(uint64_t n)
{
	// Use parallel bit counting (Hamming weight) technique
	n ^= n >> 32;
	n ^= n >> 16;
	n ^= n >> 8;
	n ^= n >> 4;
	n &= 0x0F0F0F0F0F0F0F0F; // Clear high bits in each byte
	return (0x6996 >> ((n * 0x0101010101010101) & 0xFF)) & 1;
}

// // Original
// // uint128_t internal xor
// int xorU128(uint64_t n[])
// {
// 	uint64_t temp = 0;
// 	temp = n[0] ^ n[1];
// 	if (xorU64(temp))
// 		return 1;
// 	else
// 		return 0;
// }

// Optimized version
// Calculate the parity (sum of bits modulo 2) for a 128-bit integer
int opt_parityU128(uint64_t n[])
{
	// Combine the two 64-bit integers and calculate the parity
	uint64_t combined = n[0] ^ n[1];
	return opt_parityU64(combined);
}

// // Original
// int xorU256(uint64_t n[]) // uint256_t internal xor
// {
//     uint64_t temp = 0;
//     temp = n[0] ^ n[1] ^ n[2] ^ n[3];
//     if (xorU64(temp))
//         return 1;
//     else
//         return 0;
// }

// Optimized version
// Calculate the parity (sum of bits modulo 2) for a 256-bit integer
int opt_parityU256(uint64_t n[])
{
	// Combine the four 64-bit integers and calculate the parity
	uint64_t combined = n[0] ^ n[1] ^ n[2] ^ n[3];
	return opt_parityU64(combined);
}

// 4bits HW
int opt_HWU4(uint8_t n) { return HW[n]; }

// uint8_t HW
int opt_HWU8(uint8_t n) { return HW[n]; }

// uint16_t HW
int opt_HWU16(uint16_t n)
{
	uint8_t *u = (uint8_t *)&n;
	return opt_HWU8(*u) + opt_HWU8(*(u + 1));
}

// uint32_t HW
int opt_HWU32(uint32_t n)
{
	uint16_t *u = (uint16_t *)&n;
	return opt_HWU16(*u) + opt_HWU16(*(u + 1));
}

// uint64_t HW
int opt_HWU64(uint64_t n)
{
	uint32_t *u = (uint32_t *)&n;
	return opt_HWU32(*u) + opt_HWU32(*(u + 1));
}

// uint128_t HW
int opt_HWU128(uint64_t n[]) { return opt_HWU64(n[0]) + opt_HWU64(n[1]); }

// printf uint8_t
void opt_printU8(uint8_t n) { printf("0x%x\n", n); }

// printf uint16_t
void opt_printU16(uint16_t n) { printf("0x%x\n", n); }

// printf uint32_t
void opt_printU32(uint32_t n) { printf("0x%x\n", n); }

// printf uint64_t
void opt_printU64(uint64_t n) { printf("0x%llx\n", n); }

// printf uint128_t
void opt_printU128(uint64_t n[])
{
	printf("0x%llx ", n[0]);
	printf("0x%llx\n", n[1]);
}

// printf Matrix 4*4 in the form of bits
void opt_printbitM4(M4 Mat)
{
	int i, j;
	uint8_t temp;
	for (i = 0; i < 4; i++)
	{
		temp = Mat.M[i];
		for (j = 0; j < 4; j++)
		{
			if (temp & 0x08)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		printf("\n");
	}
	printf("\n");
}

// printf Matrix 8*8 in the form of bits
void opt_printbitM8(M8 Mat)
{
	int i, j;
	uint8_t temp;
	for (i = 0; i < 8; i++)
	{
		temp = Mat.M[i];
		for (j = 0; j < 8; j++)
		{
			if (temp & 0x80)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		printf("\n");
	}
	printf("\n");
}

// printf Matrix 16*16 in the form of bits
void opt_printbitM16(M16 Mat)
{
	int i, j;
	uint16_t temp;
	for (i = 0; i < 16; i++)
	{
		temp = Mat.M[i];
		for (j = 0; j < 16; j++)
		{
			if (temp & 0x8000)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		printf("\n");
	}
	printf("\n");
}

// printf Matrix 32*32 in the form of bits
void opt_printbitM32(M32 Mat)
{
	int i, j;
	uint32_t temp;
	for (i = 0; i < 32; i++)
	{
		temp = Mat.M[i];
		for (j = 0; j < 32; j++)
		{
			if (temp & 0x80000000)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		printf("\n");
	}
	printf("\n");
}

// printf Matrix 64*64 in the form of bits
void opt_printbitM64(M64 Mat)
{
	int i, j;
	uint64_t temp;
	for (i = 0; i < 64; i++)
	{
		temp = Mat.M[i];
		for (j = 0; j < 64; j++)
		{
			if (temp & 0x8000000000000000)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		printf("\n");
	}
	printf("\n");
}

// printf Matrix 128*128 in the form of bits
void opt_printbitM128(M128 Mat)
{
	int i, j;
	uint64_t temp;
	for (i = 0; i < 128; i++)
	{
		temp = Mat.M[i][0];
		for (j = 0; j < 64; j++)
		{
			if (temp & 0x8000000000000000)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		temp = Mat.M[i][1];
		for (j = 0; j < 64; j++)
		{
			if (temp & 0x8000000000000000)
				printf("%d ", 1);
			else
				printf("%d ", 0);
			temp = temp << 1;
		}
		printf("\n");
	}
	printf("\n");
}

void opt_VecAddVecV4(V4 Vec1, V4 Vec2, V4 *Vec) { (*Vec).V = Vec1.V ^ Vec2.V; }

void opt_VecAddVecV8(V8 Vec1, V8 Vec2, V8 *Vec) { (*Vec).V = Vec1.V ^ Vec2.V; }

void opt_VecAddVecV16(V16 Vec1, V16 Vec2, V16 *Vec) { (*Vec).V = Vec1.V ^ Vec2.V; }

void opt_VecAddVecV32(V32 Vec1, V32 Vec2, V32 *Vec) { (*Vec).V = Vec1.V ^ Vec2.V; }

void opt_VecAddVecV64(V64 Vec1, V64 Vec2, V64 *Vec) { (*Vec).V = Vec1.V ^ Vec2.V; }

void opt_VecAddVecV128(V128 Vec1, V128 Vec2, V128 *Vec)
{
	(*Vec).V[0] = Vec1.V[0] ^ Vec2.V[0];
	(*Vec).V[1] = Vec1.V[1] ^ Vec2.V[1];
}

void opt_VecAddVecV256(V256 Vec1, V256 Vec2, V256 *Vec)
{
	(*Vec).V[0] = Vec1.V[0] ^ Vec2.V[0];
	(*Vec).V[1] = Vec1.V[1] ^ Vec2.V[1];
	(*Vec).V[2] = Vec1.V[2] ^ Vec2.V[2];
	(*Vec).V[3] = Vec1.V[3] ^ Vec2.V[3];
}

// matrix * number -> number 4bits
uint8_t opt_MatMulNumM4(M4 Mat, uint8_t n)
{
	int i;
	uint8_t temp = 0;
	for (i = 0; i < 4; i++)
	{
		if (opt_parityU4(Mat.M[i] & n & 0x0f))
			temp ^= idM4[i];
	}
	return temp;
}

// matrix * number -> number 8bits
uint8_t opt_MatMulNumM8(M8 Mat, uint8_t n)
{
	int i;
	uint8_t temp = 0;
	for (i = 0; i < 8; i++)
	{
		if (opt_parityU8(Mat.M[i] & n))
			temp ^= idM8[i];
	}
	return temp;
}

// matrix * number -> number 16bits
uint16_t opt_MatMulNumM16(M16 Mat, uint16_t n)
{
	int i;
	uint16_t temp = 0;
	for (i = 0; i < 16; i++)
	{
		if (opt_parityU16(Mat.M[i] & n))
			temp ^= idM16[i];
	}
	return temp;
}

// matrix * number -> number 32bits
uint32_t opt_MatMulNumM32(M32 Mat, uint32_t n)
{
	int i;
	uint32_t temp = 0;
	for (i = 0; i < 32; i++)
	{
		if (opt_parityU32(Mat.M[i] & n))
			temp ^= idM32[i];
	}
	return temp;
}

// matrix * number -> number 64bits
uint64_t opt_MatMulNumM64(M64 Mat, uint64_t n)
{
	int i;
	uint64_t temp = 0;
	for (i = 0; i < 64; i++)
	{
		if (opt_parityU64(Mat.M[i] & n))
			temp ^= idM64[i];
	}
	return temp;
}

// matrix * vector -> vector 4*1
void opt_MatMulVecM4(M4 Mat, V4 Vec, V4 *ans)
{
	int i;
	opt_initV4(ans);
	for (i = 0; i < 4; i++)
	{
		if (opt_parityU4(Mat.M[i] & Vec.V & 0x0f))
			(*ans).V ^= idM4[i];
	}
}

// matrix * vector -> vector 8*1
void opt_MatMulVecM8(M8 Mat, V8 Vec, V8 *ans)
{
	int i;
	opt_initV8(ans);
	for (i = 0; i < 8; i++)
	{
		if (opt_parityU8(Mat.M[i] & Vec.V))
			(*ans).V ^= idM8[i];
	}
}

// matrix * vector -> vector 16*1
void opt_MatMulVecM16(M16 Mat, V16 Vec, V16 *ans)
{
	int i;
	opt_initV16(ans);
	for (i = 0; i < 16; i++)
	{
		if (opt_parityU16(Mat.M[i] & Vec.V))
			(*ans).V ^= idM16[i];
	}
}

// matrix * vector -> vector 32*1
void opt_MatMulVecM32(M32 Mat, V32 Vec, V32 *ans)
{
	int i;
	opt_initV32(ans);
	for (i = 0; i < 32; i++)
	{
		if (opt_parityU32(Mat.M[i] & Vec.V))
			(*ans).V ^= idM32[i];
	}
}

// matrix * vector -> vector 64*1
void opt_MatMulVecM64(M64 Mat, V64 Vec, V64 *ans)
{
	int i;
	opt_initV64(ans);
	for (i = 0; i < 64; i++)
	{
		if (opt_parityU64(Mat.M[i] & Vec.V))
			(*ans).V ^= idM64[i];
	}
}

// // Original
// // matrix * vector -> vector 128*1
// void MatMulVecM128(M128 Mat, V128 Vec, V128 *ans)
// {
// 	int i;
// 	initV128(ans);
// 	uint64_t temp[2];
// 	for (i = 0; i < 64; i++)
// 	{
// 		temp[0] = Mat.M[i][0] & Vec.V[0];
// 		temp[1] = Mat.M[i][1] & Vec.V[1];
// 		if (xorU128(temp))
// 			(*ans).V[0] ^= idM64[i];
// 	}
// 	for (i = 64; i < 128; i++)
// 	{
// 		temp[0] = Mat.M[i][0] & Vec.V[0];
// 		temp[1] = Mat.M[i][1] & Vec.V[1];
// 		if (xorU128(temp))
// 			(*ans).V[1] ^= idM64[i - 64];
// 	}
// }

// // Optimized versions
// void opt_MatMulVecM128(M128 Mat, V128 Vec, V128 *ans)
// {
// 	opt_initV128(ans); // zero ans
// 	const uint64_t vec0 = Vec.V[0];
// 	const uint64_t vec1 = Vec.V[1];

// 	// Process 4 rows per iteration
// 	for (int i = 0; i < 128; i += 4)
// 	{
// 		// Compute masked XOR for 4 consecutive rows
// 		uint64_t x0 = (Mat.M[i + 0][0] & vec0) ^ (Mat.M[i + 0][1] & vec1);
// 		uint64_t x1 = (Mat.M[i + 1][0] & vec0) ^ (Mat.M[i + 1][1] & vec1);
// 		uint64_t x2 = (Mat.M[i + 2][0] & vec0) ^ (Mat.M[i + 2][1] & vec1);
// 		uint64_t x3 = (Mat.M[i + 3][0] & vec0) ^ (Mat.M[i + 3][1] & vec1);

// 		// Compute parity
// 		int p0 = opt_parityU64(x0);
// 		int p1 = opt_parityU64(x1);
// 		int p2 = opt_parityU64(x2);
// 		int p3 = opt_parityU64(x3);

// 		// Convert parity to mask (0 or 0xFFFFFFFFFFFFFFFF)
// 		uint64_t m0 = -(uint64_t)p0;
// 		uint64_t m1 = -(uint64_t)p1;
// 		uint64_t m2 = -(uint64_t)p2;
// 		uint64_t m3 = -(uint64_t)p3;

// 		// Branchless XOR into ans
// 		ans->V[(i + 0) / 64] ^= idM64[(i + 0) % 64] & m0;
// 		ans->V[(i + 1) / 64] ^= idM64[(i + 1) % 64] & m1;
// 		ans->V[(i + 2) / 64] ^= idM64[(i + 2) % 64] & m2;
// 		ans->V[(i + 3) / 64] ^= idM64[(i + 3) % 64] & m3;
// 	}
// }

void opt_MatMulVecM128(M128 Mat, V128 Vec, V128 *ans)
{
	opt_initV128(ans);
	const uint64_t vec0 = Vec.V[0];
	const uint64_t vec1 = Vec.V[1];

	// Process 8 rows per iteration
	for (int i = 0; i < 128; i += 8)
	{
		// Compute masked XOR for 8 consecutive rows
		uint64_t x0 = (Mat.M[i + 0][0] & vec0) ^ (Mat.M[i + 0][1] & vec1);
		uint64_t x1 = (Mat.M[i + 1][0] & vec0) ^ (Mat.M[i + 1][1] & vec1);
		uint64_t x2 = (Mat.M[i + 2][0] & vec0) ^ (Mat.M[i + 2][1] & vec1);
		uint64_t x3 = (Mat.M[i + 3][0] & vec0) ^ (Mat.M[i + 3][1] & vec1);
		uint64_t x4 = (Mat.M[i + 4][0] & vec0) ^ (Mat.M[i + 4][1] & vec1);
		uint64_t x5 = (Mat.M[i + 5][0] & vec0) ^ (Mat.M[i + 5][1] & vec1);
		uint64_t x6 = (Mat.M[i + 6][0] & vec0) ^ (Mat.M[i + 6][1] & vec1);
		uint64_t x7 = (Mat.M[i + 7][0] & vec0) ^ (Mat.M[i + 7][1] & vec1);

		// Calculate parities and apply masks
		ans->V[(i + 0) / 64] ^= idM64[(i + 0) % 64] & (-(uint64_t)opt_parityU64(x0));
		ans->V[(i + 1) / 64] ^= idM64[(i + 1) % 64] & (-(uint64_t)opt_parityU64(x1));
		ans->V[(i + 2) / 64] ^= idM64[(i + 2) % 64] & (-(uint64_t)opt_parityU64(x2));
		ans->V[(i + 3) / 64] ^= idM64[(i + 3) % 64] & (-(uint64_t)opt_parityU64(x3));
		ans->V[(i + 4) / 64] ^= idM64[(i + 4) % 64] & (-(uint64_t)opt_parityU64(x4));
		ans->V[(i + 5) / 64] ^= idM64[(i + 5) % 64] & (-(uint64_t)opt_parityU64(x5));
		ans->V[(i + 6) / 64] ^= idM64[(i + 6) % 64] & (-(uint64_t)opt_parityU64(x6));
		ans->V[(i + 7) / 64] ^= idM64[(i + 7) % 64] & (-(uint64_t)opt_parityU64(x7));
	}
}

// Original
void opt_MatMulVecM256(M256 Mat, V256 Vec, V256 *ans) // matrix * vector -> vector 256*1
{
	int i;
	opt_initV256(ans);
	uint64_t temp[4];
	for (i = 0; i < 64; i++)
	{
		temp[0] = Mat.M[i][0] & Vec.V[0];
		temp[1] = Mat.M[i][1] & Vec.V[1];
		temp[2] = Mat.M[i][2] & Vec.V[2];
		temp[3] = Mat.M[i][3] & Vec.V[3];
		if (opt_parityU256(temp))
			(*ans).V[0] ^= idM64[i];
	}
	for (i = 64; i < 128; i++)
	{
		temp[0] = Mat.M[i][0] & Vec.V[0];
		temp[1] = Mat.M[i][1] & Vec.V[1];
		temp[2] = Mat.M[i][2] & Vec.V[2];
		temp[3] = Mat.M[i][3] & Vec.V[3];
		if (opt_parityU256(temp))
			(*ans).V[1] ^= idM64[i - 64];
	}
	for (i = 128; i < 192; i++)
	{
		temp[0] = Mat.M[i][0] & Vec.V[0];
		temp[1] = Mat.M[i][1] & Vec.V[1];
		temp[2] = Mat.M[i][2] & Vec.V[2];
		temp[3] = Mat.M[i][3] & Vec.V[3];
		if (opt_parityU256(temp))
			(*ans).V[2] ^= idM64[i - 128];
	}
	for (i = 192; i < 256; i++)
	{
		temp[0] = Mat.M[i][0] & Vec.V[0];
		temp[1] = Mat.M[i][1] & Vec.V[1];
		temp[2] = Mat.M[i][2] & Vec.V[2];
		temp[3] = Mat.M[i][3] & Vec.V[3];
		if (opt_parityU256(temp))
			(*ans).V[3] ^= idM64[i - 192];
	}
}

// generate 4*4 invertible matrix and its inverse matrix
void opt_genMatpairM4(M4 *Mat, M4 *Mat_inv)
{
	int i, j, t, k;
	int p;
	M4 tempMat;
	M4 resultMat;
	uint8_t temp;
	uint8_t trail[16][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM4(Mat);
	opt_identityM4(Mat_inv);
	opt_randM4(&tempMat);
	opt_copyM4(tempMat, &resultMat);
	for (i = 0; i < 4; i++) // diagonal = 1?
	{
		if ((tempMat.M[i] & idM4[i]) == idM4[i])
		{
			for (j = i + 1; j < 4; j++)
			{
				if ((tempMat.M[j] & idM4[i]) == idM4[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 4; j++)
			{
				if ((tempMat.M[j] & idM4[i]) == idM4[i])
				{
					temp = tempMat.M[i];
					tempMat.M[i] = tempMat.M[j];
					tempMat.M[j] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 3)
				{
					p = i + 1 + cus_random() % (3 - i); // swap
					temp = tempMat.M[p];
					tempMat.M[p] = tempMat.M[i];
					tempMat.M[i] = temp;
					temp = (*Mat_inv).M[p];
					(*Mat_inv).M[p] = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = temp;
					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;
					for (t = i + 1; t < 4; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t] ^= tempMat.M[i];
							(*Mat_inv).M[t] ^= (*Mat_inv).M[i];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 4; k++)
				{
					if ((tempMat.M[k] & idM4[i]) == idM4[i])
					{
						tempMat.M[k] ^= tempMat.M[i];

						(*Mat_inv).M[k] ^= (*Mat_inv).M[i];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 3; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM4[t]) == idM4[t])
				{
					tempMat.M[j] ^= tempMat.M[t];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[t];
					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]] ^= (*Mat).M[trail[j][2]];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]];
				(*Mat).M[trail[j][1]] = (*Mat).M[trail[j][2]];
				(*Mat).M[trail[j][2]] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 3; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM4[i]) == idM4[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		opt_copyM4(resultMat, Mat);
	}
}

// generate 8*8 invertible matrix and its inverse matrix
void opt_genMatpairM8(M8 *Mat, M8 *Mat_inv)
{
	int i, j, t, k;
	int p;
	M8 tempMat;
	M8 resultMat;
	uint8_t temp;
	uint8_t trail[64][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM8(Mat);
	opt_identityM8(Mat_inv);
	opt_randM8(&tempMat);
	opt_copyM8(tempMat, &resultMat);
	for (i = 0; i < 8; i++) // diagonal = 1?
	{
		if ((tempMat.M[i] & idM8[i]) == idM8[i])
		{
			for (j = i + 1; j < 8; j++)
			{
				if ((tempMat.M[j] & idM8[i]) == idM8[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 8; j++)
			{
				if ((tempMat.M[j] & idM8[i]) == idM8[i])
				{
					temp = tempMat.M[i];
					tempMat.M[i] = tempMat.M[j];
					tempMat.M[j] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 7)
				{
					p = i + 1 + cus_random() % (7 - i); // swap
					temp = tempMat.M[p];
					tempMat.M[p] = tempMat.M[i];
					tempMat.M[i] = temp;
					temp = (*Mat_inv).M[p];
					(*Mat_inv).M[p] = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = temp;
					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;
					for (t = i + 1; t < 8; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t] ^= tempMat.M[i];
							(*Mat_inv).M[t] ^= (*Mat_inv).M[i];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 8; k++)
				{
					if ((tempMat.M[k] & idM8[i]) == idM8[i])
					{
						tempMat.M[k] ^= tempMat.M[i];

						(*Mat_inv).M[k] ^= (*Mat_inv).M[i];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 7; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM8[t]) == idM8[t])
				{
					tempMat.M[j] ^= tempMat.M[t];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[t];
					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]] ^= (*Mat).M[trail[j][2]];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]];
				(*Mat).M[trail[j][1]] = (*Mat).M[trail[j][2]];
				(*Mat).M[trail[j][2]] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 7; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM8[i]) == idM8[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		opt_copyM8(resultMat, Mat);
	}
}

// generate 16*16 invertible matrix and its inverse matrix
void opt_genMatpairM16(M16 *Mat, M16 *Mat_inv)
{
	int i, j, t, k;
	int p;
	M16 tempMat;
	M16 resultMat;
	uint16_t temp;
	uint8_t trail[256][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM16(Mat);
	opt_identityM16(Mat_inv);
	opt_randM16(&tempMat);
	opt_copyM16(tempMat, &resultMat);
	for (i = 0; i < 16; i++) // diagonal = 1?
	{
		if ((tempMat.M[i] & idM16[i]) == idM16[i])
		{
			for (j = i + 1; j < 16; j++)
			{
				if ((tempMat.M[j] & idM16[i]) == idM16[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 16; j++)
			{
				if ((tempMat.M[j] & idM16[i]) == idM16[i])
				{
					temp = tempMat.M[i];
					tempMat.M[i] = tempMat.M[j];
					tempMat.M[j] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 15)
				{
					p = i + 1 + cus_random() % (15 - i); // swap
					temp = tempMat.M[p];
					tempMat.M[p] = tempMat.M[i];
					tempMat.M[i] = temp;
					temp = (*Mat_inv).M[p];
					(*Mat_inv).M[p] = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = temp;
					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;
					for (t = i + 1; t < 16; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t] ^= tempMat.M[i];
							(*Mat_inv).M[t] ^= (*Mat_inv).M[i];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 16; k++)
				{
					if ((tempMat.M[k] & idM16[i]) == idM16[i])
					{
						tempMat.M[k] ^= tempMat.M[i];

						(*Mat_inv).M[k] ^= (*Mat_inv).M[i];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 15; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM16[t]) == idM16[t])
				{
					tempMat.M[j] ^= tempMat.M[t];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[t];
					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]] ^= (*Mat).M[trail[j][2]];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]];
				(*Mat).M[trail[j][1]] = (*Mat).M[trail[j][2]];
				(*Mat).M[trail[j][2]] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 15; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM16[i]) == idM16[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		opt_copyM16(resultMat, Mat);
	}
}

// generate 32*32 invertible matrix and its inverse matrix
void opt_genMatpairM32(M32 *Mat, M32 *Mat_inv)
{
	int i, j, t, k;
	int p;
	M32 tempMat;
	M32 resultMat;
	uint32_t temp;
	uint8_t trail[1024][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM32(Mat);
	opt_identityM32(Mat_inv);
	opt_randM32(&tempMat);
	opt_copyM32(tempMat, &resultMat);
	for (i = 0; i < 32; i++) // diagonal = 1?
	{
		if ((tempMat.M[i] & idM32[i]) == idM32[i])
		{
			for (j = i + 1; j < 32; j++)
			{
				if ((tempMat.M[j] & idM32[i]) == idM32[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 32; j++)
			{
				if ((tempMat.M[j] & idM32[i]) == idM32[i])
				{
					temp = tempMat.M[i];
					tempMat.M[i] = tempMat.M[j];
					tempMat.M[j] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 31)
				{
					p = i + 1 + cus_random() % (31 - i); // swap
					temp = tempMat.M[p];
					tempMat.M[p] = tempMat.M[i];
					tempMat.M[i] = temp;
					temp = (*Mat_inv).M[p];
					(*Mat_inv).M[p] = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = temp;
					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;
					for (t = i + 1; t < 32; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t] ^= tempMat.M[i];
							(*Mat_inv).M[t] ^= (*Mat_inv).M[i];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 32; k++)
				{
					if ((tempMat.M[k] & idM32[i]) == idM32[i])
					{
						tempMat.M[k] ^= tempMat.M[i];

						(*Mat_inv).M[k] ^= (*Mat_inv).M[i];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 31; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM32[t]) == idM32[t])
				{
					tempMat.M[j] ^= tempMat.M[t];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[t];
					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]] ^= (*Mat).M[trail[j][2]];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]];
				(*Mat).M[trail[j][1]] = (*Mat).M[trail[j][2]];
				(*Mat).M[trail[j][2]] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 31; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM32[i]) == idM32[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		opt_copyM32(resultMat, Mat);
	}
}

// generate 64*64 invertible matrix and its inverse matrix
void opt_genMatpairM64(M64 *Mat, M64 *Mat_inv)
{
	int i, j, t, k;
	int p;
	M64 tempMat;
	M64 resultMat;
	uint64_t temp;
	uint8_t trail[4096][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM64(Mat);
	opt_identityM64(Mat_inv);
	opt_randM64(&tempMat);
	opt_copyM64(tempMat, &resultMat);
	for (i = 0; i < 64; i++) // diagonal = 1?
	{
		if ((tempMat.M[i] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 64; j++)
			{
				if ((tempMat.M[j] & idM64[i]) == idM64[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 64; j++)
			{
				if ((tempMat.M[j] & idM64[i]) == idM64[i])
				{
					temp = tempMat.M[i];
					tempMat.M[i] = tempMat.M[j];
					tempMat.M[j] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = (*Mat_inv).M[j];
					(*Mat_inv).M[j] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 63)
				{
					p = i + 1 + cus_random() % (63 - i); // swap
					temp = tempMat.M[p];
					tempMat.M[p] = tempMat.M[i];
					tempMat.M[i] = temp;
					temp = (*Mat_inv).M[p];
					(*Mat_inv).M[p] = (*Mat_inv).M[i];
					(*Mat_inv).M[i] = temp;
					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;
					for (t = i + 1; t < 64; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t] ^= tempMat.M[i];
							(*Mat_inv).M[t] ^= (*Mat_inv).M[i];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 64; k++)
				{
					if ((tempMat.M[k] & idM64[i]) == idM64[i])
					{
						tempMat.M[k] ^= tempMat.M[i];

						(*Mat_inv).M[k] ^= (*Mat_inv).M[i];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 63; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM64[t]) == idM64[t])
				{
					tempMat.M[j] ^= tempMat.M[t];
					(*Mat_inv).M[j] ^= (*Mat_inv).M[t];
					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]] ^= (*Mat).M[trail[j][2]];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]];
				(*Mat).M[trail[j][1]] = (*Mat).M[trail[j][2]];
				(*Mat).M[trail[j][2]] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 63; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j] & idM64[i]) == idM64[i])
				{
					tempMat.M[j] ^= tempMat.M[i];

					(*Mat_inv).M[j] ^= (*Mat_inv).M[i];
				}
			}
		}
		opt_copyM64(resultMat, Mat);
	}
}

// generate 128*128 invertible matrix and its inverse matrix
void opt_genMatpairM128(M128 *Mat, M128 *Mat_inv)
{
	int i, j, t, k;
	int p;
	M128 tempMat;
	M128 resultMat;
	uint64_t temp;
	uint8_t trail[16384][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM128(Mat);
	opt_identityM128(Mat_inv);
	opt_randM128(&tempMat);
	opt_copyM128(tempMat, &resultMat);
	for (i = 0; i < 64; i++) // diagonal = 1?
	{
		if ((tempMat.M[i][0] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((tempMat.M[j][0] & idM64[i]) == idM64[i])
				{
					tempMat.M[j][0] ^= tempMat.M[i][0];
					tempMat.M[j][1] ^= tempMat.M[i][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 128; j++)
			{
				if ((tempMat.M[j][0] & idM64[i]) == idM64[i])
				{
					temp = tempMat.M[i][0];
					tempMat.M[i][0] = tempMat.M[j][0];
					tempMat.M[j][0] = temp;

					temp = tempMat.M[i][1];
					tempMat.M[i][1] = tempMat.M[j][1];
					tempMat.M[j][1] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				p = i + 1 + cus_random() % (127 - i); // swap

				temp = tempMat.M[p][0];
				tempMat.M[p][0] = tempMat.M[i][0];
				tempMat.M[i][0] = temp;

				temp = tempMat.M[p][1];
				tempMat.M[p][1] = tempMat.M[i][1];
				tempMat.M[i][1] = temp;

				temp = (*Mat_inv).M[p][0];
				(*Mat_inv).M[p][0] = (*Mat_inv).M[i][0];
				(*Mat_inv).M[i][0] = temp;

				temp = (*Mat_inv).M[p][1];
				(*Mat_inv).M[p][1] = (*Mat_inv).M[i][1];
				(*Mat_inv).M[i][1] = temp;

				trail[times][0] = 0;
				trail[times][1] = p;
				trail[times][2] = i;
				times++;

				for (t = i + 1; t < 128; t++)
				{
					if (cus_random() % 2)
					{
						tempMat.M[t][0] ^= tempMat.M[i][0];
						tempMat.M[t][1] ^= tempMat.M[i][1];

						(*Mat_inv).M[t][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[t][1] ^= (*Mat_inv).M[i][1];
						trail[times][0] = 1;
						trail[times][1] = t;
						trail[times][2] = i;
						times++;
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 128; k++)
				{
					if ((tempMat.M[k][0] & idM64[i]) == idM64[i])
					{
						tempMat.M[k][0] ^= tempMat.M[i][0];
						tempMat.M[k][1] ^= tempMat.M[i][1];

						(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	for (i = 64; i < 128; i++) // diagonal = 1?
	{
		if ((tempMat.M[i][1] & idM64[i - 64]) == idM64[i - 64])
		{
			for (j = i + 1; j < 128; j++)
			{
				if ((tempMat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					tempMat.M[j][1] ^= tempMat.M[i][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 128; j++)
			{
				if ((tempMat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					temp = tempMat.M[i][1];
					tempMat.M[i][1] = tempMat.M[j][1];
					tempMat.M[j][1] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 127)
				{
					p = i + 1 + cus_random() % (127 - i); // swap

					temp = tempMat.M[p][1];
					tempMat.M[p][1] = tempMat.M[i][1];
					tempMat.M[i][1] = temp;

					temp = (*Mat_inv).M[p][0];
					(*Mat_inv).M[p][0] = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = temp;

					temp = (*Mat_inv).M[p][1];
					(*Mat_inv).M[p][1] = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = temp;

					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;

					for (t = i + 1; t < 128; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t][1] ^= tempMat.M[i][1];

							(*Mat_inv).M[t][0] ^= (*Mat_inv).M[i][0];
							(*Mat_inv).M[t][1] ^= (*Mat_inv).M[i][1];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 128; k++)
				{
					if ((tempMat.M[k][1] & idM64[i - 64]) == idM64[i - 64])
					{
						tempMat.M[k][1] ^= tempMat.M[i][1];

						(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 127; t >= 64; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][1] & idM64[t - 64]) == idM64[t - 64])
				{
					tempMat.M[j][1] ^= tempMat.M[t][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[t][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[t][1];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}
		for (t = 63; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][0] & idM64[t]) == idM64[t])
				{
					tempMat.M[j][0] ^= tempMat.M[t][0];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[t][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[t][1];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]][0] ^= (*Mat).M[trail[j][2]][0];
				(*Mat).M[trail[j][1]][1] ^= (*Mat).M[trail[j][2]][1];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]][0];
				(*Mat).M[trail[j][1]][0] = (*Mat).M[trail[j][2]][0];
				(*Mat).M[trail[j][2]][0] = temp;

				temp = (*Mat).M[trail[j][1]][1];
				(*Mat).M[trail[j][1]][1] = (*Mat).M[trail[j][2]][1];
				(*Mat).M[trail[j][2]][1] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 127; i >= 64; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					tempMat.M[j][1] ^= tempMat.M[i][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				}
			}
		}
		for (i = 63; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][0] & idM64[i]) == idM64[i])
				{
					tempMat.M[j][0] ^= tempMat.M[i][0];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
				}
			}
		}
		opt_copyM128(resultMat, Mat);
	}
}

void opt_genMatpairM256(M256 *Mat, M256 *Mat_inv) // generate 256*256 invertible matrix and its inverse matrix
{
	int i, j, t, k;
	int p, q;
	M256 tempMat;
	M256 resultMat;
	uint64_t temp;
	uint8_t trail[65536][3]; // generate trail
	int flag = 0;
	int times = 0;
	int invertible = 1;
	InitRandom((randseed++) ^ ((unsigned int)time(NULL)));
	opt_identityM256(Mat);
	opt_identityM256(Mat_inv);
	opt_randM256(&tempMat);
	opt_copyM256(tempMat, &resultMat);
	for (i = 0; i < 64; i++) // diagonal = 1?
	{
		if ((tempMat.M[i][0] & idM64[i]) == idM64[i])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][0] & idM64[i]) == idM64[i])
				{
					tempMat.M[j][0] ^= tempMat.M[i][0];
					tempMat.M[j][1] ^= tempMat.M[i][1];
					tempMat.M[j][2] ^= tempMat.M[i][2];
					tempMat.M[j][3] ^= tempMat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][0] & idM64[i]) == idM64[i])
				{
					temp = tempMat.M[i][0];
					tempMat.M[i][0] = tempMat.M[j][0];
					tempMat.M[j][0] = temp;

					temp = tempMat.M[i][1];
					tempMat.M[i][1] = tempMat.M[j][1];
					tempMat.M[j][1] = temp;

					temp = tempMat.M[i][2];
					tempMat.M[i][2] = tempMat.M[j][2];
					tempMat.M[j][2] = temp;

					temp = tempMat.M[i][3];
					tempMat.M[i][3] = tempMat.M[j][3];
					tempMat.M[j][3] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				p = i + 1 + cus_random() % (255 - i); // swap

				temp = tempMat.M[p][0];
				tempMat.M[p][0] = tempMat.M[i][0];
				tempMat.M[i][0] = temp;

				temp = tempMat.M[p][1];
				tempMat.M[p][1] = tempMat.M[i][1];
				tempMat.M[i][1] = temp;

				temp = tempMat.M[p][2];
				tempMat.M[p][2] = tempMat.M[i][2];
				tempMat.M[i][2] = temp;

				temp = tempMat.M[p][3];
				tempMat.M[p][3] = tempMat.M[i][3];
				tempMat.M[i][3] = temp;

				temp = (*Mat_inv).M[p][0];
				(*Mat_inv).M[p][0] = (*Mat_inv).M[i][0];
				(*Mat_inv).M[i][0] = temp;

				temp = (*Mat_inv).M[p][1];
				(*Mat_inv).M[p][1] = (*Mat_inv).M[i][1];
				(*Mat_inv).M[i][1] = temp;

				temp = (*Mat_inv).M[p][2];
				(*Mat_inv).M[p][2] = (*Mat_inv).M[i][2];
				(*Mat_inv).M[i][2] = temp;

				temp = (*Mat_inv).M[p][3];
				(*Mat_inv).M[p][3] = (*Mat_inv).M[i][3];
				(*Mat_inv).M[i][3] = temp;

				trail[times][0] = 0;
				trail[times][1] = p;
				trail[times][2] = i;
				times++;

				for (t = i + 1; t < 256; t++)
				{
					if (cus_random() % 2)
					{
						tempMat.M[t][0] ^= tempMat.M[i][0];
						tempMat.M[t][1] ^= tempMat.M[i][1];
						tempMat.M[t][2] ^= tempMat.M[i][2];
						tempMat.M[t][3] ^= tempMat.M[i][3];

						(*Mat_inv).M[t][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[t][1] ^= (*Mat_inv).M[i][1];
						(*Mat_inv).M[t][2] ^= (*Mat_inv).M[i][2];
						(*Mat_inv).M[t][3] ^= (*Mat_inv).M[i][3];
						trail[times][0] = 1;
						trail[times][1] = t;
						trail[times][2] = i;
						times++;
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 256; k++)
				{
					if ((tempMat.M[k][0] & idM64[i]) == idM64[i])
					{
						tempMat.M[k][0] ^= tempMat.M[i][0];
						tempMat.M[k][1] ^= tempMat.M[i][1];
						tempMat.M[k][2] ^= tempMat.M[i][2];
						tempMat.M[k][3] ^= tempMat.M[i][3];

						(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
						(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
						(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	for (i = 64; i < 128; i++) // diagonal = 1?
	{
		if ((tempMat.M[i][1] & idM64[i - 64]) == idM64[i - 64])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					tempMat.M[j][1] ^= tempMat.M[i][1];
					tempMat.M[j][2] ^= tempMat.M[i][2];
					tempMat.M[j][3] ^= tempMat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					temp = tempMat.M[i][1];
					tempMat.M[i][1] = tempMat.M[j][1];
					tempMat.M[j][1] = temp;

					temp = tempMat.M[i][2];
					tempMat.M[i][2] = tempMat.M[j][2];
					tempMat.M[j][2] = temp;

					temp = tempMat.M[i][3];
					tempMat.M[i][3] = tempMat.M[j][3];
					tempMat.M[j][3] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				// if(i < 127)
				{
					p = i + 1 + cus_random() % (255 - i); // swap

					temp = tempMat.M[p][1];
					tempMat.M[p][1] = tempMat.M[i][1];
					tempMat.M[i][1] = temp;

					temp = tempMat.M[p][2];
					tempMat.M[p][2] = tempMat.M[i][2];
					tempMat.M[i][2] = temp;

					temp = tempMat.M[p][3];
					tempMat.M[p][3] = tempMat.M[i][3];
					tempMat.M[i][3] = temp;

					temp = (*Mat_inv).M[p][0];
					(*Mat_inv).M[p][0] = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = temp;

					temp = (*Mat_inv).M[p][1];
					(*Mat_inv).M[p][1] = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = temp;

					temp = (*Mat_inv).M[p][2];
					(*Mat_inv).M[p][2] = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = temp;

					temp = (*Mat_inv).M[p][3];
					(*Mat_inv).M[p][3] = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;

					for (t = i + 1; t < 256; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t][1] ^= tempMat.M[i][1];
							tempMat.M[t][2] ^= tempMat.M[i][2];
							tempMat.M[t][3] ^= tempMat.M[i][3];

							(*Mat_inv).M[t][0] ^= (*Mat_inv).M[i][0];
							(*Mat_inv).M[t][1] ^= (*Mat_inv).M[i][1];
							(*Mat_inv).M[t][2] ^= (*Mat_inv).M[i][2];
							(*Mat_inv).M[t][3] ^= (*Mat_inv).M[i][3];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 256; k++)
				{
					if ((tempMat.M[k][1] & idM64[i - 64]) == idM64[i - 64])
					{
						tempMat.M[k][1] ^= tempMat.M[i][1];
						tempMat.M[k][2] ^= tempMat.M[i][2];
						tempMat.M[k][3] ^= tempMat.M[i][3];

						(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
						(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
						(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	for (i = 128; i < 192; i++) // diagonal = 1?
	{
		if ((tempMat.M[i][2] & idM64[i - 128]) == idM64[i - 128])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					tempMat.M[j][2] ^= tempMat.M[i][2];
					tempMat.M[j][3] ^= tempMat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					temp = tempMat.M[i][2];
					tempMat.M[i][2] = tempMat.M[j][2];
					tempMat.M[j][2] = temp;

					temp = tempMat.M[i][3];
					tempMat.M[i][3] = tempMat.M[j][3];
					tempMat.M[j][3] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				// if(i < 127)
				{
					p = i + 1 + cus_random() % (255 - i); // swap

					temp = tempMat.M[p][2];
					tempMat.M[p][2] = tempMat.M[i][2];
					tempMat.M[i][2] = temp;

					temp = tempMat.M[p][3];
					tempMat.M[p][3] = tempMat.M[i][3];
					tempMat.M[i][3] = temp;

					temp = (*Mat_inv).M[p][0];
					(*Mat_inv).M[p][0] = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = temp;

					temp = (*Mat_inv).M[p][1];
					(*Mat_inv).M[p][1] = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = temp;

					temp = (*Mat_inv).M[p][2];
					(*Mat_inv).M[p][2] = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = temp;

					temp = (*Mat_inv).M[p][3];
					(*Mat_inv).M[p][3] = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;

					for (t = i + 1; t < 256; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t][2] ^= tempMat.M[i][2];
							tempMat.M[t][3] ^= tempMat.M[i][3];

							(*Mat_inv).M[t][0] ^= (*Mat_inv).M[i][0];
							(*Mat_inv).M[t][1] ^= (*Mat_inv).M[i][1];
							(*Mat_inv).M[t][2] ^= (*Mat_inv).M[i][2];
							(*Mat_inv).M[t][3] ^= (*Mat_inv).M[i][3];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 256; k++)
				{
					if ((tempMat.M[k][2] & idM64[i - 128]) == idM64[i - 128])
					{
						tempMat.M[k][2] ^= tempMat.M[i][2];
						tempMat.M[k][3] ^= tempMat.M[i][3];

						(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
						(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
						(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	for (i = 192; i < 256; i++) // diagonal = 1?
	{
		if ((tempMat.M[i][3] & idM64[i - 192]) == idM64[i - 192])
		{
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					tempMat.M[j][3] ^= tempMat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
				}
			}
		}
		else // swap to find 1
		{
			flag = 1;
			for (j = i + 1; j < 256; j++)
			{
				if ((tempMat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					temp = tempMat.M[i][3];
					tempMat.M[i][3] = tempMat.M[j][3];
					tempMat.M[j][3] = temp;

					flag = 0;

					temp = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = (*Mat_inv).M[j][0];
					(*Mat_inv).M[j][0] = temp;

					temp = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = (*Mat_inv).M[j][1];
					(*Mat_inv).M[j][1] = temp;

					temp = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = (*Mat_inv).M[j][2];
					(*Mat_inv).M[j][2] = temp;

					temp = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = (*Mat_inv).M[j][3];
					(*Mat_inv).M[j][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = j;
					trail[times][2] = i;
					times++;
					break;
				}
			}
			if (flag) // can not find 1 which means not invertible
			{
				invertible = 0;
				if (i < 255)
				{
					p = i + 1 + cus_random() % (255 - i); // swap

					temp = tempMat.M[p][3];
					tempMat.M[p][3] = tempMat.M[i][3];
					tempMat.M[i][3] = temp;

					temp = (*Mat_inv).M[p][0];
					(*Mat_inv).M[p][0] = (*Mat_inv).M[i][0];
					(*Mat_inv).M[i][0] = temp;

					temp = (*Mat_inv).M[p][1];
					(*Mat_inv).M[p][1] = (*Mat_inv).M[i][1];
					(*Mat_inv).M[i][1] = temp;

					temp = (*Mat_inv).M[p][2];
					(*Mat_inv).M[p][2] = (*Mat_inv).M[i][2];
					(*Mat_inv).M[i][2] = temp;

					temp = (*Mat_inv).M[p][3];
					(*Mat_inv).M[p][3] = (*Mat_inv).M[i][3];
					(*Mat_inv).M[i][3] = temp;

					trail[times][0] = 0;
					trail[times][1] = p;
					trail[times][2] = i;
					times++;

					for (t = i + 1; t < 256; t++)
					{
						if (cus_random() % 2)
						{
							tempMat.M[t][3] ^= tempMat.M[i][3];

							(*Mat_inv).M[t][0] ^= (*Mat_inv).M[i][0];
							(*Mat_inv).M[t][1] ^= (*Mat_inv).M[i][1];
							(*Mat_inv).M[t][2] ^= (*Mat_inv).M[i][2];
							(*Mat_inv).M[t][3] ^= (*Mat_inv).M[i][3];
							trail[times][0] = 1;
							trail[times][1] = t;
							trail[times][2] = i;
							times++;
						}
					}
				}
			}
			else // can still contiune
			{
				for (k = i + 1; k < 256; k++)
				{
					if ((tempMat.M[k][3] & idM64[i - 192]) == idM64[i - 192])
					{
						tempMat.M[k][3] ^= tempMat.M[i][3];

						(*Mat_inv).M[k][0] ^= (*Mat_inv).M[i][0];
						(*Mat_inv).M[k][1] ^= (*Mat_inv).M[i][1];
						(*Mat_inv).M[k][2] ^= (*Mat_inv).M[i][2];
						(*Mat_inv).M[k][3] ^= (*Mat_inv).M[i][3];

						trail[times][0] = 1;
						trail[times][1] = k;
						trail[times][2] = i;
						times++;
					}
				}
			}
		}
	}
	if (!invertible) // not invertible
	{
		for (t = 255; t >= 192; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][3] & idM64[t - 192]) == idM64[t - 192])
				{
					tempMat.M[j][3] ^= tempMat.M[t][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[t][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[t][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[t][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[t][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}
		for (t = 191; t >= 128; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][2] & idM64[t - 128]) == idM64[t - 128])
				{
					tempMat.M[j][2] ^= tempMat.M[t][2];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[t][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[t][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[t][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[t][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}
		for (t = 127; t >= 64; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][1] & idM64[t - 64]) == idM64[t - 64])
				{
					tempMat.M[j][1] ^= tempMat.M[t][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[t][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[t][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[t][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[t][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}
		for (t = 63; t >= 0; t--)
		{
			for (j = t - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][0] & idM64[t]) == idM64[t])
				{
					tempMat.M[j][0] ^= tempMat.M[t][0];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[t][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[t][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[t][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[t][3];

					trail[times][0] = 1;
					trail[times][1] = j;
					trail[times][2] = t;
					times++;
				}
			}
		}

		for (j = times - 1; j >= 0; j--) // generate inverse matrix
		{
			if (trail[j][0]) // add
			{
				(*Mat).M[trail[j][1]][0] ^= (*Mat).M[trail[j][2]][0];
				(*Mat).M[trail[j][1]][1] ^= (*Mat).M[trail[j][2]][1];
				(*Mat).M[trail[j][1]][2] ^= (*Mat).M[trail[j][2]][2];
				(*Mat).M[trail[j][1]][3] ^= (*Mat).M[trail[j][2]][3];
			}
			else // swap
			{
				temp = (*Mat).M[trail[j][1]][0];
				(*Mat).M[trail[j][1]][0] = (*Mat).M[trail[j][2]][0];
				(*Mat).M[trail[j][2]][0] = temp;

				temp = (*Mat).M[trail[j][1]][1];
				(*Mat).M[trail[j][1]][1] = (*Mat).M[trail[j][2]][1];
				(*Mat).M[trail[j][2]][1] = temp;

				temp = (*Mat).M[trail[j][1]][2];
				(*Mat).M[trail[j][1]][2] = (*Mat).M[trail[j][2]][2];
				(*Mat).M[trail[j][2]][2] = temp;

				temp = (*Mat).M[trail[j][1]][3];
				(*Mat).M[trail[j][1]][3] = (*Mat).M[trail[j][2]][3];
				(*Mat).M[trail[j][2]][3] = temp;
			}
		}
	}
	else // invertible
	{
		for (i = 255; i >= 192; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][3] & idM64[i - 192]) == idM64[i - 192])
				{
					tempMat.M[j][3] ^= tempMat.M[i][3];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		for (i = 191; i >= 128; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][2] & idM64[i - 128]) == idM64[i - 128])
				{
					tempMat.M[j][2] ^= tempMat.M[i][2];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		for (i = 127; i >= 64; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][1] & idM64[i - 64]) == idM64[i - 64])
				{
					tempMat.M[j][1] ^= tempMat.M[i][1];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		for (i = 63; i >= 0; i--)
		{
			for (j = i - 1; j >= 0; j--)
			{
				if ((tempMat.M[j][0] & idM64[i]) == idM64[i])
				{
					tempMat.M[j][0] ^= tempMat.M[i][0];

					(*Mat_inv).M[j][0] ^= (*Mat_inv).M[i][0];
					(*Mat_inv).M[j][1] ^= (*Mat_inv).M[i][1];
					(*Mat_inv).M[j][2] ^= (*Mat_inv).M[i][2];
					(*Mat_inv).M[j][3] ^= (*Mat_inv).M[i][3];
				}
			}
		}
		opt_copyM256(resultMat, Mat);
	}
}

// generate a pair of affine
void opt_genaffinepairM4(Aff4 *aff, Aff4 *aff_inv)
{
	opt_genMatpairM4(&(aff->Mat), &(aff_inv->Mat));
	opt_randV4(&(aff->Vec));
	opt_MatMulVecM4((*aff_inv).Mat, (*aff).Vec, &(aff_inv->Vec));
}

// generate a pair of affine
void opt_genaffinepairM8(Aff8 *aff, Aff8 *aff_inv)
{
	opt_genMatpairM8(&(aff->Mat), &(aff_inv->Mat));
	opt_randV8(&(aff->Vec));
	opt_MatMulVecM8((*aff_inv).Mat, (*aff).Vec, &(aff_inv->Vec));
}

// generate a pair of affine
void opt_genaffinepairM16(Aff16 *aff, Aff16 *aff_inv)
{
	opt_genMatpairM16(&(aff->Mat), &(aff_inv->Mat));
	opt_randV16(&(aff->Vec));
	opt_MatMulVecM16((*aff_inv).Mat, (*aff).Vec, &(aff_inv->Vec));
}

// generate a pair of affine
void opt_genaffinepairM32(Aff32 *aff, Aff32 *aff_inv)
{
	opt_genMatpairM32(&(aff->Mat), &(aff_inv->Mat));
	opt_randV32(&(aff->Vec));
	opt_MatMulVecM32((*aff_inv).Mat, (*aff).Vec, &(aff_inv->Vec));
}

// generate a pair of affine
void opt_genaffinepairM64(Aff64 *aff, Aff64 *aff_inv)
{
	opt_genMatpairM64(&(aff->Mat), &(aff_inv->Mat));
	opt_randV64(&(aff->Vec));
	opt_MatMulVecM64((*aff_inv).Mat, (*aff).Vec, &(aff_inv->Vec));
}

// generate a pair of affine
void opt_genaffinepairM128(Aff128 *aff, Aff128 *aff_inv)
{
	opt_genMatpairM128(&(aff->Mat), &(aff_inv->Mat));
	opt_randV128(&(aff->Vec));
	opt_MatMulVecM128((*aff_inv).Mat, (*aff).Vec, &(aff_inv->Vec));
}

// diagonal matrix concatenation, four 8*8 -> 32*32
void opt_MatrixcomM8to32(M8 m1, M8 m2, M8 m3, M8 m4, M32 *mat)
{
	int i;
	int j = 0;
	uint8_t *m;
	opt_initM32(mat);
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 3) = m1.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 2) = m2.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 1) = m3.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*m = m4.M[i];
		j++;
	}
}

// 4 vectors concatenation
void opt_VectorcomV8to32(V8 v1, V8 v2, V8 v3, V8 v4, V32 *vec)
{
	uint8_t *v;
	v = (uint8_t *)&(*vec).V;
	*(v + 3) = v1.V;
	*(v + 2) = v2.V;
	*(v + 1) = v3.V;
	*v = v4.V;
}

// diagonal affine concatenation, four 8*8 -> 32*32
void opt_affinecomM8to32(Aff8 aff1, Aff8 aff2, Aff8 aff3, Aff8 aff4, Aff32 *aff)
{
	opt_MatrixcomM8to32(aff1.Mat, aff2.Mat, aff3.Mat, aff4.Mat, &(aff->Mat));
	opt_VectorcomV8to32(aff1.Vec, aff2.Vec, aff3.Vec, aff4.Vec, &(aff->Vec));
}

// diagonal matrix concatenation, four 16*16 -> 64*64
void opt_MatrixcomM16to64(M16 m1, M16 m2, M16 m3, M16 m4, M64 *mat)
{
	int i;
	int j = 0;
	uint16_t *m;
	opt_initM64(mat);
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j];
		*(m + 3) = m1.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j];
		*(m + 2) = m2.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j];
		*(m + 1) = m3.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j];
		*m = m4.M[i];
		j++;
	}
}

// 4 vectors concatenation
void opt_VectorcomV16to64(V16 v1, V16 v2, V16 v3, V16 v4, V64 *vec)
{
	uint16_t *v;
	v = (uint16_t *)&(*vec).V;
	*(v + 3) = v1.V;
	*(v + 2) = v2.V;
	*(v + 1) = v3.V;
	*v = v4.V;
}

// diagonal affine concatenation,four 16*16 -> 64*64
void opt_affinecomM16to64(Aff16 aff1, Aff16 aff2, Aff16 aff3, Aff16 aff4,
						  Aff64 *aff)
{
	opt_MatrixcomM16to64(aff1.Mat, aff2.Mat, aff3.Mat, aff4.Mat, &(aff->Mat));
	opt_VectorcomV16to64(aff1.Vec, aff2.Vec, aff3.Vec, aff4.Vec, &(aff->Vec));
}

// diagonal matrix concatenation,four 8*8 -> 64*64
void opt_MatrixcomM8to64(M8 m1, M8 m2, M8 m3, M8 m4, M8 m5, M8 m6, M8 m7, M8 m8,
						 M64 *mat)
{
	int i;
	int j = 0;
	uint8_t *m;
	opt_initM64(mat);
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 7) = m1.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 6) = m2.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 5) = m3.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 4) = m4.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 3) = m5.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 2) = m6.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*(m + 1) = m7.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j];
		*m = m8.M[i];
		j++;
	}
}

// 8 vectors concatenation
void opt_VectorcomV8to64(V8 v1, V8 v2, V8 v3, V8 v4, V8 v5, V8 v6, V8 v7, V8 v8,
						 V64 *vec)
{
	uint8_t *v;
	v = (uint8_t *)&(*vec).V;
	*(v + 7) = v1.V;
	*(v + 6) = v2.V;
	*(v + 5) = v3.V;
	*(v + 4) = v4.V;
	*(v + 3) = v5.V;
	*(v + 2) = v6.V;
	*(v + 1) = v7.V;
	*v = v8.V;
}

// diagonal affine concatenation, four 8*8 -> 64*64
void opt_affinecomM8to64(Aff8 aff1, Aff8 aff2, Aff8 aff3, Aff8 aff4, Aff8 aff5,
						 Aff8 aff6, Aff8 aff7, Aff8 aff8, Aff64 *aff)
{
	opt_MatrixcomM8to64(aff1.Mat, aff2.Mat, aff3.Mat, aff4.Mat, aff5.Mat, aff6.Mat,
						aff7.Mat, aff8.Mat, &(aff->Mat));
	opt_VectorcomV8to64(aff1.Vec, aff2.Vec, aff3.Vec, aff4.Vec, aff5.Vec, aff6.Vec,
						aff7.Vec, aff8.Vec, &(aff->Vec));
}

// diagonal matrix concatenation, four 32*32 -> 128*128
void opt_MatrixcomM32to128(M32 m1, M32 m2, M32 m3, M32 m4, M128 *mat)
{
	int i;
	int j = 0;
	uint32_t *m;
	opt_initM128(mat);
	for (i = 0; i < 32; i++)
	{
		m = (uint32_t *)&(*mat).M[j][0];
		*(m + 1) = m1.M[i];
		j++;
	}
	for (i = 0; i < 32; i++)
	{
		m = (uint32_t *)&(*mat).M[j][0];
		*m = m2.M[i];
		j++;
	}
	for (i = 0; i < 32; i++)
	{
		m = (uint32_t *)&(*mat).M[j][1];
		*(m + 1) = m3.M[i];
		j++;
	}
	for (i = 0; i < 32; i++)
	{
		m = (uint32_t *)&(*mat).M[j][1];
		*m = m4.M[i];
		j++;
	}
}

// 4 vectors concatenation
void opt_VectorcomV32to128(V32 v1, V32 v2, V32 v3, V32 v4, V128 *vec)
{
	uint32_t *v;
	v = (uint32_t *)&(*vec).V[0];
	*(v + 1) = v1.V;
	*v = v2.V;
	v = (uint32_t *)&(*vec).V[1];
	*(v + 1) = v3.V;
	*v = v4.V;
}

// diagonal affine concatenation, four 32*32 -> 128*128
void opt_affinecomM32to128(Aff32 aff1, Aff32 aff2, Aff32 aff3, Aff32 aff4,
						   Aff128 *aff)
{
	opt_MatrixcomM32to128(aff1.Mat, aff2.Mat, aff3.Mat, aff4.Mat, &(aff->Mat));
	opt_VectorcomV32to128(aff1.Vec, aff2.Vec, aff3.Vec, aff4.Vec, &(aff->Vec));
}

// diagonal matrix concatenation, 16 8*8 -> 128*128
void opt_MatrixcomM8to128(M8 m1, M8 m2, M8 m3, M8 m4, M8 m5, M8 m6, M8 m7, M8 m8,
						  M8 m9, M8 m10, M8 m11, M8 m12, M8 m13, M8 m14, M8 m15,
						  M8 m16, M128 *mat)
{
	int i;
	int j = 0;
	uint8_t *m;
	opt_initM128(mat);
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 7) = m1.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 6) = m2.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 5) = m3.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 4) = m4.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 3) = m5.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 2) = m6.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*(m + 1) = m7.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][0];
		*m = m8.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 7) = m9.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 6) = m10.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 5) = m11.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 4) = m12.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 3) = m13.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 2) = m14.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*(m + 1) = m15.M[i];
		j++;
	}
	for (i = 0; i < 8; i++)
	{
		m = (uint8_t *)&(*mat).M[j][1];
		*m = m16.M[i];
		j++;
	}
}

// 16 vectors concatenation
void opt_VectorcomV8to128(V8 v1, V8 v2, V8 v3, V8 v4, V8 v5, V8 v6, V8 v7, V8 v8,
						  V8 v9, V8 v10, V8 v11, V8 v12, V8 v13, V8 v14, V8 v15,
						  V8 v16, V128 *vec)
{
	uint8_t *v;
	v = (uint8_t *)&(*vec).V[0];
	*(v + 7) = v1.V;
	*(v + 6) = v2.V;
	*(v + 5) = v3.V;
	*(v + 4) = v4.V;
	*(v + 3) = v5.V;
	*(v + 2) = v6.V;
	*(v + 1) = v7.V;
	*v = v8.V;
	v = (uint8_t *)&(*vec).V[1];
	*(v + 7) = v9.V;
	*(v + 6) = v10.V;
	*(v + 5) = v11.V;
	*(v + 4) = v12.V;
	*(v + 3) = v13.V;
	*(v + 2) = v14.V;
	*(v + 1) = v15.V;
	*v = v16.V;
}

// diagonal affine concatenation, 16 8*8 -> 128*128
void opt_affinecomM8to128(Aff8 aff1, Aff8 aff2, Aff8 aff3, Aff8 aff4, Aff8 aff5,
						  Aff8 aff6, Aff8 aff7, Aff8 aff8, Aff8 aff9, Aff8 aff10,
						  Aff8 aff11, Aff8 aff12, Aff8 aff13, Aff8 aff14,
						  Aff8 aff15, Aff8 aff16, Aff128 *aff)
{
	opt_MatrixcomM8to128(aff1.Mat, aff2.Mat, aff3.Mat, aff4.Mat, aff5.Mat, aff6.Mat,
						 aff7.Mat, aff8.Mat, aff9.Mat, aff10.Mat, aff11.Mat,
						 aff12.Mat, aff13.Mat, aff14.Mat, aff15.Mat, aff16.Mat,
						 &(aff->Mat));
	opt_VectorcomV8to128(aff1.Vec, aff2.Vec, aff3.Vec, aff4.Vec, aff5.Vec, aff6.Vec,
						 aff7.Vec, aff8.Vec, aff9.Vec, aff10.Vec, aff11.Vec,
						 aff12.Vec, aff13.Vec, aff14.Vec, aff15.Vec, aff16.Vec,
						 &(aff->Vec));
}

// diagonal matrix concatenation, 8 16*16 -> 128*128
void opt_MatrixcomM16to128(M16 m1, M16 m2, M16 m3, M16 m4, M16 m5, M16 m6, M16 m7,
						   M16 m8, M128 *mat)
{
	int i;
	int j = 0;
	uint16_t *m;
	opt_initM128(mat);
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][0];
		*(m + 3) = m1.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][0];
		*(m + 2) = m2.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][0];
		*(m + 1) = m3.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][0];
		*m = m4.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][1];
		*(m + 3) = m5.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][1];
		*(m + 2) = m6.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][1];
		*(m + 1) = m7.M[i];
		j++;
	}
	for (i = 0; i < 16; i++)
	{
		m = (uint16_t *)&(*mat).M[j][1];
		*m = m8.M[i];
		j++;
	}
}

// 8 vectors concatenation
void opt_VectorcomV16to128(V16 v1, V16 v2, V16 v3, V16 v4, V16 v5, V16 v6, V16 v7,
						   V16 v8, V128 *vec)
{
	uint16_t *v;
	v = (uint16_t *)&(*vec).V[0];
	*(v + 3) = v1.V;
	*(v + 2) = v2.V;
	*(v + 1) = v3.V;
	*v = v4.V;
	v = (uint16_t *)&(*vec).V[1];
	*(v + 3) = v5.V;
	*(v + 2) = v6.V;
	*(v + 1) = v7.V;
	*v = v8.V;
}

// diagonal affine concatenation, 8 16*16 -> 128*128
void opt_affinecomM16to128(Aff16 aff1, Aff16 aff2, Aff16 aff3, Aff16 aff4,
						   Aff16 aff5, Aff16 aff6, Aff16 aff7, Aff16 aff8,
						   Aff128 *aff)
{
	opt_MatrixcomM16to128(aff1.Mat, aff2.Mat, aff3.Mat, aff4.Mat, aff5.Mat, aff6.Mat,
						  aff7.Mat, aff8.Mat, &(aff->Mat));
	opt_VectorcomV16to128(aff1.Vec, aff2.Vec, aff3.Vec, aff4.Vec, aff5.Vec, aff6.Vec,
						  aff7.Vec, aff8.Vec, &(aff->Vec));
}

// matrix tansposition M4
void opt_MattransM4(M4 Mat, M4 *Mat_trans)
{
	int i, j;
	opt_initM4(Mat_trans);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (Mat.M[i] & idM4[j])
				(*Mat_trans).M[j] ^= idM4[i];
		}
	}
}

// // Original
// // matrix tansposition M8
// void opt_MattransM8(M8 Mat, M8 *Mat_trans) {
//   int i, j;
//   opt_initM8(Mat_trans);
//   for (i = 0; i < 8; i++) {
//     for (j = 0; j < 8; j++) {
//       if (Mat.M[i] & idM8[j]) (*Mat_trans).M[j] ^= idM8[i];
//     }
//   }
// }

// Optimized version
void opt_MattransM8(M8 Mat, M8 *Mat_trans)
{
	uint64_t x = 0;
	int i;

	for (i = 0; i <= 7; i++)
		x = x << 8 | Mat.M[i];

	x = x & 0xAA55AA55AA55AA55 | (x & 0x00AA00AA00AA00AA) << 7 |
		(x >> 7) & 0x00AA00AA00AA00AA;
	x = x & 0xCCCC3333CCCC3333 | (x & 0x0000CCCC0000CCCC) << 14 |
		(x >> 14) & 0x0000CCCC0000CCCC;
	x = x & 0xF0F0F0F00F0F0F0F | (x & 0x00000000F0F0F0F0) << 28 |
		(x >> 28) & 0x00000000F0F0F0F0;

	for (i = 7; i >= 0; i--)
	{
		(*Mat_trans).M[i] = x;
		x = x >> 8;
	}
}

// // Original
// // matrix tansposition M16
// void opt_MattransM16(M16 Mat, M16 *Mat_trans) {
//   int i, j;
//   opt_initM16(Mat_trans);
//   for (i = 0; i < 16; i++) {
//     for (j = 0; j < 16; j++) {
//       if (Mat.M[i] & idM16[j]) (*Mat_trans).M[j] ^= idM16[i];
//     }
//   }
// }

// Optimized version
void opt_MattransM16(M16 Mat, M16 *Mat_trans)
{
	int j, k;
	uint16_t m, t;
	m = 0x00FF;
	for (int i = 0; i < 16; i++)
		(*Mat_trans).M[i] = Mat.M[i];
	for (j = 8; j != 0; j = j >> 1, m = m ^ (m << j))
	{
		for (k = 0; k < 16; k = (k + j + 1) & (~j))
		{
			t = ((*Mat_trans).M[k] ^ ((*Mat_trans).M[k + j] >> j)) & m;
			(*Mat_trans).M[k] ^= t;
			(*Mat_trans).M[k + j] ^= (t << j);
		}
	}
}

// // Original
// // matrix tansposition M32
// void opt_MattransM32(M32 Mat, M32 *Mat_trans) {
//   int i, j;
//   opt_initM32(Mat_trans);
//   for (i = 0; i < 32; i++) {
//     for (j = 0; j < 32; j++) {
//       if (Mat.M[i] & idM32[j]) (*Mat_trans).M[j] ^= idM32[i];
//     }
//   }
// }

// Optimized version
void opt_MattransM32(M32 Mat, M32 *Mat_trans)
{
	int j, k;
	uint32_t m, t;
	m = 0x0000FFFF;
	for (int i = 0; i < 32; i++)
		(*Mat_trans).M[i] = Mat.M[i];
	for (j = 16; j != 0; j = j >> 1, m = m ^ (m << j))
	{
		for (k = 0; k < 32; k = (k + j + 1) & (~j))
		{
			t = ((*Mat_trans).M[k] ^ ((*Mat_trans).M[k + j] >> j)) & m;
			(*Mat_trans).M[k] ^= t;
			(*Mat_trans).M[k + j] ^= (t << j);
		}
	}
}

// // Original
// void MattransM64(M64 Mat, M64 *Mat_trans) // matrix tansposition M64
// {
// 	int i, j;
// 	uint64_t mask[6], k, k2, l, temp;
// 	mask[0] = 0x5555555555555555;
// 	mask[1] = 0x3333333333333333;
// 	mask[2] = 0x0f0f0f0f0f0f0f0f;
// 	mask[3] = 0x00ff00ff00ff00ff;
// 	mask[4] = 0x0000ffff0000ffff;
// 	mask[5] = 0x00000000ffffffff;
// 	for (j = 0; j < 6; j++)
// 	{
// 		k = 1 << j;
// 		k2 = k * 2;
// 		for (i = 0; i < 32; i++)
// 		{
// 			l = (k2 * i) % 63;
// 			temp = (Mat.M[l] & ~mask[j]) ^ ((Mat.M[l + k] & ~mask[j]) >> k);
// 			Mat.M[l + k] = (Mat.M[l + k] & mask[j]) ^ ((Mat.M[l] & mask[j]) << k);
// 			Mat.M[l] = temp;
// 		}
// 	}
// 	copyM64(Mat, Mat_trans);
// }

// Optimized version
void opt_MattransM64(M64 Mat, M64 *restrict Mat_trans)
{
	static const uint64_t mask[6] = {
		0x5555555555555555ULL,
		0x3333333333333333ULL,
		0x0f0f0f0f0f0f0f0fULL,
		0x00ff00ff00ff00ffULL,
		0x0000ffff0000ffffULL,
		0x00000000ffffffffULL};
	static const int k[6] = {1, 2, 4, 8, 16, 32};
	static const int k2[6] = {2, 4, 8, 16, 32, 64};

	static const int idx_table[6][32] = {
		{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62},
		{0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61},
		{0, 8, 16, 24, 32, 40, 48, 56, 1, 9, 17, 25, 33, 41, 49, 57, 2, 10, 18, 26, 34, 42, 50, 58, 3, 11, 19, 27, 35, 43, 51, 59},
		{0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51, 4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55},
		{0, 32, 1, 33, 2, 34, 3, 35, 4, 36, 5, 37, 6, 38, 7, 39, 8, 40, 9, 41, 10, 42, 11, 43, 12, 44, 13, 45, 14, 46, 15, 47},
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}};

	for (int stage = 0; stage < 6; ++stage)
	{
		const uint64_t m = mask[stage];
		const int kk = k[stage];

		for (int i = 0; i < 32; i += 4)
		{
			int l0 = idx_table[stage][i + 0];
			int l1 = idx_table[stage][i + 1];
			int l2 = idx_table[stage][i + 2];
			int l3 = idx_table[stage][i + 3];

			uint64_t tmp0 = (Mat.M[l0] & ~m) ^ ((Mat.M[l0 + kk] & ~m) >> kk);
			uint64_t tmp1 = (Mat.M[l1] & ~m) ^ ((Mat.M[l1 + kk] & ~m) >> kk);
			uint64_t tmp2 = (Mat.M[l2] & ~m) ^ ((Mat.M[l2 + kk] & ~m) >> kk);
			uint64_t tmp3 = (Mat.M[l3] & ~m) ^ ((Mat.M[l3 + kk] & ~m) >> kk);

			Mat.M[l0 + kk] = (Mat.M[l0 + kk] & m) ^ ((Mat.M[l0] & m) << kk);
			Mat.M[l1 + kk] = (Mat.M[l1 + kk] & m) ^ ((Mat.M[l1] & m) << kk);
			Mat.M[l2 + kk] = (Mat.M[l2 + kk] & m) ^ ((Mat.M[l2] & m) << kk);
			Mat.M[l3 + kk] = (Mat.M[l3 + kk] & m) ^ ((Mat.M[l3] & m) << kk);

			Mat.M[l0] = tmp0;
			Mat.M[l1] = tmp1;
			Mat.M[l2] = tmp2;
			Mat.M[l3] = tmp3;
		}
	}

	opt_copyM64(Mat, Mat_trans);
}

// // Original
// void MattransM128(M128 Mat, M128 *Mat_trans) // matrix tansposition M128
// {
// 	int i, j;
// 	uint64_t mask[6], k, k2, l, temp;
// 	mask[0] = 0x5555555555555555;
// 	mask[1] = 0x3333333333333333;
// 	mask[2] = 0x0f0f0f0f0f0f0f0f;
// 	mask[3] = 0x00ff00ff00ff00ff;
// 	mask[4] = 0x0000ffff0000ffff;
// 	mask[5] = 0x00000000ffffffff;
// 	for (j = 0; j < 6; j++)
// 	{
// 		k = 1 << j;
// 		k2 = k * 2;
// 		for (i = 0; i < 64; i++)
// 		{
// 			l = (k2 * i) % 127;
// 			temp = (Mat.M[l][0] & ~mask[j]) ^ ((Mat.M[l + k][0] & ~mask[j]) >> k);
// 			Mat.M[l + k][0] = (Mat.M[l + k][0] & mask[j]) ^ ((Mat.M[l][0] & mask[j]) << k);
// 			Mat.M[l][0] = temp;

// 			temp = (Mat.M[l][1] & ~mask[j]) ^ ((Mat.M[l + k][1] & ~mask[j]) >> k);
// 			Mat.M[l + k][1] = (Mat.M[l + k][1] & mask[j]) ^ ((Mat.M[l][1] & mask[j]) << k);
// 			Mat.M[l][1] = temp;
// 		}
// 	}
// 	for (i = 0; i < 64; i++)
// 	{
// 		temp = Mat.M[i + 64][0];
// 		Mat.M[i + 64][0] = Mat.M[i][1];
// 		Mat.M[i][1] = temp;
// 	}
// 	copyM128(Mat, Mat_trans);
// }

// Optimized version
// void opt_MattransM128(M128 Mat, M128 *restrict Mat_trans)
// {
// 	M64 blocks[4], blocks_t[4];
// 	// Partition Mat into 4 blocks of 64x64
// 	for (int i = 0; i < 64; i++)
// 	{
// 		blocks[0].M[i] = Mat.M[i][0];
// 		blocks[1].M[i] = Mat.M[i][1];
// 		blocks[2].M[i] = Mat.M[i + 64][0];
// 		blocks[3].M[i] = Mat.M[i + 64][1];
// 	}
// 	// Transpose each block
// 	opt_MattransM64(blocks[0], &blocks_t[0]);
// 	opt_MattransM64(blocks[1], &blocks_t[1]);
// 	opt_MattransM64(blocks[2], &blocks_t[2]);
// 	opt_MattransM64(blocks[3], &blocks_t[3]);
// 	// Reassemble transposed blocks
// 	for (int i = 0; i < 64; i++)
// 	{
// 		Mat_trans->M[i][0] = blocks_t[0].M[i];
// 		Mat_trans->M[i][1] = blocks_t[2].M[i];
// 		Mat_trans->M[i + 64][0] = blocks_t[1].M[i];
// 		Mat_trans->M[i + 64][1] = blocks_t[3].M[i];
// 	}
// }

// Use optimized M64 transpose.
void opt_MattransM128(M128 Mat, M128 *restrict Mat_trans)
{
	M64 blocks[4];
	M64 blocks_t[4];

	// Partition Mat into 4 blocks of 64x64 (unrolled by 4)
	for (int i = 0; i < 64; i += 4)
	{
		// i+0
		blocks[0].M[i + 0] = Mat.M[i + 0][0];
		blocks[1].M[i + 0] = Mat.M[i + 0][1];
		blocks[2].M[i + 0] = Mat.M[i + 0 + 64][0];
		blocks[3].M[i + 0] = Mat.M[i + 0 + 64][1];
		// i+1
		blocks[0].M[i + 1] = Mat.M[i + 1][0];
		blocks[1].M[i + 1] = Mat.M[i + 1][1];
		blocks[2].M[i + 1] = Mat.M[i + 1 + 64][0];
		blocks[3].M[i + 1] = Mat.M[i + 1 + 64][1];
		// i+2
		blocks[0].M[i + 2] = Mat.M[i + 2][0];
		blocks[1].M[i + 2] = Mat.M[i + 2][1];
		blocks[2].M[i + 2] = Mat.M[i + 2 + 64][0];
		blocks[3].M[i + 2] = Mat.M[i + 2 + 64][1];
		// i+3
		blocks[0].M[i + 3] = Mat.M[i + 3][0];
		blocks[1].M[i + 3] = Mat.M[i + 3][1];
		blocks[2].M[i + 3] = Mat.M[i + 3 + 64][0];
		blocks[3].M[i + 3] = Mat.M[i + 3 + 64][1];
	}

	// Transpose each 64x64 block (use your optimized M64 transpose)
	opt_MattransM64(blocks[0], &blocks_t[0]);
	opt_MattransM64(blocks[1], &blocks_t[1]);
	opt_MattransM64(blocks[2], &blocks_t[2]);
	opt_MattransM64(blocks[3], &blocks_t[3]);

	// Reassemble transposed blocks into Mat_trans (unrolled by 4)
	// Top 64 rows: [ blocks_t[0] , blocks_t[2] ]
	for (int i = 0; i < 64; i += 4)
	{
		Mat_trans->M[i + 0][0] = blocks_t[0].M[i + 0];
		Mat_trans->M[i + 0][1] = blocks_t[2].M[i + 0];
		Mat_trans->M[i + 1][0] = blocks_t[0].M[i + 1];
		Mat_trans->M[i + 1][1] = blocks_t[2].M[i + 1];
		Mat_trans->M[i + 2][0] = blocks_t[0].M[i + 2];
		Mat_trans->M[i + 2][1] = blocks_t[2].M[i + 2];
		Mat_trans->M[i + 3][0] = blocks_t[0].M[i + 3];
		Mat_trans->M[i + 3][1] = blocks_t[2].M[i + 3];
	}

	// Bottom 64 rows: [ blocks_t[1] , blocks_t[3] ]
	for (int i = 0; i < 64; i += 4)
	{
		Mat_trans->M[i + 64 + 0][0] = blocks_t[1].M[i + 0];
		Mat_trans->M[i + 64 + 0][1] = blocks_t[3].M[i + 0];
		Mat_trans->M[i + 64 + 1][0] = blocks_t[1].M[i + 1];
		Mat_trans->M[i + 64 + 1][1] = blocks_t[3].M[i + 1];
		Mat_trans->M[i + 64 + 2][0] = blocks_t[1].M[i + 2];
		Mat_trans->M[i + 64 + 2][1] = blocks_t[3].M[i + 2];
		Mat_trans->M[i + 64 + 3][0] = blocks_t[1].M[i + 3];
		Mat_trans->M[i + 64 + 3][1] = blocks_t[3].M[i + 3];
	}
}

// // Original
// void opt_MattransM256(M256 Mat, M256 *Mat_trans) // matrix tansposition M256
// {
// 	int i, j;
// 	uint64_t mask[6], k, k2, l, temp;
// 	mask[0] = 0x5555555555555555;
// 	mask[1] = 0x3333333333333333;
// 	mask[2] = 0x0f0f0f0f0f0f0f0f;
// 	mask[3] = 0x00ff00ff00ff00ff;
// 	mask[4] = 0x0000ffff0000ffff;
// 	mask[5] = 0x00000000ffffffff;
// 	for (j = 0; j < 6; j++)
// 	{
// 		k = 1 << j;
// 		k2 = k * 2;
// 		for (i = 0; i < 128; i++)
// 		{
// 			l = (k2 * i) % 255;
// 			temp = (Mat.M[l][0] & ~mask[j]) ^ ((Mat.M[l + k][0] & ~mask[j]) >> k);
// 			Mat.M[l + k][0] = (Mat.M[l + k][0] & mask[j]) ^ ((Mat.M[l][0] & mask[j]) << k);
// 			Mat.M[l][0] = temp;

// 			temp = (Mat.M[l][1] & ~mask[j]) ^ ((Mat.M[l + k][1] & ~mask[j]) >> k);
// 			Mat.M[l + k][1] = (Mat.M[l + k][1] & mask[j]) ^ ((Mat.M[l][1] & mask[j]) << k);
// 			Mat.M[l][1] = temp;

// 			temp = (Mat.M[l][2] & ~mask[j]) ^ ((Mat.M[l + k][2] & ~mask[j]) >> k);
// 			Mat.M[l + k][2] = (Mat.M[l + k][2] & mask[j]) ^ ((Mat.M[l][2] & mask[j]) << k);
// 			Mat.M[l][2] = temp;

// 			temp = (Mat.M[l][3] & ~mask[j]) ^ ((Mat.M[l + k][3] & ~mask[j]) >> k);
// 			Mat.M[l + k][3] = (Mat.M[l + k][3] & mask[j]) ^ ((Mat.M[l][3] & mask[j]) << k);
// 			Mat.M[l][3] = temp;
// 		}
// 	}
// 	for (i = 0; i < 64; i++)
// 	{
// 		temp = Mat.M[i + 64][0];
// 		Mat.M[i + 64][0] = Mat.M[i][1];
// 		Mat.M[i][1] = temp;

// 		temp = Mat.M[i + 64][2];
// 		Mat.M[i + 64][2] = Mat.M[i][3];
// 		Mat.M[i][3] = temp;

// 		temp = Mat.M[i + 192][0];
// 		Mat.M[i + 192][0] = Mat.M[i + 128][1];
// 		Mat.M[i + 128][1] = temp;

// 		temp = Mat.M[i + 192][2];
// 		Mat.M[i + 192][2] = Mat.M[i + 128][3];
// 		Mat.M[i + 128][3] = temp;
// 	}
// 	for (i = 0; i < 128; i++)
// 	{
// 		temp = Mat.M[i + 128][0];
// 		Mat.M[i + 128][0] = Mat.M[i][2];
// 		Mat.M[i][2] = temp;

// 		temp = Mat.M[i + 128][1];
// 		Mat.M[i + 128][1] = Mat.M[i][3];
// 		Mat.M[i][3] = temp;
// 	}
// 	opt_copyM256(Mat, Mat_trans);
// }

// Optimized version
void opt_MattransM256(M256 Mat, M256 *restrict Mat_trans)
{
	M64 blocks[16], blocks_t[16];

	// Partition the 256x256 matrix into 16 blocks of 64x64
	for (int i = 0; i < 64; i++)
	{
		// Top quarter rows
		blocks[0].M[i] = Mat.M[i][0];
		blocks[1].M[i] = Mat.M[i][1];
		blocks[2].M[i] = Mat.M[i][2];
		blocks[3].M[i] = Mat.M[i][3];

		// Second quarter rows
		blocks[4].M[i] = Mat.M[i + 64][0];
		blocks[5].M[i] = Mat.M[i + 64][1];
		blocks[6].M[i] = Mat.M[i + 64][2];
		blocks[7].M[i] = Mat.M[i + 64][3];

		// Third quarter rows
		blocks[8].M[i] = Mat.M[i + 128][0];
		blocks[9].M[i] = Mat.M[i + 128][1];
		blocks[10].M[i] = Mat.M[i + 128][2];
		blocks[11].M[i] = Mat.M[i + 128][3];

		// Bottom quarter rows
		blocks[12].M[i] = Mat.M[i + 192][0];
		blocks[13].M[i] = Mat.M[i + 192][1];
		blocks[14].M[i] = Mat.M[i + 192][2];
		blocks[15].M[i] = Mat.M[i + 192][3];
	}

	// Transpose each 64x64 block
	for (int b = 0; b < 16; b++)
	{
		opt_MattransM64(blocks[b], &blocks_t[b]);
	}

	// Reassemble the transposed blocks
	for (int i = 0; i < 64; i++)
	{
		// Top quarter rows of result
		Mat_trans->M[i][0] = blocks_t[0].M[i];
		Mat_trans->M[i][1] = blocks_t[4].M[i];
		Mat_trans->M[i][2] = blocks_t[8].M[i];
		Mat_trans->M[i][3] = blocks_t[12].M[i];

		// Second quarter rows of result
		Mat_trans->M[i + 64][0] = blocks_t[1].M[i];
		Mat_trans->M[i + 64][1] = blocks_t[5].M[i];
		Mat_trans->M[i + 64][2] = blocks_t[9].M[i];
		Mat_trans->M[i + 64][3] = blocks_t[13].M[i];

		// Third quarter rows of result
		Mat_trans->M[i + 128][0] = blocks_t[2].M[i];
		Mat_trans->M[i + 128][1] = blocks_t[6].M[i];
		Mat_trans->M[i + 128][2] = blocks_t[10].M[i];
		Mat_trans->M[i + 128][3] = blocks_t[14].M[i];

		// Bottom quarter rows of result
		Mat_trans->M[i + 192][0] = blocks_t[3].M[i];
		Mat_trans->M[i + 192][1] = blocks_t[7].M[i];
		Mat_trans->M[i + 192][2] = blocks_t[11].M[i];
		Mat_trans->M[i + 192][3] = blocks_t[15].M[i];
	}
}

void opt_MatAddMatM4(M4 Mat1, M4 Mat2, M4 *Mat)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		(*Mat).M[i] = Mat1.M[i] ^ Mat2.M[i];
	}
}

void opt_MatAddMatM8(M8 Mat1, M8 Mat2, M8 *Mat)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		(*Mat).M[i] = Mat1.M[i] ^ Mat2.M[i];
	}
}

void opt_MatAddMatM16(M16 Mat1, M16 Mat2, M16 *Mat)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		(*Mat).M[i] = Mat1.M[i] ^ Mat2.M[i];
	}
}

void opt_MatAddMatM32(M32 Mat1, M32 Mat2, M32 *Mat)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		(*Mat).M[i] = Mat1.M[i] ^ Mat2.M[i];
	}
}

void opt_MatAddMatM64(M64 Mat1, M64 Mat2, M64 *Mat)
{
	int i;
	for (i = 0; i < 64; i++)
	{
		(*Mat).M[i] = Mat1.M[i] ^ Mat2.M[i];
	}
}

void opt_MatAddMatM128(M128 Mat1, M128 Mat2, M128 *Mat)
{
	int i;
	for (i = 0; i < 128; i++)
	{
		(*Mat).M[i][0] = Mat1.M[i][0] ^ Mat2.M[i][0];
		(*Mat).M[i][1] = Mat1.M[i][1] ^ Mat2.M[i][1];
	}
}

void opt_MatAddMatM256(M256 Mat1, M256 Mat2, M256 *Mat)
{
	for (int i = 0; i < 256; i++)
	{
		(*Mat).M[i][0] = Mat1.M[i][0] ^ Mat2.M[i][0];
		(*Mat).M[i][1] = Mat1.M[i][1] ^ Mat2.M[i][1];
		(*Mat).M[i][2] = Mat1.M[i][2] ^ Mat2.M[i][2];
		(*Mat).M[i][3] = Mat1.M[i][3] ^ Mat2.M[i][3];
	}
}

// Original
// matrix multiplication 4*4 mul 4*4 -> 4*4
void opt_MatMulMatM4(M4 Mat1, M4 Mat2, M4 *Mat)
{
	int i, j;
	M4 Mat2_trans;
	opt_initM4(Mat);
	opt_MattransM4(Mat2, &Mat2_trans);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (opt_parityU4(Mat1.M[i] & Mat2_trans.M[j] & 0x0f))
				(*Mat).M[i] ^= idM4[j];
		}
	}
}

// Original
// matrix multiplication 8*8 mul 8*8 -> 8*8
void opt_MatMulMatM8(M8 Mat1, M8 Mat2, M8 *Mat)
{
	int i, j;
	M8 Mat2_trans;
	opt_initM8(Mat);
	opt_MattransM8(Mat2, &Mat2_trans);
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (opt_parityU8(Mat1.M[i] & Mat2_trans.M[j]))
				(*Mat).M[i] ^= idM8[j];
		}
	}
}

// Original
// matrix multiplication 16*16 mul 16*16 -> 16*16
void opt_MatMulMatM16(M16 Mat1, M16 Mat2, M16 *Mat)
{
	int i, j;
	M16 Mat2_trans;
	opt_initM16(Mat);
	opt_MattransM16(Mat2, &Mat2_trans);
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			if (opt_parityU16(Mat1.M[i] & Mat2_trans.M[j]))
				(*Mat).M[i] ^= idM16[j];
		}
	}
}

// Original
// matrix multiplication 32*32 mul 32*32 -> 32*32
void opt_MatMulMatM32(M32 Mat1, M32 Mat2, M32 *Mat)
{
	int i, j;
	M32 Mat2_trans;
	opt_initM32(Mat);
	opt_MattransM32(Mat2, &Mat2_trans);
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 32; j++)
		{
			if (opt_parityU32(Mat1.M[i] & Mat2_trans.M[j]))
				(*Mat).M[i] ^= idM32[j];
		}
	}
}

// // Original
// // matrix multiplication 64*64 mul 64*64 -> 64*64
// void opt_MatMulMatM64(M64 Mat1, M64 Mat2, M64 *Mat)
// {
// 	int i, j;
// 	M64 Mat2_trans;
// 	opt_initM64(Mat);
// 	opt_MattransM64(Mat2, &Mat2_trans);
// 	for (i = 0; i < 64; i++)
// 	{
// 		for (j = 0; j < 64; j++)
// 		{
// 			if (opt_parityU64(Mat1.M[i] & Mat2_trans.M[j]))
// 				(*Mat).M[i] ^= idM64[j];
// 		}
// 	}
// }

// Optimized version
void opt_MatMulMatM64(M64 Mat1, M64 Mat2, M64 *restrict Mat)
{
	opt_initM64(Mat); // Initialize result to zero

	// Transpose Mat2 for better cache access patterns
	M64 Mat2_trans;
	opt_MattransM64(Mat2, &Mat2_trans);

	for (int i = 0; i < 64; i++)
	{
		uint64_t row_i = Mat1.M[i]; // Load current row once
		uint64_t result_i = 0;		// Accumulate results in register

		// Process 8 columns at once
		for (int j = 0; j < 64; j += 8)
		{
			// Calculate dot products
			uint64_t t0 = row_i & Mat2_trans.M[j + 0];
			uint64_t t1 = row_i & Mat2_trans.M[j + 1];
			uint64_t t2 = row_i & Mat2_trans.M[j + 2];
			uint64_t t3 = row_i & Mat2_trans.M[j + 3];
			uint64_t t4 = row_i & Mat2_trans.M[j + 4];
			uint64_t t5 = row_i & Mat2_trans.M[j + 5];
			uint64_t t6 = row_i & Mat2_trans.M[j + 6];
			uint64_t t7 = row_i & Mat2_trans.M[j + 7];

			// Convert parity to masks (branchless)
			uint64_t m0 = -(uint64_t)opt_parityU64(t0);
			uint64_t m1 = -(uint64_t)opt_parityU64(t1);
			uint64_t m2 = -(uint64_t)opt_parityU64(t2);
			uint64_t m3 = -(uint64_t)opt_parityU64(t3);
			uint64_t m4 = -(uint64_t)opt_parityU64(t4);
			uint64_t m5 = -(uint64_t)opt_parityU64(t5);
			uint64_t m6 = -(uint64_t)opt_parityU64(t6);
			uint64_t m7 = -(uint64_t)opt_parityU64(t7);

			// Apply masks and accumulate
			result_i ^= idM64[j + 0] & m0;
			result_i ^= idM64[j + 1] & m1;
			result_i ^= idM64[j + 2] & m2;
			result_i ^= idM64[j + 3] & m3;
			result_i ^= idM64[j + 4] & m4;
			result_i ^= idM64[j + 5] & m5;
			result_i ^= idM64[j + 6] & m6;
			result_i ^= idM64[j + 7] & m7;
		}

		(*Mat).M[i] = result_i; // Store the final result once
	}
}

// // Original
// // matrix multiplication 128*128 mul 128*128 -> 128*128
// void MatMulMatM128(M128 Mat1, M128 Mat2, M128 *Mat)
// {
// 	int i, j;
// 	M128 Mat2_trans;
// 	uint64_t temp[2];
// 	initM128(Mat);
// 	MattransM128(Mat2, &Mat2_trans);
// 	for (i = 0; i < 128; i++)
// 	{
// 		for (j = 0; j < 64; j++)
// 		{
// 			temp[0] = Mat1.M[i][0] & Mat2_trans.M[j][0];
// 			temp[1] = Mat1.M[i][1] & Mat2_trans.M[j][1];
// 			if (xorU128(temp))
// 				(*Mat).M[i][0] ^= idM64[j];
// 		}
// 		for (j = 64; j < 128; j++)
// 		{
// 			temp[0] = Mat1.M[i][0] & Mat2_trans.M[j][0];
// 			temp[1] = Mat1.M[i][1] & Mat2_trans.M[j][1];
// 			if (xorU128(temp))
// 				(*Mat).M[i][1] ^= idM64[j - 64];
// 		}
// 	}
// }

// Optimized version
void opt_MatMulMatM128(M128 Mat1, M128 Mat2, M128 *Mat)
{
	opt_initM128(Mat); // zero output
	M128 Mat2_trans;
	opt_MattransM128(Mat2, &Mat2_trans); // transpose for cache efficiency

	for (int i = 0; i < 128; i++)
	{
		uint64_t mi0 = Mat1.M[i][0];
		uint64_t mi1 = Mat1.M[i][1];

		// Process 4 columns per iteration
		for (int j = 0; j < 128; j += 4)
		{
			uint64_t x0 = (mi0 & Mat2_trans.M[j + 0][0]) ^ (mi1 & Mat2_trans.M[j + 0][1]);
			uint64_t x1 = (mi0 & Mat2_trans.M[j + 1][0]) ^ (mi1 & Mat2_trans.M[j + 1][1]);
			uint64_t x2 = (mi0 & Mat2_trans.M[j + 2][0]) ^ (mi1 & Mat2_trans.M[j + 2][1]);
			uint64_t x3 = (mi0 & Mat2_trans.M[j + 3][0]) ^ (mi1 & Mat2_trans.M[j + 3][1]);

			// Branchless parity mask
			uint64_t m0 = -(uint64_t)opt_parityU64(x0);
			uint64_t m1 = -(uint64_t)opt_parityU64(x1);
			uint64_t m2 = -(uint64_t)opt_parityU64(x2);
			uint64_t m3 = -(uint64_t)opt_parityU64(x3);

			// XOR into output
			Mat->M[i][(j + 0) / 64] ^= idM64[(j + 0) % 64] & m0;
			Mat->M[i][(j + 1) / 64] ^= idM64[(j + 1) % 64] & m1;
			Mat->M[i][(j + 2) / 64] ^= idM64[(j + 2) % 64] & m2;
			Mat->M[i][(j + 3) / 64] ^= idM64[(j + 3) % 64] & m3;
		}
	}
}

// // Original
// void MatMulMatM256(M256 Mat1, M256 Mat2, M256 *Mat) // matrix multiplication 256*256 mul 256*256 -> 256*256
// {
// 	int i, j;
// 	M256 Mat2_trans;
// 	uint64_t temp[4];
// 	initM256(Mat);
// 	MattransM256(Mat2, &Mat2_trans);
// 	for (i = 0; i < 256; i++)
// 	{
// 		for (j = 0; j < 64; j++)
// 		{
// 			temp[0] = Mat1.M[i][0] & Mat2_trans.M[j][0];
// 			temp[1] = Mat1.M[i][1] & Mat2_trans.M[j][1];
// 			temp[2] = Mat1.M[i][2] & Mat2_trans.M[j][2];
// 			temp[3] = Mat1.M[i][3] & Mat2_trans.M[j][3];
// 			if (xorU256(temp))
// 				(*Mat).M[i][0] ^= idM64[j];
// 		}
// 		for (j = 64; j < 128; j++)
// 		{
// 			temp[0] = Mat1.M[i][0] & Mat2_trans.M[j][0];
// 			temp[1] = Mat1.M[i][1] & Mat2_trans.M[j][1];
// 			temp[2] = Mat1.M[i][2] & Mat2_trans.M[j][2];
// 			temp[3] = Mat1.M[i][3] & Mat2_trans.M[j][3];
// 			if (xorU256(temp))
// 				(*Mat).M[i][1] ^= idM64[j - 64];
// 		}
// 		for (j = 128; j < 192; j++)
// 		{
// 			temp[0] = Mat1.M[i][0] & Mat2_trans.M[j][0];
// 			temp[1] = Mat1.M[i][1] & Mat2_trans.M[j][1];
// 			temp[2] = Mat1.M[i][2] & Mat2_trans.M[j][2];
// 			temp[3] = Mat1.M[i][3] & Mat2_trans.M[j][3];
// 			if (xorU256(temp))
// 				(*Mat).M[i][2] ^= idM64[j - 128];
// 		}
// 		for (j = 192; j < 256; j++)
// 		{
// 			temp[0] = Mat1.M[i][0] & Mat2_trans.M[j][0];
// 			temp[1] = Mat1.M[i][1] & Mat2_trans.M[j][1];
// 			temp[2] = Mat1.M[i][2] & Mat2_trans.M[j][2];
// 			temp[3] = Mat1.M[i][3] & Mat2_trans.M[j][3];
// 			if (xorU256(temp))
// 				(*Mat).M[i][3] ^= idM64[j - 192];
// 		}
// 	}
// }

// 256x256 * 256x256 -> 256x256 over GF(2)
void opt_MatMulMatM256(M256 Mat1, M256 Mat2, M256 *Mat)
{
	opt_initM256(Mat);
	M256 Mat2_trans;
	opt_MattransM256(Mat2, &Mat2_trans);

	for (int i = 0; i < 256; ++i)
	{
		// Load the 256-bit row once
		const uint64_t mi0 = Mat1.M[i][0];
		const uint64_t mi1 = Mat1.M[i][1];
		const uint64_t mi2 = Mat1.M[i][2];
		const uint64_t mi3 = Mat1.M[i][3];

		for (int j = 0; j < 256; j += 8)
		{
			// For each target column j+k, compute one 64-bit value whose parity is the dot-product over GF(2)
			// t = (mi0 & col0) ^ (mi1 & col1) ^ (mi2 & col2) ^ (mi3 & col3)
			uint64_t t0 = (mi0 & Mat2_trans.M[j + 0][0]) ^ (mi1 & Mat2_trans.M[j + 0][1]) ^ (mi2 & Mat2_trans.M[j + 0][2]) ^ (mi3 & Mat2_trans.M[j + 0][3]);
			uint64_t t1 = (mi0 & Mat2_trans.M[j + 1][0]) ^ (mi1 & Mat2_trans.M[j + 1][1]) ^ (mi2 & Mat2_trans.M[j + 1][2]) ^ (mi3 & Mat2_trans.M[j + 1][3]);
			uint64_t t2 = (mi0 & Mat2_trans.M[j + 2][0]) ^ (mi1 & Mat2_trans.M[j + 2][1]) ^ (mi2 & Mat2_trans.M[j + 2][2]) ^ (mi3 & Mat2_trans.M[j + 2][3]);
			uint64_t t3 = (mi0 & Mat2_trans.M[j + 3][0]) ^ (mi1 & Mat2_trans.M[j + 3][1]) ^ (mi2 & Mat2_trans.M[j + 3][2]) ^ (mi3 & Mat2_trans.M[j + 3][3]);
			uint64_t t4 = (mi0 & Mat2_trans.M[j + 4][0]) ^ (mi1 & Mat2_trans.M[j + 4][1]) ^ (mi2 & Mat2_trans.M[j + 4][2]) ^ (mi3 & Mat2_trans.M[j + 4][3]);
			uint64_t t5 = (mi0 & Mat2_trans.M[j + 5][0]) ^ (mi1 & Mat2_trans.M[j + 5][1]) ^ (mi2 & Mat2_trans.M[j + 5][2]) ^ (mi3 & Mat2_trans.M[j + 5][3]);
			uint64_t t6 = (mi0 & Mat2_trans.M[j + 6][0]) ^ (mi1 & Mat2_trans.M[j + 6][1]) ^ (mi2 & Mat2_trans.M[j + 6][2]) ^ (mi3 & Mat2_trans.M[j + 6][3]);
			uint64_t t7 = (mi0 & Mat2_trans.M[j + 7][0]) ^ (mi1 & Mat2_trans.M[j + 7][1]) ^ (mi2 & Mat2_trans.M[j + 7][2]) ^ (mi3 & Mat2_trans.M[j + 7][3]);

			// Parity -> 0/1 bits (no branches, encourage inlining)
			uint64_t p0 = -(uint64_t)opt_parityU64(t0);
			uint64_t p1 = -(uint64_t)opt_parityU64(t1);
			uint64_t p2 = -(uint64_t)opt_parityU64(t2);
			uint64_t p3 = -(uint64_t)opt_parityU64(t3);
			uint64_t p4 = -(uint64_t)opt_parityU64(t4);
			uint64_t p5 = -(uint64_t)opt_parityU64(t5);
			uint64_t p6 = -(uint64_t)opt_parityU64(t6);
			uint64_t p7 = -(uint64_t)opt_parityU64(t7);

			// Place result bits into correct 64-bit word & position
			Mat->M[i][(j + 0) / 64] ^= idM64[(j + 0) % 64] & p0;
			Mat->M[i][(j + 1) / 64] ^= idM64[(j + 1) % 64] & p1;
			Mat->M[i][(j + 2) / 64] ^= idM64[(j + 2) % 64] & p2;
			Mat->M[i][(j + 3) / 64] ^= idM64[(j + 3) % 64] & p3;
			Mat->M[i][(j + 4) / 64] ^= idM64[(j + 4) % 64] & p4;
			Mat->M[i][(j + 5) / 64] ^= idM64[(j + 5) % 64] & p5;
			Mat->M[i][(j + 6) / 64] ^= idM64[(j + 6) % 64] & p6;
			Mat->M[i][(j + 7) / 64] ^= idM64[(j + 7) % 64] & p7;
		}
	}
}

// mixed transformation of (previous affine inversion) and this round affine
void opt_affinemixM4(Aff4 aff, Aff4 preaff_inv, Aff4 *mixaff)
{
	opt_MatMulMatM4(aff.Mat, preaff_inv.Mat, &(mixaff->Mat));
	opt_MatMulVecM4(aff.Mat, preaff_inv.Vec, &(mixaff->Vec));
	(*mixaff).Vec.V ^= aff.Vec.V;
}

// mixed transformation of (previous affine inversion) and this round affine
void opt_affinemixM8(Aff8 aff, Aff8 preaff_inv, Aff8 *mixaff)
{
	opt_MatMulMatM8(aff.Mat, preaff_inv.Mat, &(mixaff->Mat));
	opt_MatMulVecM8(aff.Mat, preaff_inv.Vec, &(mixaff->Vec));
	(*mixaff).Vec.V ^= aff.Vec.V;
}

// mixed transformation of (previous affine inversion) and this round affine
void opt_affinemixM16(Aff16 aff, Aff16 preaff_inv, Aff16 *mixaff)
{
	opt_MatMulMatM16(aff.Mat, preaff_inv.Mat, &(mixaff->Mat));
	opt_MatMulVecM16(aff.Mat, preaff_inv.Vec, &(mixaff->Vec));
	(*mixaff).Vec.V ^= aff.Vec.V;
}

// mixed transformation of (previous affine inversion) and this round affine
void opt_affinemixM32(Aff32 aff, Aff32 preaff_inv, Aff32 *mixaff)
{
	opt_MatMulMatM32(aff.Mat, preaff_inv.Mat, &(mixaff->Mat));
	opt_MatMulVecM32(aff.Mat, preaff_inv.Vec, &(mixaff->Vec));
	(*mixaff).Vec.V ^= aff.Vec.V;
}

// mixed transformation of (previous affine inversion) and this round affine
void opt_affinemixM64(Aff64 aff, Aff64 preaff_inv, Aff64 *mixaff)
{
	opt_MatMulMatM64(aff.Mat, preaff_inv.Mat, &(mixaff->Mat));
	opt_MatMulVecM64(aff.Mat, preaff_inv.Vec, &(mixaff->Vec));
	(*mixaff).Vec.V ^= aff.Vec.V;
}

// mixed transformation of (previous affine inversion) and this round affine
void opt_affinemixM128(Aff128 aff, Aff128 preaff_inv, Aff128 *mixaff)
{
	opt_MatMulMatM128(aff.Mat, preaff_inv.Mat, &(mixaff->Mat));
	opt_MatMulVecM128(aff.Mat, preaff_inv.Vec, &(mixaff->Vec));
	(*mixaff).Vec.V[0] ^= aff.Vec.V[0];
	(*mixaff).Vec.V[1] ^= aff.Vec.V[1];
}