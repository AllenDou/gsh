LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC =gcc
CFLAGS = -Wall -g3 -rdynamic -ggdb -O0 -fPIC 
FORMULA=../../lib/libblog_tag.so
LINK=-lm -fPIC -shared

QUIET_LINK = @printf ' %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)blog_tag$(ENDCOLOR);

SHARE_OBJ=lib/cJSON.o lib/hiredis.o lib/net.o lib/sds.o
FORMULA_OBJ= blog_tag.o $(SHARE_OBJ)

all: $(FORMULA)

$(FORMULA): $(FORMULA_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o $@ $^ $(LINK)
clean:
	$(RM) $(FORMULA) *.o

