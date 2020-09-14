INSTALL= install -p
INSTALL_EXEC= $(INSTALL) -m 0755
INSTALL_DATA= $(INSTALL) -m 0644

MKDIR= mkdir -p
RM= rm -f

SUPPORTED_OS= Windows Linux

ifeq ($(OS), Windows_NT)
	DETECTED_OS := Windows
else
	DETECTED_OS := $(shell uname -s)
endif

ifeq ($(DETECTED_OS), Windows)
	PROG_NAME= cflat.exe
	LIB_NAME= cflat.lib
	INC_NAME= cflat.h
	INSTALL_BIN= ../bin
	INSTALL_INC= ../include
	INSTALL_LIB= ../lib
endif

ifeq ($(DETECTED_OS), Linux)
	PROG_NAME= cflat
	LIB_NAME= cflat.a
	INC_NAME= cflat.h
	INSTALL_TOP= /usr/local
	INSTALL_BIN= $(INSTALL_TOP)/bin
	INSTALL_INC= $(INSTALL_TOP)/include
	INSTALL_LIB= $(INSTALL_TOP)/lib
endif

all: $(DETECTED_OS)

$(SUPPORTED_OS) clean:
	@echo "$(DETECTED_OS) detected..."
	@cd src && $(MAKE) $@

install: 
	cd src && $(MKDIR) $(INSTALL_BIN) $(INSTALL_INC) $(INSTALL_LIB)
	cd src && $(INSTALL_EXEC) $(PROG_NAME) $(INSTALL_BIN)
	#cd src && $(INSTALL_DATA) $(LIB_NAME) $(INSTALL_INC)
	cd src && $(INSTALL_DATA) $(LIB_NAME) $(INSTALL_LIB)

uninstall:
	cd $(INSTALL_BIN) && $(RM) $(PROG_NAME)
	cd $(INSTALL_LIB) && $(RM) $(LIB_NAME)
	#cd $(INSTALL_INC) && $(RM) $(INC_NAME)

local:
	$(MAKE) install INSTALL_TOP=../install

PHONY: all install local
