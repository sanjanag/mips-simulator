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
  unsigned int pc;
  int lastbd;
  int currbd;
  unsigned EX_MEM_decodedDST, MEM_WB_decodedDST;
  int null;
  Bool isSyscall;
  while (1)
    {
      AWAIT_P_PHI0;	// @posedge

      ins = _mc->IF_ID_ins;
      pc = _mc->IF_ID_pc;
      lastbd = _mc->ID_EX_bd;
      currbd = _mc->ID_EX_bd;
      EX_MEM_decodedDST = _mc->EX_MEM_decodedDST;
      MEM_WB_decodedDST = _mc->MEM_WB_decodedDST;
      null = _mc->ID_EX_null;
      npc = _mc->IF_ID_npc;
      
      AWAIT_P_PHI1;	// @negedge

      if(null == 1)
        {
          ins = 0;
          _mc->Dec(0);
        }
      else
        {

          _mc->Dec(ins);

          //check for RAW hazards
          if((_mc->ID_EX_decodedSRC1 == EX_MEM_decodedDST || _mc->ID_EX_decodedSRC2 == EX_MEM_decodedDST ||) || ( _mc->ID_EX_decodedSRC1 == MEM_WB_decodedDST || _mc->ID_EX_decodedSRC2 == MEM_WB_decodedDST))
            _mc->ID_EX_raw = 1;

      
          if(currbd || _mc->ID_EX_raw || _mc->isSyscall)
            _mc->ID_EX_null = 1;

          _mc->ID_EX_npc = npc;
          if(_mc->ID_EX_raw)
            {
              ins = 0;
              _mc->Dec(ins);
              _mc->ID_EX_npc = pc;
            }
        }
      ID_EX_ins = ins;
      ID_EX_pc = pc; // for jalr
     
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
#endif
    
    
    }
}
    
