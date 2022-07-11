OUTPUT_ARCH(sh4)
OUTPUT_FORMAT(elf32-sh)
ENTRY(_start)

MEMORY {
  rom (rx):  o = 0x00300200, l = 523776
  ram (rwx): o = 0x08100000, l = 30720
}

SECTIONS {
  . = ORIGIN(rom);
  
  .text : {
    *(.entry)
    *(.ctors .ctors.*)
    *(.dtors .dtors.*)
    *(.text .text.*)
  } > rom
  
  .rodata : SUBALIGN(4) {
    *(.rodata.4)
    *(.rodata .rodata.*)
  } > rom
  
  . = ORIGIN(ram);
  
  .bss (NOLOAD) : {
    _ld_bss = .;
    
    *(.bss COMMON)
    . = ALIGN(16);
  } > ram :NONE
  
  _ld_bss_size = SIZEOF(.bss);
  
  .data ALIGN(4) : ALIGN(4) {
    _ld_data_load = LOADADDR(.data);
    _ld_data = .;
    
    *(.data .data.*)
    . = ALIGN(16);
  } > ram AT> rom
  
  .data.4 : SUBALIGN(4) {
    *(.data.4)
    . = ALIGN(16);
  } > ram AT> rom
  
  _ld_data_size = SIZEOF(.data) + SIZEOF(.data.4);
  
  /DISCARD/ : {
    *(.debug_info .debug_abbrev .debug_loc .debug_aranges .debug_ranges .debug_line .debug_str .debug_frame .debug_loclists .debug_rnglists)
    *(.jcr)
    *(.eh_frame_hdr)
    *(.eh_frame)
    *(.comment)
  }
}
