
BldPath=bld
SrcPath=src
DepPath=dep
CxPath=$(DepPath)/cx

CMAKE=cmake
GODO=$(CMAKE) -E chdir
MKDIR=$(CMAKE) -E make_directory

.PHONY: default
default:
	if [ ! -d $(BldPath) ] ; then $(MAKE) cmake ; fi
	if [ ! -x bin/cx ] ; then $(MAKE) cx ; fi
	$(MAKE) proj

.PHONY: all
all:
	$(MAKE) cmake
	$(MAKE) cx
	$(MAKE) proj

.PHONY: cmake
cmake:
	if [ ! -d $(BldPath) ] ; then $(MKDIR) $(BldPath) ; fi
	$(GODO) $(BldPath) $(CMAKE) ../$(SrcPath)

.PHONY: cx
cx:
	if [ ! -d $(CxPath)/bld ] ; then $(MKDIR) $(CxPath)/bld ; fi
	$(GODO) $(CxPath)/bld $(CMAKE) ..
	$(GODO) $(CxPath)/bld $(MAKE)

.PHONY: proj
proj:
	$(GODO) $(BldPath) $(MAKE)

.PHONY: clean
clean:
	$(GODO) $(BldPath) $(MAKE) clean


