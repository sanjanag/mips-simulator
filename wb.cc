#include "wb.h"

Writeback::Writeback (Mipc *mc)
{
  _mc = mc;
}

Writeback::~Writeback (void) {}

void
Writeback::MainLoop (void)
{
  unsigned int ins;
  Bool writeReg;
  Bool writeFReg;
  Bool loWPort;
  Bool hiWPort;
  Bool isSyscall;
  Bool isIllegalOp;
  unsigned decodedDST;
  unsigned opResultLo, opResultHi;

  while (1)
    {
      AWAIT_P_PHI0;	// @posedge
      // Sample the important signals
      //      if (_mc->_memValid) {
      writeReg = _mc->MEM_WB_writeREG;
      writeFReg = _mc->MEM_WB_writeFREG;
      loWPort = _mc->MEM_WB_loWPort;
      hiWPort = _mc->MEM_WB_hiWPort;
      decodedDST = _mc->MEM_WB_decodedDST;
      opResultLo = _mc->MEM_WB_opResultLo;
      opResultHi = _mc->MEM_WB_opResultHi;
      isSyscall = _mc->MEM_WB_isSyscall;
      isIllegalOp = _mc->MEM_WB_isIllegalOp;
      ins = _mc->MEM_WB_ins;
      AWAIT_P_PHI1;       // @negedge
      if (isSyscall)
        {
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> SYSCALL! Trapping to emulation layer at PC %#x\n", SIM_TIME, _mc->_pc);
#endif      
          _mc->MEM_WB_opControl(_mc, ins);
          _mc->_pc += 4;
        }
      else if (isIllegalOp)
        {
          printf("Illegal ins %#x at PC %#x. Terminating simulation!\n", ins, _mc->_pc);
#ifdef MIPC_DEBUG
          fclose(_mc->_debugLog);
#endif
          printf("Register state on termination:\n\n");
          _mc->dumpregs();
          exit(0);
        }
      else {
        if (writeReg)
          {
            _mc->_gpr[decodedDST] = opResultLo;
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Writing to reg %u, value: %#x\n", SIM_TIME, decodedDST, opResultLo);
#endif
          }
        else if (writeFReg)
          {
            _mc->_fpr[(decodedDST)>>1].l[FP_TWIDDLE^((decodedDST)&1)] = opResultLo;
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Writing to freg %u, value: %#x\n", SIM_TIME, decodedDST>>1, opResultLo);
#endif
          }
        else if (loWPort || hiWPort)
          {
            if (loWPort) {
              _mc->_lo = opResultLo;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Writing to Lo, value: %#x\n", SIM_TIME, opResultLo);
#endif
            }
            if (hiWPort)
              {
                _mc->_hi = opResultHi;
#ifdef MIPC_DEBUG
                fprintf(_mc->_debugLog, "<%llu> Writing to Hi, value: %#x\n", SIM_TIME, opResultHi);
#endif
              }
          }
      }
      _mc->_gpr[0] = 0;
      //         _mc->_memValid = FALSE;
      //         _mc->_insDone = TRUE;
      
    }
}
