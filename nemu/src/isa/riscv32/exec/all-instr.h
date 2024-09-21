#include "cpu/exec.h"

make_EHelper(lui);

make_EHelper(ld); // ?? LD 是 RISCV64 的指令，为什么在 RISCV32 中实现了呢？
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);
