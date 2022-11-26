
extern CSR_state csr;

#define Supervisor_software_interrupt 1
#define Virtual_supervisor_software_interrupt 2
#define Machine_Software_Interrupt 3
#define Supervisor_timer_interrupt 5
#define Virtual_supervisor_timer_interrupt 6
#define Machine_timer_interrupt 7
#define User_Software_Interrupt 8
#define Supervisor_external_interrupt 9
#define Virtual_supervisor_external_interrupt 10
#define Machine_external_interrupt 11
#define Supervisor_guest_external_interrupt 12

rtlreg_t* csr_register(const sword_t imm) {
    switch (imm)
    {
        case 0x341:
            return &(csr.mepc);
            break;
        case 0x300:
            return &(csr.mstatus.value);
            break;
        case 0x342:
            return &(csr.mcause);
            break;
        case 0x305:
            return &(csr.mtvec);
            break;
        
        default:
            panic("error csr imm");
            break;
    }
}

def_EHelper(csrrw) {
    rtl_mv(s, s0, csr_register(id_src2->imm));
    rtl_mv(s, ddest, s0);
    rtl_mv(s, csr_register(id_src2->imm), dsrc1);
}

def_EHelper(csrrs) {
    rtl_mv(s, s0, csr_register(id_src2->imm));
    rtl_mv(s, ddest, s0);
    rtl_or(s, csr_register(id_src2->imm), dsrc1, s0);
}

def_EHelper(ecall) {
  // word_t dnpc = 0;
  
  // // if (csr.mstatus.m.MPP == 3){
  //   dnpc = isa_raise_intr(Machine_Software_Interrupt, s->pc);
  // // }else if (csr.mstatus.m.MPP == 0){
  // //   dnpc = isa_raise_intr(User_Software_Interrupt, s->pc);
  // // }else {
  // //   assert(0);
  // // }
  // rtl_li(s, &s->dnpc, &(dnpc));

  rtl_li(s, &s->dnpc, isa_raise_intr(Machine_Software_Interrupt,s->pc));
}

def_EHelper(mret) {
  //在__am_irq_handle中处理
  //rtl_addi(s, s0, &(csr.mepc), 4);
  rtl_mv(s, &s->dnpc, &(csr.mepc));

  // csr.mstatus.m.MIE = csr.mstatus.m.MPIE;
  // csr.mstatus.m.MPIE= 1;
  // if (csr.mstatus.m.MIE){
  //   IFDEF(CONFIG_ETRACE, Log("etrace: MIE = 1"));
  // }else {
  //   IFDEF(CONFIG_ETRACE, Log("etrace: MIE = 0"));
  // }
}


