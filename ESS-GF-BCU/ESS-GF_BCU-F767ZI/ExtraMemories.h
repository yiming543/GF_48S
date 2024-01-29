#pragma once

//E.g. void RAM2_TEXT func();
#define RAM2_TEXT __attribute__((section(".ram2_text")))

//E.g. int RAM2_DATA g_Initialized = 1;
#define RAM2_DATA __attribute__((section(".ram2_data")))

//E.g. int RAM2_BSS g_Uninitialized;
#define RAM2_BSS __attribute__((section(".ram2_bss")))


