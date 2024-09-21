#include "cpu/exec.h"
#include "all-instr.h"

// clang-format off

/*
000'b LB: Load Byte
001'b LH: Load Half
010'b LW: Load Word
011'b LD: RV64 Load Double
100'b LBU: Load Byte Unsigned
101'b LHU: Load Half Unsigned
110'b LWU: RV64 Load Word Unsigned
*/
// static OpcodeEntry load_table [8] = {
//   EXW(lb,1), EXW(lh,2), EXW(lw, 4), EMPTY, EXW(lbu, 1), EXW(lhu, 2), EMPTY, EMPTY
// };
static OpcodeEntry load_table [8] = {
  EMPTY, EMPTY, EXW(ld, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

// make_EHelper(load) 展开为 void exec_load(vaddr_t *pc)
static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry store_table [8] = {
  EMPTY, EMPTY, EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

// static void exec_store(vaddr_t *pc)
static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, EMPTY, IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ EMPTY, EMPTY, EX(nemu_trap), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
};
// clang-format on

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  // 不支持压缩指令，所有 opcode 低两位必须为 0b11
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  // 打印指令
  print_asm_template1(decinfo.isa.instr);
  // 打印 val opcode6_2 funct3
  printf("val: 0x%08x, opcode6_2: 0x%02x, funct3: 0x%02x\n", decinfo.isa.instr.val, decinfo.isa.instr.opcode6_2, decinfo.isa.instr.funct3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}
