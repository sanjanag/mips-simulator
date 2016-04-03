#include "mips.h"
#include <assert.h>
#include "mips-irix5.h"
#include "app_syscall.h"

Mipc::Mipc (Mem *m) : _l('M')
{
  _mem = m;
  _sys = new MipcSysCall (this);	// Allocate syscall layer

#ifdef MIPC_DEBUG
  _debugLog = fopen("mipc.debug", "w");
  assert(_debugLog != NULL);
#endif
   
  Reboot (ParamGetString ("Mipc.BootROM"));
}

Mipc::~Mipc (void)
{

}

void 
Mipc::MainLoop (void)
{
  LL addr;
  unsigned int ins;	// Local instruction register
  unsigned int pc;
  int stallFETCH; 
  Assert (_boot, "Mipc::MainLoop() called without boot?");

  _nfetched = 0;
  
  while (!_sim_exit)
    {

      
      AWAIT_P_PHI0;	// @posedge
      pc = _npc;
      addr = pc;
      stallFETCH = _stallFETCH;
      #ifdef MIPC_DEBUG
      fprintf(_debugLog,"<%llu> stallFETCH %d\n",SIM_TIME, stallFETCH);
      #endif

      AWAIT_P_PHI1;	// @negedge
      if(stallFETCH == 0)
        {
          if(_BTAKEN==1)
            {  
              addr = _npc;
              _BTAKEN = 0;
            }
          
          ins = _mem->BEGetWord (addr, _mem->Read(addr & ~(LL)0x7)); //ins fetched
            _npc = _npc + 4;
            
          IF_ID_pc = pc;
          IF_ID_ins = ins;
          IF_ID_bd = 0;
          IF_ID_NOP = 0;
#ifdef MIPC_DEBUG
          fprintf(_debugLog, "<%llu> Fetched ins %#x from PC %#x\n", SIM_TIME, ins, pc);
#endif
          _nfetched++;
        }/*
      else
        {
          _stallFETCH = _stallFETCH -1;
          }*/
    }
  MipcDumpstats();
  Log::CloseLog();
   
#ifdef MIPC_DEBUG
  assert(_debugLog != NULL);
  fclose(_debugLog);
#endif

  exit(0);
}
          
void
Mipc::MipcDumpstats()
{
  Log l('*');
  l.startLogging = 0;

  l.print ("");
  l.print ("************************************************************");
  l.print ("");
  l.print ("Number of instructions: %llu", _nfetched);
  l.print ("Number of simulated cycles: %llu", SIM_TIME);
  l.print ("CPI: %.2f", ((double)SIM_TIME)/_nfetched);
  l.print ("Int Conditional Branches: %llu", _num_cond_br);
  l.print ("Jump and Link: %llu", _num_jal);
  l.print ("Jump Register: %llu", _num_jr);
  l.print ("Number of fp instructions: %llu", _fpinst);
  l.print ("Number of loads: %llu", _num_load);
  l.print ("Number of syscall emulated loads: %llu", _sys->_num_load);
  l.print ("Number of stores: %llu", _num_store);
  l.print ("Number of syscall emulated stores: %llu", _sys->_num_store);
  l.print ("");

}

void 
Mipc::fake_syscall (unsigned int ins, unsigned int _pc)
{
  //  printf("Inside fake_syscall pc %#x\n",_pc);
  _sys->pc = _pc;
  _sys->quit = 0;
  _sys->EmulateSysCall ();
  if (_sys->quit)
    _sim_exit = 1;
}

/*------------------------------------------------------------------------
 *
 *  Mipc::Reboot --
 *
 *   Reset processor state
 *
 *------------------------------------------------------------------------
 */
