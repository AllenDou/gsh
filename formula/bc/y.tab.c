#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>
#include <string.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20100216

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)

#define YYPREFIX "yy"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
#ifdef YYPARSE_PARAM_TYPE
#define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
#else
#define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
#endif
#else
#define YYPARSE_DECL() yyparse(void)
#endif /* YYPARSE_PARAM */

extern int YYPARSE_DECL();

#line 2 "bc.y"
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
#line 45 "y.tab.c"
#define NUMBER 257
#define ADD 258
#define SUB 259
#define MUL 260
#define DIV 261
#define ABS 262
#define OP 263
#define CP 264
#define EOL 265
#define YYERRCODE 256
static const short yylhs[] = {                           -1,
    0,    0,    0,    1,    1,    1,    1,    2,    2,    2,
    3,    3,    3,
};
static const short yylen[] = {                            2,
    0,    3,    2,    1,    3,    3,    3,    1,    3,    3,
    1,    2,    3,
};
static const short yydefred[] = {                         1,
    0,   11,    0,    0,    3,    0,    0,    8,   12,    0,
    0,    0,    0,    2,    0,    0,   13,    0,    0,    0,
    9,   10,
};
static const short yydgoto[] = {                          1,
    6,    7,    8,
};
static const short yysindex[] = {                         0,
 -256,    0, -222, -222,    0, -228, -258,    0,    0, -226,
 -222, -222, -222,    0, -222, -222,    0, -216, -258, -258,
    0,    0,
};
static const short yyrindex[] = {                         0,
    0,    0,    0,    0,    0,    0, -254,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -220, -244, -236,
    0,    0,
};
static const short yygindex[] = {                         0,
   13,   35,   -3,
};
#define YYTABLESIZE 48
static const short yytable[] = {                          9,
    2,   15,   16,    4,    4,    3,    4,    4,    5,    4,
    4,   21,   22,    6,    6,    0,   10,    6,    0,    6,
    6,    7,    7,   18,    0,    7,    0,    7,    7,   11,
   12,   11,   12,   13,    2,   13,   14,   17,    0,    3,
    4,   11,   12,    5,    5,   13,   19,   20,
};
static const short yycheck[] = {                          3,
  257,  260,  261,  258,  259,  262,  263,  262,  265,  264,
  265,   15,   16,  258,  259,   -1,    4,  262,   -1,  264,
  265,  258,  259,   11,   -1,  262,   -1,  264,  265,  258,
  259,  258,  259,  262,  257,  262,  265,  264,   -1,  262,
  263,  258,  259,  264,  265,  262,   12,   13,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 265
#if YYDEBUG
static const char *yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"NUMBER","ADD","SUB","MUL","DIV",
"ABS","OP","CP","EOL",
};
static const char *yyrule[] = {
"$accept : calclist",
"calclist :",
"calclist : calclist exp EOL",
"calclist : calclist EOL",
"exp : factor",
"exp : exp ADD exp",
"exp : exp SUB factor",
"exp : exp ABS factor",
"factor : term",
"factor : factor MUL term",
"factor : factor DIV term",
"term : NUMBER",
"term : ABS term",
"term : OP exp CP",

};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#if YYDEBUG
#include <stdio.h>
#endif

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;

typedef struct {
    unsigned stacksize;
    short    *s_base;
    short    *s_mark;
    short    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;

#define YYPURE 0

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 42 "bc.y"


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




#line 248 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = data->s_mark - data->s_base;
    newss = (data->s_base != 0)
          ? (short *)realloc(data->s_base, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    data->s_base  = newss;
    data->s_mark = newss + i;

    newvs = (data->l_base != 0)
          ? (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack)) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 2:
#line 22 "bc.y"
	{ result = yystack.l_mark[-1];/* printf("= %d\n> ", $2);*/ }
break;
case 3:
#line 23 "bc.y"
	{ /*printf("> ");*/ }
break;
case 5:
#line 27 "bc.y"
	{ yyval = yystack.l_mark[-2] + yystack.l_mark[0]; }
break;
case 6:
#line 28 "bc.y"
	{ yyval = yystack.l_mark[-2] - yystack.l_mark[0]; }
break;
case 7:
#line 29 "bc.y"
	{ yyval = yystack.l_mark[-2] | yystack.l_mark[0]; }
break;
case 9:
#line 33 "bc.y"
	{ yyval = yystack.l_mark[-2] * yystack.l_mark[0]; }
break;
case 10:
#line 34 "bc.y"
	{ yyval = yystack.l_mark[-2] / yystack.l_mark[0]; }
break;
case 12:
#line 38 "bc.y"
	{ yyval = yystack.l_mark[0] >= 0? yystack.l_mark[0] : - yystack.l_mark[0]; }
break;
case 13:
#line 39 "bc.y"
	{ yyval = yystack.l_mark[-1]; }
break;
#line 486 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (short) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
