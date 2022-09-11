OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80000000;

  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trampoline == 0x1000, "error: trampoline larger than one page");
    PROVIDE(etext = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
  }
   .vdso : { /* Begin een *output* sectie genaamd .vdso */
   . = ALIGN(0x1000); /* Align het huidige adres (.) op een page boundary */
   _vdso_start = .; /* Maak een nieuw symbol _vdso_start met het huidige adres */
   *(.vdso); /* Plaats alle *input* secties genaamd .vdso in de huidige output sectie  */
   . = ALIGN(0x1000); /* Align het huidige adres op een page boundary */
   ASSERT(. - _vdso_start == 0x1000, "error: vdso larger than one page");
 }
  PROVIDE(end = .);
}
