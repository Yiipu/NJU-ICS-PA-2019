#include "cpu/exec.h"


make_EHelper(mul) {
  rtl_imul_lo(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(mul);
}

make_EHelper(mulh) {
  rtl_imul_hi(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(mulh);
}

make_EHelper(mulhsu) {
  rtl_imul_hi(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(mulhsu);
}

make_EHelper(mulhu) {
  rtl_imul_hi(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(mulhu);
}

make_EHelper(div) {
  rtl_idiv_q(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(div);
}

make_EHelper(divu) {
  rtl_div_q(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(divu);
}

make_EHelper(rem) {
  rtl_idiv_r(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(rem);
}

make_EHelper(remu) {
  rtl_div_r(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(remu);
}
