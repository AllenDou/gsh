LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC =g++
CFLAGS = -g3 -rdynamic -ggdb -O0 -fPIC 
FORMULA=../../lib/libiask_wordseg.so
LINK=-lm -fPIC -shared -lstdc++ -ldl -m32

QUIET_LINK = @printf ' %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)iask_wordseg$(ENDCOLOR);

FORMULA_OBJ= iask_wordseg.o

all: $(FORMULA)

$(FORMULA): $(FORMULA_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o $@ $^ *.a  $(LINK)
clean:
	$(RM) $(FORMULA) *.o

