#include "memory.h"

Memory::Memory (Mipc *mc)
{
  _mc = mc;
}

Memory::~Memory (void) {}

void
Memory::MainLoop (void)
{
  unsigned int pc, ins,bd;
  Bool isSyscall, isIllegalOp;
  signed int decodedSRC1, decodedSRC2;
  unsigned decodedDST;
  Bool writeREG, writeFREG;
  Bool memControl;
  void (*opControl)(Mipc*, unsigned);
  unsigned decodedShiftAmt;
  unsigned int btgt;
  signed int branchOffset;
  unsigned subregOperand;
  void (*memOp)(Mipc*);
  Bool hiWPort, loWPort;
  unsigned opResultHi, opResultLo;
  int btaken;
  unsigned int hi,lo;

  while (1) {

    AWAIT_P_PHI0;	// @posedge
    //sampling inputs
    pc = _mc->EX_MEM_pc;
    ins = _mc->EX_MEM_ins;
    bd = _mc->EX_MEM_bd;
    isSyscall = _mc->EX_MEM_isSyscall;
    isIllegalOp = _mc->EX_MEM_isIllegalOp;
    decodedSRC1 = _mc->EX_MEM_decodedSRC1;
    decodedSRC2 = _mc->EX_MEM_decodedSRC2;
    decodedDST = _mc->EX_MEM_decodedDST;
    writeREG = _mc->EX_MEM_writeREG;
    writeFREG = _mc->EX_MEM_writeFREG;
    memControl = _mc->EX_MEM_memControl;
    opControl = _mc->EX_MEM_opControl;
    decodedShiftAmt = _mc->EX_MEM_decodedShiftAmt;
    btgt = _mc->EX_MEM_btgt;
    branchOffset = _mc->EX_MEM_branchOffset;
    subregOperand = _mc->EX_MEM_subregOperand;
    memOp = _mc->EX_MEM_memOp;
    hiWPort = _mc->EX_MEM_hiWPort;
    loWPort = _mc->EX_MEM_loWPort;
    opResultLo = _mc->EX_MEM_opResultLo;
    opResultHi = _mc->EX_MEM_opResultHi;
    btaken = _mc->EX_MEM_btaken;
    hi = _mc->EX_MEM_hi;
    lo = _mc->EX_MEM_lo;
    _mc->tempsubregOperand = _mc->EX_MEM_subregOperand;
    _mc->tempdecodedDST = _mc->EX_MEM_decodedDST;
    
    AWAIT_P_PHI1;       // @negedge

    if (memControl)
      {
        memOp (_mc);
#ifdef MIPC_DEBUG
        fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, _mc->_MAR_MEM, ins);
#endif
      }
    else {
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, ins);
#endif
    }

    //update registers
    _mc->MEM_WB_pc = pc;
    _mc->MEM_WB_ins = ins;
    _mc->MEM_WB_bd = bd;
    _mc->MEM_WB_isSyscall = isSyscall;
    _mc->MEM_WB_isIllegalOp = isIllegalOp;
    _mc->MEM_WB_decodedSRC1 = decodedSRC1;
    _mc->MEM_WB_decodedSRC2 = decodedSRC2;
    _mc->MEM_WB_decodedDST = decodedDST;
    _mc->MEM_WB_writeREG = writeREG;
    _mc->MEM_WB_writeFREG = writeFREG;
    _mc->MEM_WB_memControl = memControl;
    _mc->MEM_WB_opControl = opControl;
    _mc->MEM_WB_decodedShiftAmt = decodedShiftAmt;
    _mc->MEM_WB_btgt = btgt;
    _mc->MEM_WB_branchOffset = branchOffset;
    _mc->MEM_WB_subregOperand = subregOperand;
    _mc->MEM_WB_memOp = memOp;
    _mc->MEM_WB_hiWPort = hiWPort;
    _mc->MEM_WB_loWPort = loWPort;
    _mc->MEM_WB_opResultHi = opResultHi;
    _mc->MEM_WB_btaken = btaken;
    _mc->MEM_WB_hi = hi;
    _mc->MEM_WB_lo = lo;
  }
}

