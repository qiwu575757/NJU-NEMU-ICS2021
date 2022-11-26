#include <proc.h>
#include <elf.h>
#include <klib.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
  
  // ramdisk_read(&ehdr, 0, sizeof(ehdr));
  int fd = fs_open(filename, 0, 0);
  assert(fd != -1);

  fs_read(fd, &ehdr, sizeof(ehdr));

  char riscv32_magic_num[] = {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
  // printf("magic number is %s\n", (char *)(ehdr.e_ident));
  assert(strcmp((char *)(ehdr.e_ident), riscv32_magic_num)== 0);

  uint32_t entry = ehdr.e_entry;
  uint32_t ph_offset = ehdr.e_phoff;
  uint32_t ph_num = ehdr.e_phnum;

  Elf_Phdr phdr;
  for(int i = 0; i < ph_num; ++i){
    ramdisk_read(&phdr, ph_offset + i * sizeof(phdr), sizeof(phdr));
    fs_lseek(fd, ph_offset + i * sizeof(phdr), SEEK_SET);
    fs_read(fd, &phdr, sizeof(phdr));
    if(phdr.p_type != PT_LOAD)continue;

    // printf("load program header %d", i);
    
    uint32_t offset = phdr.p_offset;
    uint32_t file_size = phdr.p_filesz;
    uint32_t vaddr = phdr.p_vaddr;
    uint32_t mem_size = phdr.p_memsz;

    // printf("load program from [%p, %p] to [%p, %p]\n", offset, file_size, vaddr, mem_size);

    // ramdisk_read((void *)vaddr, offset, file_size);
    fs_lseek(fd, offset, SEEK_SET);
    fs_read(fd, (void *)vaddr, file_size);
    
    assert(mem_size >= file_size);
    memset((void*)(vaddr + file_size), 0, mem_size - file_size);
  }

  assert(fs_close(fd) != -1);
  
  return entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

int ptr_len(char *const argv[]) {
  int len = 0;
  while (argv[len] != NULL) {
    len++;
  }

  return len;
}

Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg)
{
  Area kstack = RANGE(pcb, (char *)pcb + STACK_SIZE);
  
  return kcontext(kstack, entry, arg);
}

Context *context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  Area kstack = RANGE(pcb, (char *)pcb + STACK_SIZE);

  // reverse fill the ustack with argc, argv, envp, string...
  int argc = (argv == NULL) ? 0 : ptr_len(argv);
  int envc = (envp == NULL) ? 0 : ptr_len(envp);

  // malloc temp argv 
  char *args[argc];
  char *envs[envc];

  // get stack pointer
  char *sp = (char *)new_page(8) + 8 * PGSIZE;

  // fill the argc string to string segment
  for (int i = 0; i < argc; i++) {
    sp -= (strlen(argv[i]) + 1);
    strcpy(sp, argv[i]);
    args[i] = sp;
  }

  // fill the envp string to string segment
  for (int i = 0; i < envc; i++) {
    sp -= (strlen(envp[i]) + 1);
    strcpy(sp, envp[i]);
    envs[i] = sp;
  }

  // fill the point to string in stack
  char **spp = (char **)sp;

  spp--;
  *spp = NULL; 
  for (int i = envc-1; i >= 0; i--) {
    spp--;
    *spp = envs[i];
  }

  spp--;
  *spp = NULL; 
  for (int i = argc-1; i >= 0; i--) {
    spp--;
    *spp = args[i];
  }

  spp--;
  *((int *)spp) = argc;

  uintptr_t entry = loader(pcb, filename);
  Context *ctx = ucontext(NULL, kstack, (void *)entry);
  // ctx->GPRx = (uintptr_t)spp;

  return ctx;
}