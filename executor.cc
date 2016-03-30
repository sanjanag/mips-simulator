#include "executor.h"

Exe::Exe (Mipc *mc)
{
  _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
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
  
  while (1)
    {
      AWAIT_P_PHI0;	// @posedge

      //sampling inputs
      ins = _mc->ID_EX_ins;
      isSyscall = _mc->ID_EX_isSyscall;
      isIllegalOp = _mc->ID_EX_isIllegalOp;
      opControl = _mc->ID_EX_opControl;
      btgt = _mc->ID_EX_btgt;
      pc = _mc->ID_EX_pc;
      bd = _mc=>ID_EX_bd;
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
      
      if (!isSyscall && !isIllegalOp)
        {
          opControl(_mc,ins);
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, ins);
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
          fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, ins, _mc->_pc);
#endif
        }

      if (!isIllegalOp && !isSyscall)
        {
          if ( _mc->contents->btaken)
            {
              _mc->_npc = btgt;
              _mc->_BTAKEN = 1;
            }
        }
        
      
      
      AWAIT_P_PHI1;	// @negedge
      //transfer pipeline register contents
      EX_MEM_pc = pc;
      EX_MEM_ins = ins;
      EX_MEM_bd = bd;
      EX_MEM_decodedSRC1 = decodedSRC1;
      EX_MEM_decodedSRC2 = decodedSRC2;
      EX_MEM_decodedShiftAmt = decodedShiftAmt;
      EX_MEM_btgt = btgt;
      EX_MEM_branchOffset = branchOffset;
      EX_MEM_subregOperand  = subregOperand;
      EX_MEM_hiWPort = hiWPort;
      EX_MEM_loWPort = loWPort;
      EX_MEM_memOp = memOp;
      EX_MEM_memControl = memControl;
      EX_MEM_writeREG = writeREG;
      EX_MEM_writeFREG = writeFREG;
      EX_MEM_decodedDST = decodedDST;
      EX_MEM_isSyscall = isSyscall;
      EX_MEM_isIllegalOp = isIllegalOp;
      EX_MEM_opControl = opControl;
      EX_MEM_hi = hi;
      EX_MEM_lo = lo;
      EX_MEM_opResultHi = _mc->contents->opResultHi;
      EX_MEM_opResultLo = _mc->contents->opResultLo;
      EX_MEM_btaken = _mc->contents->btaken;
      //FETCH
      
    }
}
