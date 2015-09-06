#
# makefile for libbaalue
#

all::
	@echo "+----------------------------------------------------------+"
	@echo "| make setup_build -> setup build env and configure with   |"
	@echo "|                     defaults                             |"
	@echo "| make build       -> cd _build && make                    |"
	@echo "| make install     -> cd _build && make install            |"
	@echo "+----------------------------------------------------------+"	

setup_build::
	mkdir -p _build
	(cd _build && cmake .. -DCMAKE_INSTALL_PREFIX=${HOME})
	@echo ""
	@echo "+----------------------------------------------------------+"
	@echo "| ready to build -> cd _build && make && make install      |"
	@echo "+----------------------------------------------------------+"	

build::
	(cd _build && make VERBOSE=1)

install::
	(cd _build && make install)

clean::
	rm -f *~
	rm -f .*~
	rm -rf build
	rm -rf _build
	rm -f cmake/modules/*~
	rm -f cmake/modules/.*~
	rm -f man/*~
	rm -f man/.*~

distclean: clean
