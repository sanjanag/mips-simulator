#include "decode.h"

Decode::Decode (Mipc *mc)
{
  _mc = mc;
}

Decode::~Decode (void) {}

void
Decode::MainLoop (void)
{
  unsigned int ins;
  while (1)
    {
      AWAIT_P_PHI0;	// @posedge
      ins = IF_ID_ins;
    
      AWAIT_P_PHI1;	// @negedge
      _mc->Dec(ins);
      ID_EX_ins = IF_ID_ins;
      ID_EX_pc = IF_ID_pc; // for jalr
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
#endif
    
    
    }
}
    
