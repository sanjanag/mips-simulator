#ifndef __MIPS_H__
#define __MIPS_H__

#include "sim.h"

class Mipc;
class MipcSysCall;
class SysCall;

typedef unsigned Bool;
#define TRUE 1
#define FALSE 0

#if BYTE_ORDER == LITTLE_ENDIAN

#define FP_TWIDDLE 0

#else

#define FP_TWIDDLE 1

#endif

#include "mem.h"
#include "../../common/syscall.h"
#include "queue.h"

#define MIPC_DEBUG 1

class Mipc : public SimObject {
public:
   Mipc (Mem *m);
   ~Mipc ();

   int _stallFETCH;
   int _stallDECODE;
   int _SYSCALL;
   int _RAW;
   int _BTAKEN;
   unsigned int _DEC_ins;
   unsigned int _DEC_pc;
   unsigned int _DEC_bd;

   struct temp_contents
   {
     unsigned opResultLo, opResultHi;
     int btaken;
   } EX_MEM_contents ;

   unsigned tempsubregOperand;
   unsigned tempdecodedDST;
	temp_contents* contents ;

   unsigned int _npc;
   unsigned _MAR_EX; //to store the destination for load store in EX
   unsigned _MAR_MEM; //to update the destination with load/store value
   //Needed because update happens before use
   
   //Pipeline registers  IF/ID
   unsigned int IF_ID_pc;
   unsigned int IF_ID_ins;
   unsigned int IF_ID_bd;
   int IF_ID_NOP;
   int IF_ID_BRANCH;
   
   //Pipeline registers ID/EX
   unsigned int ID_EX_pc;
   unsigned int ID_EX_ins;
   unsigned int ID_EX_bd;
   Bool ID_EX_isSyscall;
   Bool ID_EX_isIllegalOp;
   signed int	ID_EX_decodedSRC1, ID_EX_decodedSRC2;
   unsigned	ID_EX_decodedDST;
   Bool		ID_EX_writeREG, ID_EX_writeFREG;
   Bool 	ID_EX_hiWPort, ID_EX_loWPort;
   Bool 	ID_EX_memControl; 
   void (*ID_EX_opControl)(Mipc*, unsigned);
   unsigned	ID_EX_decodedShiftAmt;
   unsigned int	ID_EX_btgt; 
   signed int	ID_EX_branchOffset;
   unsigned 	ID_EX_subregOperand;
   void (*ID_EX_memOp)(Mipc*);
   unsigned int ID_EX_hi, ID_EX_lo; //not updated yet
   int ID_EX_NOP;
   int ID_EX_BRANCH;
   
   //Pipeline registers EX/MEM
   unsigned int EX_MEM_pc;
   unsigned int EX_MEM_ins;
   unsigned int EX_MEM_bd;
   Bool EX_MEM_isSyscall;
   Bool EX_MEM_isIllegalOp;
   signed int EX_MEM_decodedSRC1, EX_MEM_decodedSRC2;
   unsigned	EX_MEM_decodedDST;
   Bool		EX_MEM_writeREG, EX_MEM_writeFREG;
   Bool EX_MEM_memControl; 
   void (*EX_MEM_opControl)(Mipc*, unsigned);
   unsigned EX_MEM_decodedShiftAmt;
   unsigned int EX_MEM_btgt;
   signed int EX_MEM_branchOffset;
   unsigned 	EX_MEM_subregOperand;
   void (*EX_MEM_memOp)(Mipc*);
   Bool 	EX_MEM_hiWPort, EX_MEM_loWPort;
   unsigned	EX_MEM_opResultHi, EX_MEM_opResultLo;
   int 		EX_MEM_btaken;
   unsigned int EX_MEM_hi, EX_MEM_lo;
   int EX_MEM_NOP;
   int EX_MEM_BRANCH;
   
   //Pipeline registers MEM/WB
   unsigned int MEM_WB_pc;
   unsigned int MEM_WB_ins;
   unsigned int MEM_WB_bd;
   Bool MEM_WB_isSyscall;
   Bool MEM_WB_isIllegalOp;
   signed int MEM_WB_decodedSRC1, MEM_WB_decodedSRC2;
   unsigned	MEM_WB_decodedDST;
   Bool		MEM_WB_writeREG, MEM_WB_writeFREG;
   Bool MEM_WB_memControl; 
   void (*MEM_WB_opControl)(Mipc*, unsigned);
   unsigned MEM_WB_decodedShiftAmt;
   unsigned int MEM_WB_btgt;
   signed int MEM_WB_branchOffset;
   unsigned 	MEM_WB_subregOperand;
   void (*MEM_WB_memOp)(Mipc*);
   Bool 	MEM_WB_hiWPort, MEM_WB_loWPort;
   unsigned	MEM_WB_opResultHi, MEM_WB_opResultLo;
   int 		MEM_WB_btaken;
   unsigned int MEM_WB_hi, MEM_WB_lo;
   int MEM_WB_NOP;
   int MEM_WB_BRANCH;
   
   FAKE_SIM_TEMPLATE;

   MipcSysCall *_sys;		// Emulated system call layer

   void dumpregs (unsigned int _pc);	// Dumps current register state

   void Reboot (char *image = NULL);
				// Restart processor.
				// "image" = file name for new memory
				// image if any.

