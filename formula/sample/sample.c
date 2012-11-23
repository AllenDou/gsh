#include "sample.h"

/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_sample_init(void *arg,void *ret)
{
		return 1;
}

int gsh_formula_sample_run(void *arg,void *ret)
{
		int len = sprintf(ret,"1111111111111111111");
		if( len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}

