# --------------------------------------------------------------------------- #

# declare name of executable
EXE = tdmreaper

# source name
SRC := tdm_reaper

# compiler and C++ standard
CC = g++ -std=c++17

# compiler options and optimization flags
OPT = -O3 -Wall -Werror -Wunused-variable -Wsign-compare

# include library path
LIB = pugixml/

# determine git version/commit tag
GTAG := $(shell git tag | head -n1)
GHSH := $(shell git rev-parse HEAD | head -c8)

# define install location
INST := /usr/local/bin

# --------------------------------------------------------------------------- #
# CLI tool

$(EXE) : main.o $(SRC).o
	$(CC) $(OPT) $^ -o $@

install : $(EXE)
	cp $< $(INST)/

uninstall : $(INST)/$(EXE)
	rm $<

# build main.cpp object file and include git version/commit tag
main.o : src/main.cpp
	@cp $< $<.cpp
	@sed -i 's/TAGSTRING/$(GTAG)/g' $<.cpp
	@sed -i 's/HASHSTRING/$(GHSH)/g' $<.cpp
	$(CC) -c $(OPT) -I $(LIB) -I lib/ $<.cpp -o $@
	@rm $<.cpp

$(SRC).o : lib/$(SRC).cpp lib/$(SRC).hpp lib/tdm_datamodel.hpp
	$(CC) -c $(OPT) -I $(LIB) $< -o $@

clean :
	rm -f $(EXE) *.o

# --------------------------------------------------------------------------- #
# python module
#
# pylib : setup.py pytdm_ripper.pyx tdm_ripper.pxd tdm_ripper.o
# 	python3 setup.py build_ext --inplace
#
# install : setup.py pytdm_ripper.pyx tdm_ripper.pxd lib/tdm_ripper.cpp lib/tdm_ripper.hpp
# 	python3 setup.py install
#
# install_osx : setup_osx.py pytdm_ripper.pyx tdm_ripper.pxd lib/tdm_ripper.cpp lib/tdm_ripper.hpp
# 	python3 setup_osx.py install
#
# lib/libtdmripper.a :
# 	make -C lib libtdmripper.a
#
# clean-lib :
# 	rm -f lib/*.o lib/*.a
# 	rm -f -r build
# 	rm -f pytdm_ripper.cpp
# 	rm -f *.so


# --------------------------------------------------------------------------- #
