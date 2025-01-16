#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB * pcb, const char * filename) {
  Elf_Ehdr Ehdr;
  ramdisk_read(&Ehdr, 0, sizeof(Ehdr));
  for (uint16_t i = 0; i < Ehdr.e_phnum; i++) {
    Elf_Phdr Phdr;
    ramdisk_read(&Phdr, Ehdr.e_phoff + i * Ehdr.e_phentsize, sizeof(Phdr));
    if (Phdr.p_type == PT_LOAD) {
      ramdisk_read((void *)Phdr.p_vaddr, Phdr.p_offset, Phdr.p_filesz);
      memset((void *)(Phdr.p_vaddr + Phdr.p_filesz), 0, (Phdr.p_memsz - Phdr.p_filesz));
    }
  }
  return Ehdr.e_entry;
}

void naive_uload(PCB * pcb, const char * filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void (*)())entry)();
}

void context_kload(PCB * pcb, void * entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB * pcb, const char * filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
