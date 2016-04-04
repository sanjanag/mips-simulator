#include "wb.h"
#include "app_syscall.h"
Writeback::Writeback (Mipc *mc)
{
  _mc = mc;
}

Writeback::~Writeback (void) {}

void
Writeback::MainLoop (void)
{
  unsigned int ins,pc;
  Bool writeREG;
  Bool writeFREG;
  Bool loWPort;
  Bool hiWPort;
  Bool isSyscall;
  Bool isIllegalOp;
  unsigned decodedDST;
  unsigned opResultLo, opResultHi;
  unsigned int hi,lo;
  int count=0;
  int NOP;
  while (1)
    {
      AWAIT_P_PHI0;	// @posedge

      writeREG = _mc->MEM_WB_writeREG;
      writeFREG = _mc->MEM_WB_writeFREG;
      loWPort = _mc->MEM_WB_loWPort;
      hiWPort = _mc->MEM_WB_hiWPort;
      decodedDST = _mc->MEM_WB_decodedDST;
      opResultLo = _mc->MEM_WB_opResultLo;


      opResultHi = _mc->MEM_WB_opResultHi;
      isSyscall = _mc->MEM_WB_isSyscall;
      isIllegalOp = _mc->MEM_WB_isIllegalOp;
      ins = _mc->MEM_WB_ins;
      pc = _mc->MEM_WB_pc;
      NOP = _mc->MEM_WB_NOP;

      if (isSyscall)
        {
          
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> SYSCALL! Trapping to emulation layer at PC %#x\n", SIM_TIME, pc);
#endif
          _mc->dumpregs(pc);
          _mc->MEM_WB_opControl(_mc, ins);
        }
      else if (isIllegalOp)
        {
          printf("Illegal ins %#x at PC %#x. Terminating simulation!\n", ins, pc);
#ifdef MIPC_DEBUG
          fclose(_mc->_debugLog);
#endif
          printf("Register state on termination:\n\n");
          exit(0);
        }
      else {
        
          if (writeREG && NOP==0)
            {
              _mc->_gpr[decodedDST] = opResultLo;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Writing to reg %u, value: %#x\n", SIM_TIME, decodedDST, _mc->_gpr[decodedDST]);
#endif
            }
          else if (writeFREG && NOP==0)
            {
              _mc->_fpr[(decodedDST)>>1].l[FP_TWIDDLE^((decodedDST)&1)] = opResultLo;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Writing to freg %u, value: %#x\n", SIM_TIME, decodedDST>>1, opResultLo);
#endif
            }
          else if (loWPort || hiWPort)
            {
              if (loWPort) {
                lo = opResultLo;
#ifdef MIPC_DEBUG
                fprintf(_mc->_debugLog, "<%llu> Writing to Lo, value: %#x\n", SIM_TIME, opResultLo);
#endif
              }
              if (hiWPort)
                {
                  hi = opResultHi;
#ifdef MIPC_DEBUG
                  fprintf(_mc->_debugLog, "<%llu> Writing to Hi, value: %#x\n", SIM_TIME, opResultHi);
#endif
                }
            }

      }
      _mc->_gpr[0] = 0;
      
      AWAIT_P_PHI1;       // @negedge
      _mc->ID_EX_hi = hi;
      _mc->ID_EX_lo = lo;
    }
}
