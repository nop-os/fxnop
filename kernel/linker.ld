OUTPUT_ARCH(sh4)
OUTPUT_FORMAT(elf32-sh)
ENTRY(_start)

MEMORY {
  rom       (rx):  o = 0x00000200, l = 523776
  addin_rom (rx):  o = 0x00300200, l = 523776
  ram       (rwx): o = 0x88000000, l = 524288
}

SECTIONS {
  . = ORIGIN(addin_rom);
  
  .entry : {
    *(.entry)
  } > addin_rom AT> rom
  
  . = (. - ORIGIN(addin_rom)) + ORIGIN(rom);
  
  .text : {
    _ld_text = .;
    
    *(.text .text.*)
  } > rom
  
  .rodata : SUBALIGN(4) {
    *(.rodata.4)
    *(.rodata .rodata.*)
  } > rom
  
  _ld_text_size = SIZEOF(.text) + SIZEOF(.rodata);
  
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
  
  _ld_size = SIZEOF(.entry) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.bss) + SIZEOF(.data) + SIZEOF(.data.4);
  
  /DISCARD/ : {
    *(.debug_info .debug_abbrev .debug_loc .debug_aranges .debug_ranges .debug_line .debug_str .debug_frame .debug_loclists .debug_rnglists)
    *(.jcr)
    *(.eh_frame_hdr)
    *(.eh_frame)
    *(.comment)
  }
}
