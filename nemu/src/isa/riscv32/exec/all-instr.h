#include "cpu/exec.h"

make_EHelper(lui);
make_EHelper(addi);
make_EHelper(auipc);
make_EHelper(add);
make_EHelper(sll);
make_EHelper(slt);
make_EHelper(sltu);
make_EHelper(xor);
make_EHelper(srl);
make_EHelper(or);
make_EHelper(and);
make_EHelper(sub);
make_EHelper(sra);
make_EHelper(slli);
make_EHelper(slti);
make_EHelper(sltiu);
make_EHelper(xori);
make_EHelper(srlai);
make_EHelper(ori);
make_EHelper(andi);

make_EHelper(mul);
make_EHelper(mulh);
make_EHelper(mulhsu);
make_EHelper(mulhu);
make_EHelper(div);
make_EHelper(divu);
make_EHelper(rem);
make_EHelper(remu);

make_EHelper(jal);
make_EHelper(jalr);
make_EHelper(beq);
make_EHelper(bne);
make_EHelper(blt);
make_EHelper(bge);
make_EHelper(bltu);
make_EHelper(bgeu);

make_EHelper(ld);
make_EHelper(st);
make_EHelper(sld);

make_EHelper(inv);
make_EHelper(nemu_trap);
