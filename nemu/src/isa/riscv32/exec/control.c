#include "cpu/exec.h"

make_EHelper(jal){
    rtl_j(decinfo.seq_pc + id_src->val);
    rtl_add(&id_dest->val, &decinfo.seq_pc, &id_src->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    
    print_asm_template2(jal);
}