   void MipcDumpstats();			// Prints simulation statistics
   void Dec (unsigned int pc, unsigned int ins, unsigned int bd);			// Decoder function
   void fake_syscall (unsigned int ins, unsigned int pc);	// System call interface


   unsigned int 	_gpr[32];		// general-purpose integer registers

   union {
      unsigned int l[2];
      float f[2];
      double d;
   } _fpr[16];					// floating-point registers (paired)

   unsigned int _boot;				// boot code loaded?


   // Simulation statistics counters

   LL	_nfetched;
   LL	_num_cond_br;
   LL	_num_jal;
   LL	_num_jr;
   LL   _num_load;
   LL   _num_store;
   LL   _fpinst;

   Mem	*_mem;	// attached memory (not a cache)

   Log	_l;
   int  _sim_exit;		// 1 on normal termination

   //   void (*_opControl)(Mipc*, unsigned);
   //   void (*_memOp)(Mipc*);

   FILE *_debugLog;

   // EXE stage definitions

   static void func_add_addu (Mipc*, unsigned);
   static void func_and (Mipc*, unsigned);
   static void func_nor (Mipc*, unsigned);
   static void func_or (Mipc*, unsigned);
   static void func_sll (Mipc*, unsigned);
   static void func_sllv (Mipc*, unsigned);
   static void func_slt (Mipc*, unsigned);
   static void func_sltu (Mipc*, unsigned);
   static void func_sra (Mipc*, unsigned);
   static void func_srav (Mipc*, unsigned);
   static void func_srl (Mipc*, unsigned);
   static void func_srlv (Mipc*, unsigned);
   static void func_sub_subu (Mipc*, unsigned);
   static void func_xor (Mipc*, unsigned);
   static void func_div (Mipc*, unsigned);
   static void func_divu (Mipc*, unsigned);
   static void func_mfhi (Mipc*, unsigned);
   static void func_mflo (Mipc*, unsigned);
   static void func_mthi (Mipc*, unsigned);
   static void func_mtlo (Mipc*, unsigned);
   static void func_mult (Mipc*, unsigned);
   static void func_multu (Mipc*, unsigned);
   static void func_jalr (Mipc*, unsigned);
   static void func_jr (Mipc*, unsigned);
   static void func_await_break (Mipc*, unsigned);
   static void func_syscall (Mipc*, unsigned);
   static void func_addi_addiu (Mipc*, unsigned);
   static void func_andi (Mipc*, unsigned);
   static void func_lui (Mipc*, unsigned);
   static void func_ori (Mipc*, unsigned);
   static void func_slti (Mipc*, unsigned);
   static void func_sltiu (Mipc*, unsigned);
   static void func_xori (Mipc*, unsigned);
   static void func_beq (Mipc*, unsigned);
   static void func_bgez (Mipc*, unsigned);
   static void func_bgezal (Mipc*, unsigned);
   static void func_bltzal (Mipc*, unsigned);
   static void func_bltz (Mipc*, unsigned);
   static void func_bgtz (Mipc*, unsigned);
   static void func_blez (Mipc*, unsigned);
   static void func_bne (Mipc*, unsigned);
   static void func_j (Mipc*, unsigned);
   static void func_jal (Mipc*, unsigned);
   static void func_lb (Mipc*, unsigned);
   static void func_lbu (Mipc*, unsigned);
   static void func_lh (Mipc*, unsigned);
   static void func_lhu (Mipc*, unsigned);
   static void func_lwl (Mipc*, unsigned);
   static void func_lw (Mipc*, unsigned);
   static void func_lwr (Mipc*, unsigned);
   static void func_lwc1 (Mipc*, unsigned);
   static void func_swc1 (Mipc*, unsigned);
   static void func_sb (Mipc*, unsigned);
   static void func_sh (Mipc*, unsigned);
   static void func_swl (Mipc*, unsigned);
   static void func_sw (Mipc*, unsigned);
   static void func_swr (Mipc*, unsigned);
   static void func_mtc1 (Mipc*, unsigned);
   static void func_mfc1 (Mipc*, unsigned);

   // MEM stage definitions

   static void mem_lb (Mipc*);
   static void mem_lbu (Mipc*);
   static void mem_lh (Mipc*);
   static void mem_lhu (Mipc*);
   static void mem_lwl (Mipc*);
   static void mem_lw (Mipc*);
   static void mem_lwr (Mipc*);
   static void mem_lwc1 (Mipc*);
   static void mem_swc1 (Mipc*);
   static void mem_sb (Mipc*);
   static void mem_sh (Mipc*);
   static void mem_swl (Mipc*);
   static void mem_sw (Mipc*);
   static void mem_swr (Mipc*);
};


// Emulated system call interface

class MipcSysCall : public SysCall {
public:

   MipcSysCall (Mipc *ms) {

      char buf[1024];
      m = ms->_mem;
      _ms = ms;
      _num_load = 0;
      _num_store = 0;
   };

   ~MipcSysCall () { };

   LL GetDWord (LL addr);
   void SetDWord (LL addr, LL data);

   Word GetWord (LL addr);
   void SetWord (LL addr, Word data);
  
   void SetReg (int reg, LL val);
   LL GetReg (int reg);
   LL GetTime (void);

private:

   Mipc *_ms;
};
#endif /* __MIPS_H__ */
