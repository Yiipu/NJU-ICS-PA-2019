#include "cpu/exec.h"

make_EHelper(ld) {
  rtl_lm(&s0, &id_src->addr, decinfo.width);
  rtl_sr(id_dest->reg, &s0, 4);

  switch (decinfo.width) {
    case 4: print_asm_template2(lw); break;
    case 2: print_asm_template2(lhu); break;
    case 1: print_asm_template2(lbu); break;
    default: assert(0);
  }
}

make_EHelper(sld){
  rtl_lm(&s0, &id_src->addr, decinfo.width);

  switch (decinfo.width) {
    case 2: {
      rtl_shli(&s0, &s0, 16);
      rtl_sari(&s0, &s0, 16);
      print_asm_template2(lh);
      break;
    }
    case 1: {
            rtl_shli(&s0, &s0, 8);
      rtl_sari(&s0, &s0, 8);
      print_asm_template2(lb); 
      break;
    }
    default: assert(0);
  }

  rtl_sr(id_dest->reg, &s0, 4);
}

make_EHelper(st) {
  rtl_sm(&id_src->addr, &id_dest->val, decinfo.width);

  switch (decinfo.width) {
    case 4: print_asm_template2(sw); break;
    case 2: print_asm_template2(sh); break;
    case 1: print_asm_template2(sb); break;
    default: assert(0);
  }
}
