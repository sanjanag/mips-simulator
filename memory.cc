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
        fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, _mc->_MAR_MEM, _mc->_ins);
#endif
      }
    else {
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, _mc->EX_MEM_ins);
#endif
    }
    _mc->MEM_WB_ins = mc->EX_MEM_ins;
    _mc->MEM_WB_writeREG = mc->EX_MEM_writeREG;
    _mc->MEM_WB_writeFREG = mc->EX_MEM_writeFREG;
    _mc->MEM_WB_hiWPort = mc->EX_MEM_hiWPort;
    _mc->MEM_WB_loWPort = mc->EX_MEM_loWPort;
    _mc->MEM_WB_decodedDST = mc->EX_MEM_decodedDST;
    _mc->MEM_WB_opResultHi = mc->EX_MEM_opResultHi;
    _mc->MEM_WB_opResultLo = mc->EX_MEM_opResultLo;
    _mc->MEM_WB_isSyscall = mc->EX_MEM_isSyscall;
    _mc->MEM_WB_isIllegalOp = mc->EX_MEM_isIllegalOp;
    _mc->MEM_WB_opControl = mc->EX_MEM_opControl;
    
    //         _mc->_execValid = FALSE;
    // _mc->_memValid = TRUE;
    //AWAIT_P_PHI1;       // @negedge
  }
}

