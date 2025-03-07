#ifndef __RISCV32_DECODE_H__
#define __RISCV32_DECODE_H__

typedef union {
  // R-type
  struct {
    uint32_t opcode1_0 : 2;
    uint32_t opcode6_2 : 5;
    uint32_t rd        : 5;
    uint32_t funct3    : 3;
    uint32_t rs1       : 5;
    uint32_t rs2       : 5;
    uint32_t funct7    : 7;
  };
  // I-type，执行时处理 shamt
  struct {
    // opcode rd funct3 rs1
    uint32_t pad0      :20;
    int32_t  simm11_0  :12;
  };
  // S-type
  struct {
    // opcode
    uint32_t pad1      : 7;
    uint32_t imm4_0    : 5;
    // funct3 rs1 rs2
    uint32_t pad2      :13;
    int32_t  simm11_5  : 7;
  };
  // B-type
  struct {
    // opcode
    uint32_t pad3      : 7;
    uint32_t imm11     : 1;
    uint32_t imm4_1    : 4;
    // funct3 rs1 rs2
    uint32_t pad4      :13;
    uint32_t imm10_5   : 6;
    int32_t  simm12    : 1;
  };
  // U-type
  struct {
    // opcode rd
    uint32_t pad5      :12;
    uint32_t imm31_12  :20;
  };
  // J-type
  struct {
    // opcode rd
    uint32_t pad6      :12;
    uint32_t imm19_12  : 8;
    uint32_t imm11_    : 1;
    uint32_t imm10_1   :10;
    int32_t  simm20    : 1;
  };
  // CSR-type
  struct {
    // opcode rd func3 rs1
    uint32_t pad7      :20;
    uint32_t csr       :12;
  };
  uint32_t val;
} Instr;


struct ISADecodeInfo {
  Instr instr;
  // CSR 寄存器
  uint32_t sepc, sstatus, scause, stvec;
};

make_DHelper(I);
make_DHelper(S);
make_DHelper(U);
make_DHelper(J);
make_DHelper(R);
make_DHelper(B);
make_DHelper(ld);
make_DHelper(st);
make_DHelper(system);

#endif
