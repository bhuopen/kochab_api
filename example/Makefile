ifndef TG
$(error "Please define TG (version)")
endif

INC_DIR := ../include/$(TG)

ifneq ($(INC_DIR), $(wildcard $(INC_DIR)))
$(error "No include dir for version:$(TG)")
endif

ifndef INST_DIR
$(error "Please define INST_DIR (install dir)")
endif



CC :=mips-linux-gcc
STRIP :=mips-linux-strip

CFLAGS := -I$(INC_DIR) -L../lib -DLOCK_DEBUG
LIBS := -lm -lpthread -ldml -lssl -lcrypt -liptc -lip4tc -lxtables -liconv -lcrypto -ldl

ALL_OBJ := wlanuser conf_merge conf_praser ctrl

all: clean install
	
install: $(ALL_OBJ)
	cp $(ALL_OBJ) $(INST_DIR)

clean:
	rm -f $(ALL_OBJ)

wlanuser: wlanuser.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

conf_merge: conf_merge.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

conf_praser: conf_praser.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

ctrl: ctrl.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

