#include "cpu/decode.h"
#include "rtl/rtl.h"
#include <stdint.h>

// decode operand helper
#define make_DopHelper(name) \
  void concat(decode_op_, name)(Operand * op, uint32_t val, bool load_val)

static inline make_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline make_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  if (load_val) {
    rtl_lr(&op->val, op->reg, 4);
  }

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg, 4));
}

make_DHelper(I) {
  int32_t imm = decinfo.isa.instr.simm11_0;

  // sltiu 是无符号,在这里特判
  if (decinfo.isa.instr.funct3 != 0b011) {
    imm = (imm << 20) >> 20;
  }

  decode_op_i(id_src2, imm, true);
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(U) {
  int32_t simm = decinfo.isa.instr.imm31_12 << 12;
  decode_op_i(id_src, simm, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.imm31_12);
}

make_DHelper(J) {
  int32_t simm =
      ((decinfo.isa.instr.imm19_12 << 12) | (decinfo.isa.instr.imm11_ << 11) |
       (decinfo.isa.instr.imm10_1 << 1) | (decinfo.isa.instr.simm20 << 20));
  simm = (simm << 11) >> 11;
  decode_op_i(id_src, simm, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", simm);
}

make_DHelper(R) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "%s", reg_name(id_src->reg, 4));
  print_Dop(id_src2->str, OP_STR_SIZE, "%s", reg_name(id_src2->reg, 4));
}

make_DHelper(B) {
  int32_t imm =
      (decinfo.isa.instr.simm12 << 12) | (decinfo.isa.instr.imm10_5 << 5) |
      (decinfo.isa.instr.imm4_1 << 1) | (decinfo.isa.instr.imm11 << 11);
  imm = (imm << 19) >> 19;
  imm = imm + cpu.pc;
  decode_op_i(id_dest, imm, true);
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);

  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", imm);
}

make_DHelper(ld) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.simm11_0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val,
            reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(st) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  int32_t simm = (decinfo.isa.instr.simm11_5 << 5) | decinfo.isa.instr.imm4_0;
  decode_op_i(id_src2, simm, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val,
            reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rs2, true);
}

make_DHelper(system) {
  t0 = 4;
  rtl_add(&decinfo.jmp_pc, &t0, &decinfo.isa.sepc);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
}