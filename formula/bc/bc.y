%{
#include <stdio.h>
#include <string.h>
#include "../../src/common/formula.h"
#include "../../src/common/cJSON.h"
extern FILE* yyin;
extern FILE* yyout;
extern char* yytext;
static int result = 0;
void *g_ret;
int err_flag=0;
%}

%token NUMBER
%token ADD SUB MUL DIV ABS
%token OP CP
%token EOL

%%

calclist: /* nothing */
	| calclist exp EOL { result = $2;/* printf("= %d\n> ", $2);*/ }
	| calclist EOL { /*printf("> ");*/ } /* blank line or a comment */
	;

exp: factor
	| exp ADD exp { $$ = $1 + $3; }
	| exp SUB factor { $$ = $1 - $3; }
	| exp ABS factor { $$ = $1 | $3; }
    ;

factor: term
	| factor MUL term { $$ = $1 * $3; }
	| factor DIV term { $$ = $1 / $3; }
	;

term: NUMBER
	| ABS term { $$ = $2 >= 0? $2 : - $2; }
	| OP exp CP { $$ = $2; }
	;
%%


//int main(int argc,char**argv)
//{
//
////	yydebug = 1;
//	yyin = fopen(argv[1], "r");
//	return yyparse();
//}

void yyerror(char* s)
{
		err_flag = 1;
		//fprintf(stderr,"%s",s);
		int len = sprintf(g_ret,"%s",s);
		if( len >= FORMULA_BUFLEN) return ;
} 

int yywrap()
{
		return 1;
}

int gsh_formula_bc_init(void *arg,void *ret)
{
		return 1;
}

int gsh_formula_bc_run(void *arg,void *ret)
{
		g_ret = ret;
		cJSON *data = (cJSON*)arg;
		cJSON *exp = cJSON_GetObjectItem(data,"exp");
		if(!exp)
		{
				return 0;
		}
		FILE *f = fopen(".tmp.txt","w");
		if(!f) 
		{
				fprintf(stderr,"open tmp.txt failed.\r\n");
				exit(1);
		}
		int sz = fwrite(exp->valuestring,strlen(exp->valuestring),1,f);	
		if(sz!=1)
		{
				fprintf(stderr,"write file failed.\r\n");
		}
		char a = '\n';
		sz = fwrite(&a,1,1,f);	
		fflush(f);
		fclose(f);
		
		yyin = fopen(".tmp.txt","r");
		yyparse();
		fclose(yyin);
		if(err_flag)
		{	
				err_flag = 0;
				return 0;
		}
		int len = sprintf(ret,"%d",result);
		if( len >= FORMULA_BUFLEN) return 0;
		return 1;
}




