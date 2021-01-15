# --------------------------------------------------------------------------- #

# declare name of executable
EXE = tdmripper

# compiler and C++ standard
CC = g++ -std=c++11

# compiler options and optimization flags
OPT = -O3 -Wall -Werror -Wunused-variable -Wsign-compare

# include library path
LIB = pugixml/

# determine git version/commit tag
TAGV := $(shell git describe --tag | head -n1)
COMM := $(shell git rev-parse HEAD | head -c8)
GITV := $(TAGV)-g$(COMM)

# --------------------------------------------------------------------------- #
# CLI tool

$(EXE) : main.o tdm_ripper.o
	$(CC) $(OPT) $^ -o $@

# build main.cpp object file and include git version/commit tag
main.o : src/main.cpp
	@cp $< $<.cpp
	@sed -i 's/tagfullstring/$(GITV)/g' $<.cpp
	@sed -i 's/versionstring/$(TAGV)/g' $<.cpp
	$(CC) -c $(OPT) -I $(LIB) -I lib/ $<.cpp -o $@
	@rm $<.cpp

tdm_ripper.o : lib/tdm_ripper.cpp lib/tdm_ripper.hpp
	$(CC) -c $(OPT) -I $(LIB) $< -o $@

extall : extract_all.o tdm_ripper.o
	$(CC) $(OPT) $^ -o extract_all

extract_all.o : extract_all.cpp
	$(CC) -c $(OPT) -I $(LIB) $< -o $@

clean :
	rm -f $(EXE) *.o
	rm -f *.dat
	rm -f extract_all
	rm -f data/*.dat
	rm -f data/*.csv

# --------------------------------------------------------------------------- #
# python module

pylib : setup.py pytdm_ripper.pyx tdm_ripper.pxd tdm_ripper.o
	python3 setup.py build_ext --inplace

install : setup.py pytdm_ripper.pyx tdm_ripper.pxd lib/tdm_ripper.cpp lib/tdm_ripper.hpp
	python3 setup.py install

install_osx : setup_osx.py pytdm_ripper.pyx tdm_ripper.pxd lib/tdm_ripper.cpp lib/tdm_ripper.hpp
	python3 setup_osx.py install

lib/libtdmripper.a :
	make -C lib libtdmripper.a

clean-lib :
	rm -f lib/*.o lib/*.a
	rm -f -r build
	rm -f pytdm_ripper.cpp
	rm -f *.so
