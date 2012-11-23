#include "bc.h"
/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_bc_init(void *arg,void *ret)
{
		return 1;
}

int gsh_formula_bc_run(void *arg,void *ret)
{
		int len = sprintf(ret,"bc formula test....");
		if( len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}

