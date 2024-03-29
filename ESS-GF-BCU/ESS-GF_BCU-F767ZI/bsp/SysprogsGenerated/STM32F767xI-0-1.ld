/* Linker script to configure memory regions. */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  CLASSBRAM (rw)   : ORIGIN = 0x20000000, LENGTH = 0x80
  RAM (xrw)        : ORIGIN = 0x20000080, LENGTH = 0x4FF80  
 
}
/* Linker script to place sections and symbol values. Should be used together

 * with other linker script that defines memory regions FLASH and RAM.

 * It references following symbols, which must be defined in code:

 *   Reset_Handler : Entry of reset handler

 * 

 * It defines following symbols, which code can use without definition:

 *   __exidx_start

 *   __exidx_end

 *   __etext

 *   __data_start__

 *   __preinit_array_start

 *   __preinit_array_end

 *   __init_array_start

 *   __init_array_end

 *   __fini_array_start

 *   __fini_array_end

 *   __data_end__

 *   __bss_start__

 *   __bss_end__

 *   __end__

 *   end

 *   __HeapLimit

 *   __StackLimit

 *   __StackTop

 *   __stack

 *   _estack

 */
ENTRY(Reset_Handler)
SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > FLASH
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

	/* check sum specified by user at the end of program in flash */
  .check_sum :
  {
  	. = ALIGN(64); /* alignement must correspond to the size of tested block at FLASH run time test */
  	PROVIDE( _Check_Sum = . );
  	/* check sum defined by user - see user manual */
  	LONG(0x849ACEF9);
  } >FLASH
  
    __exidx_end = .;
    __etext = .;
    _sidata = .;
    .data : AT (__etext)
    {
        __data_start__ = .;
        _sdata = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
        _edata = .;
    } > RAM
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = .;
    } > RAM
    .heap (COPY):
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > RAM
    /* .stack_dummy section doesn't contains any symbols. It is only

     * used for linker to calculate size of stack sections, and assign

     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM
	
	
    /* Set stack top to end of RAM, and stack limit move down by

     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    _estack = __StackTop;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
	
	  /* User class B RAM region */
  ._user_classb(NOLOAD) :
  {
    . = ALIGN(4);
    *(.run_time_ram_buf)
    *(.run_time_ram_buf*)
    *(.run_time_ram_pnt)
    *(.run_time_ram_pnt*)
    
    . = ALIGN(4);
    PROVIDE(_classb_start = . );
    
    *(.class_b_ram)
    *(.class_b_ram*)
    . = ALIGN(4);
    *(.class_b_ram_rev)
    *(.class_b_ram_rev*)
    
    . = ALIGN(4);
    PROVIDE(_classb_end = . );
  } >CLASSBRAM	
}
