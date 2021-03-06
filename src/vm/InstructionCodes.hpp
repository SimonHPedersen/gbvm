//$Id: InstructionCodes.hpp,v 1.19 2001/03/14 14:27:49 hempe Exp $

#ifndef INSTRUCTION_CODES
#define INSTRUCTION_CODES

// PTN = pattern
// MP = main-part
// OP = operation
// OBJ = object
// CMP = component
// GEN = general
// SIMP = simple
// INT = integer
// REAL = real
// BOOL = boolean
// STR = string
// SPEC = sepcialize
// QUAL = qualification
// GT = greater than
// LIT = literal
// EQ = not equal
// NEQ = equal
// DIV = /
// DIVINT = DIV










































































enum instruction_id {
  ADDMP_ID = 500,
  ADDOPPLUS_ID,
  ADDOPMINUS_ID,
  ADDOPOR_ID,
  ADDOPXOR_ID,
  CALL_ID,
  CALLREP_ID,
  CHKNONE_ID,
  CHKPTNNONE_ID,
  CHKQUALOBJ_ID,
  DISCARD_ID,
  ENSURECMP_ID,
  EXTEND_ID,
  FORK_ID,
  GATHERVIRT_ID,
  GETSIZE_ID,
  GENERALIF_ID,
  INNER_ID,
  INSTALLPTN_ID,
  INSTALLQUALPTN_ID,
  INSTALLOBJ_ID,
  INSTALLQUALOBJ_ID,
  INSTALLREP_ID,
  KILL_ID,
  LOCSIMPIF_ID,
  MERGE_ID,
  MULOPMUL_ID,
  MULOPDIV_ID,
  MULOPAND_ID,
  MULOPDIVINT_ID,
  MULOPMOD_ID,
  NEGINT_ID,
  NEGREAL_ID,
  NEWPTNOBJ_ID,
  NEWPTNTMP_ID,
  NEWREP_ID,
  NOT_ID,
  NAMEDFOR_ID,
  OSSYSIN_ID,
  OSSYSINOUT_ID,
  PEEKINXOBJREF_ID,
  PEEKINXTMPREF_ID,
  PEEKOBJREF_ID,
  PEEKTMPREF_ID,
  POPBOOL_ID,
  POPBOOLVAL_ID,
  POPCHAR_ID,
  POPCHARPUSHINT_ID,
  POPCHARPUSHREAL_ID,
  POPCHARPUSHSTR_ID,
  POPCHARVAL_ID,
  POPINTPUSHCHARAT_ID,
  POPINT_ID,
  POPINTPUSHCHAR_ID,
  POPINTPUSHREAL_ID,
  POPINTVAL_ID,
  POPINXOBJREF_ID,
  POPINXPTNREF_ID,
  POPOBJPUSHPTN_ID,
  POPOBJPUSHTMP_ID,
  POPOBJREF_ID,
  POPPTNSPECOBJ,
  POPPTNREF_ID,
  POPREAL_ID,
  POPREALPUSHINT_ID,
  POPREALVALUE_ID,
  POPSTR_ID,
  POPSTRREP_ID,
  POPSTRVAL_ID,
  POPSTRPUSHCHAR_ID,
  POPSTRPUSHINT_ID,
  POPSTRPUSHREAL_ID,
  PUSHBOOL_ID,
  PUSHBOOLVAL_ID,
  PUSHCHAR_ID,
  PUSHCHARVAL_ID,
  PUSHINDEX_ID,
  PUSHINT_ID,
  PUSHINTVAL_ID,
  PUSHINXOBJ_ID,
  PUSHINXOBJREF_ID,
  PUSHINXPTNREF_ID,
  PUSHINXQUAL_ID,
  PUSHINXTMPOBJ_ID,
  PUSHINXTMPOBJREF_ID,
  PUSHOBJ_ID,
  PUSHOBJREF_ID,
  PUSHPTN_ID,
  PUSHPTNOBJ_ID,
  PUSHPTNREF_ID,
  PUSHQUAL_ID,
  PUSHREAL_ID,
  PUSHREALVAL_ID,
  PUSHSTRLEN_ID,
  PUSHSTRREP_ID,
  PUSHSTR_ID,
  PUSHSTRVAL_ID,
  PUSHTMPOBJ_ID,
  PUSHTMPOBJREF_ID,
  PUSHINONEOBJ_ID,
  PUSHINONEPTN_ID,
  PUSHIBOOLLIT_ID,
  PUSHICHARLIT_ID,
  PUSHIINTLIT_ID,
  PUSHIREALLIT_ID,
  PUSHISTRLIT_ID,
  RELOPLT_ID,
  RELOPLTEQ_ID,
  RELOPNEQ_ID,
  RELOPEQ_ID,
  RELOPGT_ID,
  RELOPGTEQ_ID,
  RESETFRAME_ID,
  RESIZEREP_ID,
  SEMCOUNT_ID,
  SEMP_ID,
  SEMTRYP_ID,
  SEMV_ID,
  SUSPEND_ID,
  SIMPLEFOR_ID,
  SIMPLEIF_ID,
  STDIOIN_ID,
  STDIOOUT_ID,
  WHEN_ID,
  WHILE_ID,
  RETURN_ID,                    //special instructions
  RETURNNPPO_ID,
  RETURNMPATTRINIT_ID,
  CONTGATHERVIRT_ID,
  JUMPNPPO_ID,
  JUMPTRUENPPO_ID,
  JUMPSUBNPPO_ID,
  COPYTOP_ID,
  COPYTOP2_ID,
  UNSUPPORTED_ID
};

enum types { 
  BOOLEAN_ID = 300,
  CHAR_ID,
  INTEGER_ID,
  OBJECT_ID,
  OBJECTREFERENCE_ID,
  PATTERN_ID,
  PATTERNREFERENCE_ID,
  REAL_ID,
  STRING_ID,
};

#endif


