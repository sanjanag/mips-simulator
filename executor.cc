#include "executor.h"

Exe::Exe (Mipc *mc)
{
  _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
  unsigned int ins;
  Bool isSyscall, isIllegalOp;

  while (1)
    {
      AWAIT_P_PHI0;	// @posedge
      //  if (_mc->_decodeValid) {
      //        ins = _mc->_ins; //PIPELINED
      ins = _mc->ID_EX_ins;
      //        isSyscall = _mc->_isSyscall; //PIPELINED
      isSyscall = _mc->ID_EX_isSyscall;
      //         isIllegalOp = _mc->_isIllegalOp;
      isIllegalOp = _mc->ID_EX_isIllegalOp;

      //AWAIT_P_PHI1;	// @negedge
      if (!isSyscall && !isIllegalOp)
        {
          _mc->ID_EX_opControl(_mc,ins);
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
      //         _mc->_decodeValid = FALSE;
      // _mc->_execValid = TRUE;

      if (!isIllegalOp && !isSyscall)
        {
          if (/*_mc->_lastbd &&*/ _mc->EX_MEM_btaken)
            {
              _mc->_pc = _mc->ID_EX_btgt;
            }
          /*        else
                    {
                    _mc->_pc = _mc->_pc + 4;
                    }*/
          //            _mc->_lastbd = _mc->_bd;
        }
      //transfer pipeline register contents
      EX_MEM_ins = ID_EX_ins;
      EX_MEM_hiWPort = ID_EX_hiWPort;
      EX_MEM_loWPort = ID_EX_loWPort;
      EX_MEM_memOp = ID_EX_memOp;
      EX_MEM_memControl = ID_EX_memControl;
      EX_MEM_writeREG = ID_EX_writeREG;
      EX_MEM_writeFREG = ID_EX_writeFREG;
      EX_MEM_decodedDST = ID_EX_decodedDST;
      EX_MEM_isSyscall = ID_EX_isSyscall;
      EX_MEM_isIllegalOp = ID_EX_isIllegalOp;
      EX_MEM_opControl = ID_EX_opControl;
      
      AWAIT_P_PHI1;	// @negedge
      //FETCH
      
    }
}
