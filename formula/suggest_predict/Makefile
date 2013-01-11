LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC=gcc
CFLAGS= -Wall -g -rdynamic -ggdb -O0 -fPIC 
LINK=-lm -fPIC -shared -ldl

FORMULA=../../lib/libsuggest_predict.so
QUIET_LINK = @printf ' %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR);

SHARE_OBJ=lib/cJSON.o lib/hiredis.o lib/net.o lib/sds.o
FORMULA_OBJ=suggest_predict.o $(SHARE_OBJ)

all:$(FORMULA)

$(FORMULA):$(FORMULA_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o $@ $^ $(LINK)
clean:
	$(RM) $(FORMULA) *.o lib/*.o
