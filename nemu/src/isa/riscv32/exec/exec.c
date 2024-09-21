#include "cpu/exec.h"
#include "all-instr.h"


// clang-format off
/*
branch
000'b BEQ: Branch Equal
001'b BNE: Branch Not Equal
100'b BLT: Branch Less Than
101'b BGE: Branch Greater Than or Equal
110'b BLTU: Branch Less Than Unsigned
111'b BGEU: Branch Greater Than or Equal Unsigned
*/
static OpcodeEntry branch_table [8] = {
  EX(beq), EX(bne), EMPTY, EMPTY, EX(blt), EX(bge), EX(bltu), EX(bgeu)
};
// clang-format on

static make_EHelper(branch) {
  idex(pc, &branch_table[decinfo.isa.instr.funct3]);
}

// clang-format off
/*
funct7        funct3
0000000'b ->  000'b ADD
              001'b SLL
              010'b SLT
              011'b SLTU
              100'b XOR
              101'b SRL
              110'b OR
              111'b AND
0000001'b ->  000'b MUL
              001'b MULH
              010'b MULHSU
              011'b MULHU
              100'b DIV
              101'b DIVU
              110'b REM
              111'b REMU
0100000'b ->  000'b SUB
              101'b SRA
*/
static OpcodeEntry reg_table [24] = {
  /* 0000000'b */ EX(add), EX(sll),  EX(slt),    EX(sltu),  EX(xor), EX(srl),  EX(or), EX(and),
  /* 0000001'b */ EX(mul), EX(mulh), EX(mulhsu), EX(mulhu), EX(div), EX(divu), EX(rem),EX(remu),
  /* 0100000'b */ EX(sub), EMPTY,    EMPTY,      EMPTY,     EMPTY,   EX(sra),  EMPTY,  EMPTY
};
// clang-format on

static make_EHelper(reg) {
  switch (decinfo.isa.instr.funct7) {
  case 0x0000000:
    idex(pc, reg_table + decinfo.isa.instr.funct3);
    break;
  case 0x0000001:
    idex(pc, reg_table + 8 + decinfo.isa.instr.funct3);
    break;
  case 0x0100000:
    idex(pc, reg_table + 16 + decinfo.isa.instr.funct3);
    break;
  }
}

// clang-format off
/*
000'b ADDI: Add Immediate
001'b SLLI: Shift Left Logical Immediate
010'b SLTI: Set Less Than Immediate
011'b SLTIU: Set Less Than Immediate Unsigned
100'b XORI: XOR Immediate
101'b SRLI/SRAI: Shift Right Logical/Arithmetic Immediate
110'b ORI: OR Immediate
111'b ANDI: AND Immediate
*/
static OpcodeEntry imm_table[8] = {
  EX(addi), EX(slli), EX(slti), EX(sltiu), EX(xori), EX(srlai), EX(ori), EX(andi)
};
// clang-format on

static make_EHelper(imm) { idex(pc, &imm_table[decinfo.isa.instr.funct3]); }

// clang-format off
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
// clang-format on

// make_EHelper(load) 展开为 void exec_load(vaddr_t *pc)
static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

// clang-format off
/*
000'b SB: Store Byte
001'b SH: Store Half
010'b SW: Store Word
011'b SD: RV64 Store Double
*/
static OpcodeEntry store_table [8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY, EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY
};
// clang-format on

// static void exec_store(vaddr_t *pc)
static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

// clang-format off
static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, imm), IDEX(U, auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, reg), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, branch), IDEX(I, jalr), EX(nemu_trap), IDEX(J, jal), EMPTY, EMPTY, EMPTY, EMPTY,
};
// clang-format on

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  // 不支持压缩指令，所有 opcode 低两位必须为 0b11
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}
