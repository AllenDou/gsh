all:
	lex bc.l
	yacc -t -v -d bc.y
	gcc -g -O0 lex.yy.c y.tab.c ../../src/common/cJSON.c -fPIC -shared -o ../../lib/libbc.so
clean:
	rm -rf ../../lib/libbc.so ../../.tmp.txt

