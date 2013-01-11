#!/bin/sh
if [ -z $1 ]; then
	echo "formula name is empty."
	exit 1
fi

HFILE=$1.h
CFILE=$1.c

if [ -d $1 ]; then
	echo "formula $1 is exist..."
	exit 1
	#rm -rf $1
fi

echo "generate $1 formula begin...."

mkdir $1
cd $1

# generate .h file start. #
echo "#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include \"../../src/common/formula.h\"
int gsh_formula_$1_init(void *arg,void *p);
int gsh_formula_$1_run(void *arg,void *p);
" > $HFILE

# generate .c file start. #
echo "#include \"$1.h\"
/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_$1_init(void *arg,void *ret){

		return 1;
}

int gsh_formula_$1_run(void *arg,void *ret){

		int len = sprintf(ret,\"$1 formula test....\");
		if( len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}
"> $CFILE


# generate Makefile start. #
#echo "
#all:
#	gcc -Wall -g -rdynamic -ggdb -O0 $1.c -fPIC -shared -o ../../lib/lib$1.so
#clean:
#	rm -rf ../../lib/lib$1.so
#" > Makefile

echo "\
LINKCOLOR=\"\033[34;1m\"
BINCOLOR=\"\033[37;1m\"
ENDCOLOR=\"\033[0m\"

CC =gcc
CFLAGS = -Wall -g3 -rdynamic -ggdb -O0 -fPIC 
FORMULA=../../lib/lib$1.so
LINK=-lm -fPIC -shared

QUIET_LINK = @printf ' %b %b\n' \$(LINKCOLOR)LINK\$(ENDCOLOR) \$(BINCOLOR)$@\$(ENDCOLOR);

FORMULA_OBJ= $1.o

all: \$(FORMULA)

\$(FORMULA): \$(FORMULA_OBJ)
	\$(QUIET_LINK) \$(CC) \$(CFLAGS) -o \$@ $^ \$(LINK)
clean:
	\$(RM) \$(FORMULA) *.o
" > Makefile

# add new formula to conf start #
cd ../../etc/
#grep -v "^formula" gsh.conf > gsh.conf.tmp
cp gsh.conf gsh.conf.tmp
echo "formula $1 " >> gsh.conf.tmp
cp gsh.conf.tmp gsh.conf

echo "generate $1 formula finished...."
#echo "formula $1 $1_test " >> ../../etc/gsh.conf







