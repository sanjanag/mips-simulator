#include "executor.h"

Exe::Exe (Mipc *mc)
{
  _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
  //  printf("execution stage\n");
  Bool isSyscall, isIllegalOp;
  unsigned int ins;
  void (*opControl)(Mipc*, unsigned);
  unsigned int btgt;
  unsigned int pc, bd;
  signed int decodedSRC1, decodedSRC2;
  unsigned decodedDST;
  Bool writeREG, writeFREG;
  Bool hiWPort, loWPort;
  Bool memControl;
  unsigned decodedShiftAmt;
  signed int branchOffset;
  unsigned subregOperand;
  void(*memOp)(Mipc*);
  unsigned int hi, lo;
  int NOP,BRANCH;

  while (1)
    {
      AWAIT_P_PHI0;	// @posedge

      ins = _mc->ID_EX_ins;
      isSyscall = _mc->ID_EX_isSyscall;
      isIllegalOp = _mc->ID_EX_isIllegalOp;
      opControl = _mc->ID_EX_opControl;
      btgt = _mc->ID_EX_btgt;
      pc = _mc->ID_EX_pc;
      bd = _mc->ID_EX_bd;
      decodedSRC1 = _mc->ID_EX_decodedSRC1;
      decodedSRC2 = _mc->ID_EX_decodedSRC2;
      decodedDST = _mc->ID_EX_decodedDST;
      writeREG = _mc->ID_EX_writeREG;
      writeFREG = _mc->ID_EX_writeFREG;
      hiWPort = _mc->ID_EX_hiWPort;
      loWPort = _mc->ID_EX_loWPort;
      memControl = _mc->ID_EX_memControl;
      decodedShiftAmt = _mc->ID_EX_decodedShiftAmt;
      branchOffset = _mc->ID_EX_branchOffset;
      subregOperand = _mc->ID_EX_subregOperand;
      memOp = _mc->ID_EX_memOp;
      hi = _mc->ID_EX_hi;
      lo = _mc->ID_EX_lo;
      NOP = _mc->ID_EX_NOP;
      BRANCH = _mc->ID_EX_BRANCH;

      if (!isSyscall && !isIllegalOp)
        {
          opControl(_mc,ins);
      
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Executed ins %#x opresultlo %#x\n", SIM_TIME, ins,_mc->EX_MEM_contents.opResultLo);
#endif
        }
      else if (isSyscall)
        {
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, ins);
#endif
        }
      else
        {
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, ins, pc);
#endif
        }

      if (!isIllegalOp && !isSyscall)
        {
          if ( _mc->EX_MEM_contents.btaken)
            {
              _mc->_npc = btgt;

              _mc->_BTAKEN = 1;
            }
        }
        
      
      
      AWAIT_P_PHI1;	// @negedge
      
      _mc->EX_MEM_pc = pc;
      _mc->EX_MEM_ins = ins;
      _mc->EX_MEM_bd = bd;
      _mc->EX_MEM_decodedSRC1 = decodedSRC1;
      _mc->EX_MEM_decodedSRC2 = decodedSRC2;
      _mc->EX_MEM_decodedShiftAmt = decodedShiftAmt;
      _mc->EX_MEM_btgt = btgt;
      _mc->EX_MEM_branchOffset = branchOffset;
      _mc->EX_MEM_subregOperand  = subregOperand;
      _mc->EX_MEM_hiWPort = hiWPort;
      _mc->EX_MEM_loWPort = loWPort;
      _mc->EX_MEM_memOp = memOp;
      _mc->EX_MEM_memControl = memControl;
      _mc->EX_MEM_writeREG = writeREG;
      _mc->EX_MEM_writeFREG = writeFREG;
      _mc->EX_MEM_decodedDST = decodedDST;
      _mc->EX_MEM_isSyscall = isSyscall;
      _mc->EX_MEM_isIllegalOp = isIllegalOp;
      _mc->EX_MEM_opControl = opControl;
      _mc->EX_MEM_hi = hi;
      _mc->EX_MEM_lo = lo;
      _mc->EX_MEM_opResultHi = _mc->EX_MEM_contents.opResultHi;
      _mc->EX_MEM_opResultLo = _mc->EX_MEM_contents.opResultLo;
      _mc->EX_MEM_btaken = _mc->EX_MEM_contents.btaken;
      _mc->EX_MEM_contents.opResultLo = 0;
      _mc->EX_MEM_contents.opResultHi = 0;
      _mc->EX_MEM_contents.btaken = 0;
      _mc->EX_MEM_NOP = NOP;
      _mc->EX_MEM_BRANCH = BRANCH;
      //FETCH
      
    }
}
