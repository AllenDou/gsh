#include "iask_wordseg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h>
#include "lexicon.h"
#include "fromuni.h"
#include "touni.h"
#include "ustring.h"
/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_iask_wordseg_init(void *arg,void *ret){

		LEXICON *lexicon = NULL;
		lexicon = OpenLexicon_Opt("/data0/iask-wordseg/", 0x1f);
		return 1;
}

int gsh_formula_iask_wordseg_run(void *arg,void *ret){

		int len = sprintf((char*)ret,"iask_wordseg formula test....");
		if( len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}

