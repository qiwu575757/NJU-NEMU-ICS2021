def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

// I 型算数运算指令
def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slti) {
  rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu) {
  rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori) {
  rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai) {
  rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

// 控制指令
def_EHelper(jal) {
  rtl_li(s, ddest, s->pc + 4);
  rtl_j(s, id_src1->imm + s->pc);
}

def_EHelper(jalr) { // r1 + offset = dnc
  // 尽量不使用其他寄存器
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
  rtl_jr(s, ddest);
  rtl_li(s, ddest, s->pc + 4);
}

// R 型 算数运算指令
def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl) {
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

// R 型 乘除余运算
def_EHelper(mul) { // 有符号，无符号乘法低位相同
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

// def_EHelper(mulhsu) {
//   rtl_and(s, ddest, dsrc1, dsrc2);
// }

def_EHelper(mulhu) {
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div) {
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu) {
  rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu) {
  rtl_divu_r(s, ddest, dsrc1, dsrc2);
}

// Branch Instruction
def_EHelper(beq) {
  if (interpret_relop(RELOP_EQ, *dsrc1, *dsrc2)) {
    rtl_j(s, id_dest->imm + s->pc);
  }
}

def_EHelper(bne) {
  if (interpret_relop(RELOP_NE, *dsrc1, *dsrc2)) {
    rtl_j(s, id_dest->imm + s->pc);
  }
}

def_EHelper(blt) {
  if (interpret_relop(RELOP_LT, *dsrc1, *dsrc2)) {
    rtl_j(s, id_dest->imm + s->pc);
  }
}

def_EHelper(bge) {
  if (interpret_relop(RELOP_GE, *dsrc1, *dsrc2)) {
    rtl_j(s, id_dest->imm + s->pc);
  }
}

def_EHelper(bltu) {
  if (interpret_relop(RELOP_LTU, *dsrc1, *dsrc2)) {
    rtl_j(s, id_dest->imm + s->pc);
  }
}

def_EHelper(bgeu) {
  if (interpret_relop(RELOP_GEU, *dsrc1, *dsrc2)) {
    rtl_j(s, id_dest->imm + s->pc);
  }
}
