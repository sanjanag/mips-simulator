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
  int bd;

  while (1)
    {
      AWAIT_P_PHI0;	// @posedge
      if(!_mc->_RAW)
        {
          ins = _mc->IF_ID_ins;
          pc = _mc->IF_ID_pc;
          bd = _mc->ID_EX_bd;
        }
      else
        {
          ins = _mc->_DEC_ins;
          pc = _mc->_DEC_pc;
          bd = _mc->_DEC_bd;
        }
      
      AWAIT_P_PHI1;	// @negedge

      _mc->Dec(pc, ins, bd);
      
      
      //check for RAW hazards
      if((_mc->ID_EX_decodedSRC1 == EX_MEM_decodedDST || _mc->ID_EX_decodedSRC2 == EX_MEM_decodedDST ||) || ( _mc->ID_EX_decodedSRC1 == MEM_WB_decodedDST || _mc->ID_EX_decodedSRC2 == MEM_WB_decodedDST))
        {
          _mc->_DEC_ins = ins;
          _mc->_DEC_pc = pc;
          _mc->_DEC_bd = bd;
          ins = 0;
          _mc->Dec(pc, ins, bd);
          _mc->_stallFETCH = 1;
          _mc->_RAW = 1;
        }
      else
        {
          _mc->_stallFETCH = 0;
          _mc->_RAW = 0;
        }
      
      
      ID_EX_pc = pc; // for jalr
      ID_EX_ins = ins;
     
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
#endif
    
    }
}
    
