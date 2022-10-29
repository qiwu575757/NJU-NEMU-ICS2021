def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(jal) {
  rtl_li(s, ddest, s->pc + 4);
  rtl_j(s, id_src1->imm + s->pc);
}

def_EHelper(jalr) { // r1 + offset = dnc
  rtl_li(s, ddest, s->pc + 4);
  rtl_addi(s, s0, dsrc1, id_src2->imm);
  rtl_jr(s, s0);
}

