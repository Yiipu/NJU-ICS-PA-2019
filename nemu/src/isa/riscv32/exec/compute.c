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
  rtl_addi(&id_dest->reg, &cpu.pc, id_src->imm);
  rtl_sr(id_dest->reg, &id_dest->reg, 4);
  print_asm_template2(auipc);
}
