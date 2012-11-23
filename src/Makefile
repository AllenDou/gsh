LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC=gcc
CFLAGS= -O0 -g -rdynamic -ggdb
WALL= -Wall

LINK= -ldl -lm
TARGET=../bin
GSHSERVER=gsh-server
QUIET_LINK = @printf ' OMG!!! %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR);

OBJ= ae.o ae_epoll.o anet.o command.o config.o db.o debug.o dict.o gsh.o networking.o object.o common/adlist.o common/cJSON.o common/sds.o common/util.o common/zmalloc.o

all: $(GSHSERVER)

$(GSHSERVER): $(OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o $(TARGET)/$@ $^ $(LINK)
clean:
	rm -rf *.o $(TARGET)/$(GSHSERVER) 

