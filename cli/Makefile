LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC=gcc
CFLAGS=-Wall -g3 -O0
LINK= -ldl -lm
bin = bin

PREDICT_SUB=predict_sub
PREDICT=predict
CACHE=cache
FMLOAD=fmload
CARSVM=carsvm
BC=bc
QUIET_LINK = @printf ' %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR);

SHARE_OBJ=lib/cJSON.o lib/hiredis.o lib/net.o lib/sds.o

PREDICT_SUB_OBJ=predict_sub.o $(SHARE_OBJ)
PREDICT_OBJ=predict.o $(SHARE_OBJ)
CACHE_OBJ=cache.o $(SHARE_OBJ)
FMLOAD_OBJ=fmload.o $(SHARE_OBJ)
CARSVM_OBJ=carsvm.o $(SHARE_OBJ)
BC_OBJ=bc.o  $(SHARE_OBJ)

all:$(PREDICT_SUB) $(PREDICT) $(FMLOAD) $(CARSVM) $(BC)

$(PREDICT_SUB):$(PREDICT_SUB_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(PREDICT):$(PREDICT_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(FMLOAD):$(FMLOAD_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(CARSVM):$(CARSVM_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(BC):$(BC_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
clean:
	rm -rf $(bin)/* *.o lib/*.o
