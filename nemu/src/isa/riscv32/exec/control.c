#include "cpu/exec.h"

make_EHelper(jal) {
  rtl_li(&t0, cpu.pc + 4);
  rtl_sr(id_dest->reg, &t0, 4);

  t0 = id_src->val + cpu.pc;
  rtl_j(t0);
  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", t0);
  print_asm_template2(jal);
}

make_EHelper(jalr) {
  rtl_li(&t0, cpu.pc + 4);
  rtl_sr(id_dest->reg, &t0, 4);

  t0 = id_src->val + cpu.pc;
  rtl_j(t0);
  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", t0);
  print_asm_template2(jalr);
}

make_EHelper(beq) {
  rtl_jrelop(RELOP_EQ, &id_src->val, &id_src2->val, id_dest->addr);
  print_asm_template3(beq);
}

make_EHelper(bne) {
  rtl_jrelop(RELOP_NE, &id_src->val, &id_src2->val, id_dest->addr);
  print_asm_template3(bne);
}

make_EHelper(blt) {
  rtl_jrelop(RELOP_LT, &id_src->val, &id_src2->val, id_dest->addr);
  print_asm_template3(blt);
}

make_EHelper(bge) {
  rtl_jrelop(RELOP_GE, &id_src->val, &id_src2->val, id_dest->addr);
  print_asm_template3(bge);
}

make_EHelper(bltu) {
  rtl_jrelop(RELOP_LTU, &id_src->val, &id_src2->val, id_dest->addr);
  print_asm_template3(bltu);
}

make_EHelper(bgeu) {
  rtl_jrelop(RELOP_GEU, &id_src->val, &id_src2->val, id_dest->addr);
  print_asm_template3(bgeu);
}
