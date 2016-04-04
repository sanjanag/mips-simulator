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
  unsigned EX_MEM_decodedDST, ID_EX_decodedDST;
  unsigned int EX_MEM_ins, ID_EX_ins;
  int ID_EX_NOP, EX_MEM_NOP;
  Bool EX_MEM_writeFREG,EX_MEM_writeREG,ID_EX_writeFREG,ID_EX_writeREG;
  
  while (1)
    {
      AWAIT_P_PHI0;	// @posedge


      int stallDECODE = _mc->_stallDECODE;
      EX_MEM_decodedDST = _mc->EX_MEM_decodedDST;
      ID_EX_decodedDST = _mc->ID_EX_decodedDST;
      EX_MEM_ins = _mc->EX_MEM_ins;
      ID_EX_ins = _mc->ID_EX_ins;
      ID_EX_NOP = _mc->ID_EX_NOP;
      EX_MEM_NOP = _mc->EX_MEM_NOP;
      EX_MEM_writeFREG = _mc->EX_MEM_writeFREG;
      EX_MEM_writeREG = _mc->EX_MEM_writeREG;
      ID_EX_writeFREG = _mc->ID_EX_writeFREG;
      ID_EX_writeREG = _mc->ID_EX_writeREG;
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog,"<%llu> RAW %d\n",SIM_TIME, _mc->_RAW);
#endif
      if(!_mc->_RAW && !_mc->_SYSCALL)
        {
          ins = _mc->IF_ID_ins;
          pc = _mc->IF_ID_pc;
          bd = _mc->IF_ID_bd;
          _mc->_RAW = 0;
        }
      else
        {
          ins = _mc->_DEC_ins;
          pc = _mc->_DEC_pc;
          bd = _mc->_DEC_bd;
          _mc->_RAW = 0;
        }
      
      AWAIT_P_PHI1;	// @negedge

      if(_mc->_stallFETCH > 0)
        _mc->_stallFETCH -= 1;
      int hazard = 0;
      if(stallDECODE  == 2)
        {
          ins = 0;
          _mc->Dec(pc, ins, bd);
          _mc->ID_EX_NOP = 1;
          _mc->_stallDECODE = 1;
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Decoder stalled. Decoded ins %#x \n", SIM_TIME,ins);
#endif
        }
      else if(stallDECODE == 1)
        {
          ins = 0;
          _mc->Dec(pc,ins,bd);
          _mc->ID_EX_NOP = 1;
          _mc->_stallDECODE = 0;
          _mc->_SYSCALL = 0;
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Decoder stalled. Decoded ins 0 \n", SIM_TIME);
#endif
        }
      
      else if(stallDECODE == 0)
        {
          if(ins != 0)
            {
              _mc->ID_EX_NOP = 0;
            }
          else
            {
              _mc->ID_EX_NOP = 1;
              #ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> ins = 0\n",SIM_TIME);
              #endif
            }

          _mc->Dec(pc, ins, bd);
          
#ifdef MIPC_DEBUG
          if(_mc->ID_EX_writeREG)
            fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x  BD %d TGT %#x decodedSRC1 %#x decodedSRC2 %#x decodedDST %u\n", SIM_TIME, ins, _mc->ID_EX_bd,_mc->ID_EX_btgt,_mc->ID_EX_decodedSRC1,_mc->ID_EX_decodedSRC2,_mc->ID_EX_decodedDST);
          else if(_mc->ID_EX_writeFREG)
            fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x  BD %d TGT %#x decodedSRC1 %#x decodedSRC2 %#x decodedDST %u\n", SIM_TIME, ins, _mc->ID_EX_bd,_mc->ID_EX_btgt,_mc->ID_EX_decodedSRC1,_mc->ID_EX_decodedSRC2,_mc->ID_EX_decodedDST);
          else
            fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x  BD %d TGT %#x decodedSRC1 %#x decodedSRC2 %#x decodedDST %u\n", SIM_TIME, ins, _mc->ID_EX_bd,_mc->ID_EX_btgt,_mc->ID_EX_decodedSRC1,_mc->ID_EX_decodedSRC2,_mc->ID_EX_decodedDST);
#endif
          
        }      

      //check if syscall
      if(_mc->ID_EX_isSyscall)
        {
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x is a syscall\n", SIM_TIME,ins);
#endif
          _mc->_DEC_ins = ins;
          _mc->_DEC_pc = pc;
          _mc->_DEC_bd = bd;

          _mc->_stallFETCH = 2;
          _mc->_stallDECODE = 2;
          _mc->_SYSCALL = 1;
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog, "<%llu> stalling _stallFETCH %d _stallDECODE %d _SYSCALL %d \n", SIM_TIME,_mc->_stallFETCH, _mc->_stallDECODE, _mc->_SYSCALL);
#endif

        }
