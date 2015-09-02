#
# makefile for libbaalue
#

all::
	@echo "+----------------------------------------------------------+"
	@echo "| make setup_build -> setup build env and configure with   |"
	@echo "|                     defaults                             |"
	@echo "+----------------------------------------------------------+"	

setup_build::
	mkdir -p _build
	(cd _build && cmake .. -DCMAKE_INSTALL_PREFIX=${HOME})
	@echo ""
	@echo "+----------------------------------------------------------+"
	@echo "| ready to build -> cd _build && make && make install      |"
	@echo "+----------------------------------------------------------+"	


clean::
	rm -f *~
	rm -f .*~
	rm -rf build
	rm -rf _build

distclean: clean
