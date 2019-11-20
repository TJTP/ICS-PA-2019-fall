#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();


static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  //return 0;
  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr,0,sizeof(Elf_Ehdr));

  Elf_Phdr pht[ehdr.e_phnum];
  ramdisk_read(pht, ehdr.e_ehsize, sizeof(Elf_Phdr)*ehdr.e_phnum);
  for(int i = 0; i<ehdr.e_phnum; i++){
    if(pht[i].p_type == PT_LOAD){
      ramdisk_read((void*)pht[i].p_vaddr, pht[i].p_offset, pht[i].p_memsz);
      memset((void*)(pht[i].p_vaddr+pht[i].p_filesz), 0, pht[i].p_memsz-pht[i].p_filesz);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
