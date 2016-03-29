#include "memory.h"

Memory::Memory (Mipc *mc)
{
  _mc = mc;
}

Memory::~Memory (void) {}

void
Memory::MainLoop (void)
{
  Bool memControl;

  while (1) {
    AWAIT_P_PHI0;	// @posedge
    //      if (_mc->_execValid) {

    memControl = _mc->EX_MEM_memControl;
    
    AWAIT_P_PHI1;       // @negedge

    if (memControl)
      {
        _mc->EX_MEM_memOp (_mc);
#ifdef MIPC_DEBUG
        fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, _mc->_MAR, _mc->_ins);
#endif
      }
    else {
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, _mc->_ins);
#endif
    }
    MEM_WB_ins = EX_MEM_ins;
    MEM_WB_writeREG = EX_MEM_writeREG;
    MEM_WB_writeFREG = EX_MEM_writeFREG;
    MEM_WB_hiWPort = EX_MEM_hiWPort;
    MEM_WB_loWPort = EX_MEM_loWPort;
    MEM_WB_decodedDST = EX_MEM_decodedDST;
    MEM_WB_opResultHi = EX_MEM_opResultHi;
    MEM_WB_opResultLo = EX_MEM_opResultLo;
    MEM_WB_isSyscall = EX_MEM_isSyscall;
    MEM_WB_isIllegalOp = EX_MEM_isIllegalOp;
    MEM_WB_opControl = EX_MEM_opControl;
    
    //         _mc->_execValid = FALSE;
    // _mc->_memValid = TRUE;
    //AWAIT_P_PHI1;       // @negedge
  }
}

