#include "cpu/exec.h"
#include "all-instr.h"

// clang-format off

/*
000'b ADDI: Add Immediate
001'b SLLI: Shift Left Logical Immediate
010'b SLTI: Set Less Than Immediate
011'b SLTIU: Set Less Than Immediate Unsigned
100'b XORI: XOR Immediate
101'b SRLI: Shift Right Logical Immediate
110'b ORI: OR Immediate
111'b ANDI: AND Immediate
*/
static OpcodeEntry imm_table [8] = {
  EXW(addi, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(imm) {
  decinfo.width = imm_table[decinfo.isa.instr.funct3].width;
  idex(pc, &imm_table[decinfo.isa.instr.funct3]);
}

/*
000'b LB: Load Byte
001'b LH: Load Half
010'b LW: Load Word
011'b LD: RV64 Load Double
100'b LBU: Load Byte Unsigned
101'b LHU: Load Half Unsigned
110'b LWU: RV64 Load Word Unsigned
111'b Reserved
*/
static OpcodeEntry load_table [8] = {
  EXW(ld, 1), EXW(ld, 2), EXW(ld, 4), EMPTY, EXW(ld, 2), EXW(ld, 1), EXW(ld, 4), EMPTY
};

// make_EHelper(load) 展开为 void exec_load(vaddr_t *pc)
static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

/*
000'b SB: Store Byte
001'b SH: Store Half
010'b SW: Store Word
011'b SD: RV64 Store Double
*/
static OpcodeEntry store_table [8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY, EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY
};

// static void exec_store(vaddr_t *pc)
static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEXW(I, imm, 0), EMPTY, EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, EMPTY, IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ EMPTY, EMPTY, EX(nemu_trap), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
};
// clang-format on

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  // 不支持压缩指令，所有 opcode 低两位必须为 0b11
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  // 打印 val opcode6_2 funct3
  printf("val: 0x%08x, opcode6_2: 0x%02x, funct3: 0x%02x\n", decinfo.isa.instr.val, decinfo.isa.instr.opcode6_2, decinfo.isa.instr.funct3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}
