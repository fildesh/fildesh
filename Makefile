
BldPath=bld

ScanBldPath=clang
ScanRptPath=$(ScanBldPath)/report
SCAN_BUILD=scan-build -o $(PWD)/$(ScanRptPath)

CMakeExe=cmake
CMAKE=$(CMakeExe)
GODO=$(CMakeExe) -E chdir
MKDIR=$(CMakeExe) -E make_directory
CTAGS=ctags

.PHONY: default all cmake proj \
	test analyze tags \
	clean-analyze clean distclean \
	update pull

default:
	if [ ! -d $(BldPath) ] ; then $(MAKE) cmake ; fi
	$(MAKE) proj

all:
	$(MAKE) cmake
	$(MAKE) proj

cmake:
	if [ ! -d $(BldPath) ] ; then $(MKDIR) $(BldPath) ; fi
	$(GODO) $(BldPath) $(CMAKE) -D CMAKE_BUILD_TYPE=RelOnHost ..

proj:
	$(GODO) $(BldPath) $(MAKE)

test:
	$(GODO) $(BldPath) $(MAKE) test

analyze:
	rm -fr $(ScanRptPath)
	$(MAKE) 'BldPath=$(ScanBldPath)' 'CMAKE=$(SCAN_BUILD) cmake' 'MAKE=$(SCAN_BUILD) make'

tags:
	$(CTAGS) -R src

clean-analyze:
	rm -fr $(ScanBldPath)

clean:
	$(GODO) $(BldPath) $(MAKE) clean

distclean:
	rm -fr $(BldPath) $(ScanBldPath) tags

update:
	git pull origin trunk

pull:
	git pull origin trunk