void 
Mipc::Reboot (char *image)
{
  FILE *fp;
  Log l('*');

  _boot = 0;

  if (image) {
    _boot = 1;
    printf ("Executing %s\n", image);
    fp = fopen (image, "r");
    if (!fp) {
      fatal_error ("Could not open `%s' for booting host!", image);
    }
    _mem->ReadImage(fp);
    fclose (fp);

    // Reset state
    /*          _ins = 0;
                _insValid = FALSE;
                _decodeValid = FALSE;
                _execValid = FALSE;
                _memValid = FALSE;
                _insDone = TRUE;

                _num_load = 0;
                _num_store = 0;
                _fpinst = 0;
                _num_cond_br = 0;
                _num_jal = 0;
                _num_jr = 0;

                _lastbd = 0;
                _bd = 0;
                _btaken = 0;
                _btgt = 0xdeadbeef;
                _sim_exit = 0;
                _pc = ParamGetInt ("Mipc.BootPC");	// Boom! GO
                /*----------------------------------*/
    cout << "Resetting states" << endl;
    for(int i=0;i<32;i++)
      _gpr[i] = 0;
    _npc = ParamGetInt ("Mipc.BootPC");	// Boom! GO
    IF_ID_BRANCH = 0;
    ID_EX_BRANCH = 0;
    EX_MEM_BRANCH = 0;
    MEM_WB_BRANCH = 0;
    IF_ID_NOP = 1;
    ID_EX_NOP = 1;
    EX_MEM_NOP = 1;
    MEM_WB_NOP = 1;
    EX_MEM_contents.opResultLo = 0;
    EX_MEM_contents.opResultHi = 0;
    EX_MEM_contents.btaken = 0;
    //   contents = &EX_MEM_contents;
    _num_load = 0;
    _num_store = 0;
    _fpinst = 0;
    _num_cond_br = 0;
    _num_jal = 0;
    _num_jr = 0;
    _sim_exit = 0;
    _BTAKEN = 0;
    //    _npc = ParamGetInt ("Mipc.BootPC");	// Boom! GO

    _stallFETCH = 0;
    _stallDECODE = 0;
    _SYSCALL = 0;
    _RAW = 0;
   
    _DEC_ins = 0;
    _DEC_pc = 0;;
    _DEC_bd = 0;
          
    tempsubregOperand = 0;
    tempdecodedDST = 0;
    contents = NULL;
    _MAR_EX = 0; //to store the destination for load store in EX
    _MAR_MEM = 0; //to update the destination with load/store value
    //Needed because update happens before use
   
    //Pipeline registers  IF/ID
    IF_ID_pc = 0;
    IF_ID_ins = 0;
    IF_ID_bd = 0;
   
    //Pipeline registers ID/EX
    ID_EX_pc = 0;
    ID_EX_ins = 0;
    ID_EX_bd = 0;
    ID_EX_isSyscall = FALSE;
    ID_EX_isIllegalOp = FALSE;
    ID_EX_decodedSRC1 = 0; ID_EX_decodedSRC2 = 0;;
    ID_EX_decodedDST = 0;;
    ID_EX_writeREG = TRUE; ID_EX_writeFREG = FALSE;
    ID_EX_hiWPort = FALSE; ID_EX_loWPort = FALSE;
    ID_EX_memControl = FALSE; 
    ID_EX_opControl = func_add_addu;
    ID_EX_decodedShiftAmt = 0;
    ID_EX_btgt = 0xdeadbeef; 
    ID_EX_branchOffset = 0;
    ID_EX_subregOperand = 0;
    ID_EX_memOp = mem_lb;
    ID_EX_hi = 0; ID_EX_lo = 0;
   
    //Pipeline registers EX/MEM
    EX_MEM_pc = 0;
    EX_MEM_ins = 0;
    EX_MEM_bd = 0;
    EX_MEM_isSyscall = FALSE;
    EX_MEM_isIllegalOp = FALSE;
    EX_MEM_decodedSRC1 = 0; EX_MEM_decodedSRC2 = 0;
    EX_MEM_decodedDST = 0;
    EX_MEM_writeREG = TRUE; EX_MEM_writeFREG = FALSE;
    EX_MEM_memControl = FALSE; 
    EX_MEM_opControl = func_add_addu;
    EX_MEM_decodedShiftAmt = 0;
    EX_MEM_btgt = 0xdeadbeef;
    EX_MEM_branchOffset = 0;
    EX_MEM_subregOperand = 0;
    EX_MEM_memOp = mem_lb;
    EX_MEM_hiWPort = FALSE; EX_MEM_loWPort = FALSE;
    EX_MEM_opResultHi = 0; EX_MEM_opResultLo = 0;
    EX_MEM_btaken = 0;
    EX_MEM_hi = 0; EX_MEM_lo = 0;
   
    //Pipeline registers MEM/WB
    MEM_WB_pc = 0;
    MEM_WB_ins = 0;
    MEM_WB_bd = 0;
    MEM_WB_isSyscall = FALSE;
    MEM_WB_isIllegalOp = FALSE;
    MEM_WB_decodedSRC1 = 0; MEM_WB_decodedSRC2 = 0;
    MEM_WB_decodedDST = 0;
    MEM_WB_writeREG = TRUE, MEM_WB_writeFREG = FALSE;
    MEM_WB_memControl = FALSE; 
    MEM_WB_opControl = func_add_addu;
    MEM_WB_decodedShiftAmt = 0;
    MEM_WB_btgt = 0xdeadbeef;
    MEM_WB_branchOffset = 0;
    MEM_WB_subregOperand = 0;
    MEM_WB_memOp = mem_lb;
    MEM_WB_hiWPort = FALSE; MEM_WB_loWPort = FALSE;
    MEM_WB_opResultHi = 0; MEM_WB_opResultLo = 0;
    MEM_WB_btaken = 0;
    MEM_WB_hi = 0; MEM_WB_lo = 0;

    cout << "Reset done" << endl;
  }
}

LL
MipcSysCall::GetDWord(LL addr)
{
  _num_load++;      
  return m->Read (addr);
}

void
MipcSysCall::SetDWord(LL addr, LL data)
{
  
  m->Write (addr, data);
  _num_store++;
}

Word 
MipcSysCall::GetWord (LL addr) 
{ 
  
  _num_load++;   
  return m->BEGetWord (addr, m->Read (addr & ~(LL)0x7)); 
}

void 
MipcSysCall::SetWord (LL addr, Word data) 
{ 
  
  m->Write (addr & ~(LL)0x7, m->BESetWord (addr, m->Read(addr & ~(LL)0x7), data)); 
  _num_store++;
}
  
void 
MipcSysCall::SetReg (int reg, LL val) 
{ 
  _ms->_gpr[reg] = val; 
}

LL 
MipcSysCall::GetReg (int reg) 
{
  return _ms->_gpr[reg]; 
}

LL
MipcSysCall::GetTime (void)
{
  return SIM_TIME;
}
