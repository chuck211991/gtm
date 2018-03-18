#include "mdef.h"
#include "compiler.h"
#include "opcode.h"
#include "toktyp.h"
#include "advancewindow.h"

error_def(ERR_COMMA);

int f_zpiecediff(oprtype *a, opctype op)
{
	triple *args[3];
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	args[0] = maketriple(op);
	if (EXPR_FAIL == expr(&(args[0]->operand[0]), MUMPS_STR))
	 	return FALSE;
	if (TK_COMMA != TREF(window_token))
	{
		stx_error(ERR_COMMA);
		return FALSE;
	}
	
	args[1] = newtriple(OC_PARAMETER);
	advancewindow();
	if (EXPR_FAIL == expr(&(args[1]->operand[0]), MUMPS_STR))
		return FALSE;
	if (TK_COMMA != TREF(window_token))
	{
		stx_error(ERR_COMMA);
		return FALSE;
	}
	args[0]->operand[1] = put_tref(args[1]);
	
	args[2] = newtriple(OC_PARAMETER);
	advancewindow();
	if (EXPR_FAIL == expr(&(args[2]->operand[0]), MUMPS_STR))
		return FALSE;
	args[1]->operand[1] = put_tref(args[2]);
	
	ins_triple(args[0]);
	*a = put_tref(args[0]);
	return TRUE;
}