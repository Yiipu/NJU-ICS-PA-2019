#include "cpu/decode.h"
#include "cpu/exec.h"
#include <stdint.h>

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(addi) {
  rtl_addi(&t0, &id_src->val, id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(addi);
}

make_EHelper(auipc) {
  rtl_addi(&t0, &cpu.pc, id_src->imm);
  rtl_sr(id_dest->reg, &t0, 4);
  print_asm_template2(auipc);
}

make_EHelper(add) {
  rtl_add(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(add);
}

make_EHelper(sll) {
  rtl_shl(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(sll);
}

make_EHelper(slt) {
  rtl_setrelop(RELOP_LT, &t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(slt);
}

make_EHelper(sltu) {
  rtl_setrelop(RELOP_LTU, &t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(sltu);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(xor);
}

make_EHelper(srl) {
  rtl_shr(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(srl);
}

make_EHelper(or) {
  rtl_or(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(or);
}

make_EHelper(and) {
  rtl_and(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(and);
}

make_EHelper(sub) {
  rtl_sub(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(sub);
}

make_EHelper(sra) {
  rtl_sar(&t0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(sra);
}

make_EHelper(slli) {
  rtl_shl(&t0, &id_src->val, &id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(slli);
}

make_EHelper(slti) {
  rtl_setrelopi(RELOP_LT, &t0, &id_src->val, id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(slti);
}

make_EHelper(sltiu) {
  rtl_setrelopi(RELOP_LTU, &t0, &id_src->val, id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(sltiu);
}

make_EHelper(xori) {
  rtl_xor(&t0, &id_src->val, &id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(xori);
}

make_EHelper(srlai) {
  bool is_alg = decinfo.src2.imm >> 5;
  id_src2->imm &= 0x1f;
  if (is_alg) {
    rtl_sar(&t0, &id_src->val, &id_src2->imm);
    print_asm_template3(srai);
  } else {
    rtl_shr(&t0, &id_src->val, &id_src2->imm);
    print_asm_template3(srli);
  }
  rtl_sr(id_dest->reg, &t0, 4);
}

make_EHelper(ori) {
  rtl_or(&t0, &id_src->val, &id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(ori);
}

make_EHelper(andi) {
  rtl_and(&t0, &id_src->val, &id_src2->imm);
  rtl_sr(id_dest->reg, &t0, 4);

  print_asm_template3(andi);
}