else{
      //check for RAW hazards
      if(_mc->ID_EX_NOP != 1 && ID_EX_NOP != 1)
        {
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog,"<%llu> Checking RAW hazard for ins %#x ID_EX_writeREG %d ID_EX_writeFREG %d\n",SIM_TIME,ins,ID_EX_writeREG,ID_EX_writeFREG);
#endif

          if(ID_EX_writeREG && (_mc->ID_EX_decodedSRC1 == _mc->_gpr[ID_EX_decodedDST] || _mc->ID_EX_decodedSRC2 == _mc->_gpr[ID_EX_decodedDST]))
            {

              
              hazard = 1;
            }
          else if(ID_EX_writeFREG && (_mc->ID_EX_decodedSRC1 == _mc->_fpr[(ID_EX_decodedDST)>>1].l[FP_TWIDDLE^((ID_EX_decodedDST)&1)] || _mc->ID_EX_decodedSRC2 == _mc->_fpr[(ID_EX_decodedDST)>>1].l[FP_TWIDDLE^((ID_EX_decodedDST)&1)]))
            hazard = 1;
          else
            hazard = 0;
          
          if(hazard == 1)
            {
              _mc->_RAW = 1;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x has a RAW with prev to prev ins\n", SIM_TIME,ins);
#endif
          
              _mc->_DEC_ins = ins;
              _mc->_DEC_pc = pc;
              _mc->_DEC_bd = bd;
              ins = 0;
              _mc->Dec(pc, ins, bd);
              _mc->ID_EX_NOP = 1;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Nullifying ins %#x by decoding ins %#x \n", SIM_TIME,_mc->_DEC_ins,ins);
#endif          
              _mc->_stallFETCH = 1;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> stalling _stallFETCH %d _RAW %d  \n", SIM_TIME,_mc->_stallFETCH, _mc->_RAW);
#endif
            }
            
        }
      if(_mc->ID_EX_NOP != 1 &&  EX_MEM_NOP != 1 && hazard==0)
        {
#ifdef MIPC_DEBUG
          fprintf(_mc->_debugLog,"<%llu> Checking RAW hazard for ins %#x EX_MEM\n",SIM_TIME,ins);
#endif

          if(EX_MEM_writeREG && (_mc->ID_EX_decodedSRC1 == _mc->_gpr[EX_MEM_decodedDST] || _mc->ID_EX_decodedSRC2 == _mc->_gpr[EX_MEM_decodedDST])) 
            hazard =1;
            
          else if(EX_MEM_writeFREG && (_mc->ID_EX_decodedSRC1 == _mc->_fpr[(EX_MEM_decodedDST)>>1].l[FP_TWIDDLE^((EX_MEM_decodedDST)&1)] || _mc->ID_EX_decodedSRC2 == _mc->_fpr[(EX_MEM_decodedDST)>>1].l[FP_TWIDDLE^((EX_MEM_decodedDST)&1)]))
                hazard =1;
            
          else
            hazard = 0;
            
          if(hazard == 1)
            {

#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x has a RAW with prev\n", SIM_TIME,ins);
#endif
              _mc->_RAW = 1;    
              _mc->_DEC_ins = ins;
              _mc->_DEC_pc = pc;
              _mc->_DEC_bd = bd;
              ins = 0;
              _mc->Dec(pc, ins, bd);
              _mc->ID_EX_NOP = 1;
#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> Nullifying ins %#x by decoding ins %#x \n", SIM_TIME,_mc->_DEC_ins,ins);
#endif          
              _mc->_stallFETCH = 1;

#ifdef MIPC_DEBUG
              fprintf(_mc->_debugLog, "<%llu> stalling _stallFETCH %d _RAW %d  \n", SIM_TIME,_mc->_stallFETCH, _mc->_RAW);
#endif
            }
            
          
        }
 }

      _mc->ID_EX_pc = pc; // for jalr
      _mc->ID_EX_ins = ins;
      
    }
}
    
