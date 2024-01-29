#include "ExtraMemories.h"

void __attribute__((constructor)) RAM2_Init()
{
	extern void *_siram2_text, *_sram2_text, *_eram2_text;
	extern void *_siram2_data, *_sram2_data, *_eram2_data;
	extern void *_sram2_bss, *_eram2_bss;

	void **pSource, **pDest;
	for (pSource = &_siram2_text, pDest = &_sram2_text; pDest != &_eram2_text; pSource++, pDest++)
		*pDest = *pSource;

	for (pSource = &_siram2_data, pDest = &_sram2_data; pDest != &_eram2_data; pSource++, pDest++)
		*pDest = *pSource;

	for (pDest = &_sram2_bss; pDest != &_eram2_bss; pDest++)
		*pDest = 0;

}

