#include "cpu/exec.h"
#include <stdint.h>

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(addi) {
  rtl_addi(&id_dest->reg, &id_src2->imm, id_src->val);
  rtl_sr(id_dest->reg, &id_dest->reg, 4);

  print_asm_template3(addi);
}

make_EHelper(auipc) {

  // int32_t pc_hi = decinfo.seq_pc & 0xfffff000;
  int32_t dest = decinfo.seq_pc + id_src->imm;

  printf("saving %x to %d\n", dest, id_dest->reg);

  rtl_addi(&id_dest->reg, 0, dest);
  rtl_sr(id_dest->reg, &id_dest->reg, 4);
  print_asm_template2(auipc);
}
