ifndef TG
$(error "Please define TG (version)")
endif

INC_DIR := ./include/$(TG)

ifneq ($(INC_DIR), $(wildcard $(INC_DIR)))
$(error "No include dir for version:$(TG)")
endif

ifndef IMG
$(error "Please define IMG (firmware)")
endif

ifneq ($(IMG), $(wildcard $(IMG)))
$(error "No firmware found:$(IMG)")
endif


PFDIR=$(shell pwd)
IMGFILE=$(PFDIR)/$(IMG)
BUILDDIR=$(PFDIR)/build
FW_PART_DIR=$(BUILDDIR)/$(IMG).part
ROOTFS=$(FW_PART_DIR)/squashfs-root
INST_DIR=$(ROOTFS)/usr/sbin
TOOLS_DIR=$(PFDIR)/tools

all: compile mkroot

clean:
	@rm -rf build/*
	make -C example TG=$(TG) INST_DIR=$(INST_DIR) clean

extract: clean
	@cd $(BUILDDIR);\
	$(TOOLS_DIR)/fwextract $(IMGFILE);
	cd $(FW_PART_DIR);\
	$(TOOLS_DIR)/unsquashfs rootfs;
	-@cp $(ROOTFS)/lib/lib* lib/
	-@cp $(ROOTFS)/usr/lib/lib* lib/

compile:
	make -C example TG=$(TG) INST_DIR=$(INST_DIR)

mkroot:
	@cd $(FW_PART_DIR);\
	$(TOOLS_DIR)/mksquashfs squashfs-root rootfs.new -processors 1 -all-root 



