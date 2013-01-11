LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC =gcc
CFLAGS = -Wall -g3 -rdynamic -ggdb -O0 
FORMULA=../../lib/libcache.so
LINK=-lm -fPIC -shared

QUIET_LINK = @printf ' %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)cache$(ENDCOLOR);

FORMULA_OBJ= cache.o dsl_assoc.o hash.o rbtree.o common.o ../../src/common/cJSON.o

all: $(FORMULA)

$(FORMULA): $(FORMULA_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o $@ $^ $(LINK)
clean:
	$(RM) $(FORMULA) *.o
