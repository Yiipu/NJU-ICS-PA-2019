#include "cpu/exec.h"

make_EHelper(jal) {
  // 保存返回地址到目标寄存器
  rtl_li(&t0, cpu.pc + 4);
  rtl_sr(id_dest->reg, &t0, 4);

  // 跳转到目标地址
  rtl_j(cpu.pc + id_src->val);
  print_asm_template2(jal);
}