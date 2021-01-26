# --------------------------------------------------------------------------- #

# declare name of executable
EXE = tdmreaper

# sources and headers
SRC := tdm_reaper
HPP = $(wildcard lib/*.hpp)

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

# platform
OST := $(shell uname)

# --------------------------------------------------------------------------- #
# CLI tool

$(EXE) : main.o $(SRC).o
	$(CC) $(OPT) $^ -o $@

install : $(EXE)
	cp $< $(INST)/

uninstall : $(INST)/$(EXE)
	rm $<

# build main.cpp object file and include git version/commit tag
main.o : src/main.cpp lib/$(SRC).hpp $(HPP)
	@cp $< $<.cpp
	@if [ $(OST) = "Linux" ]; then\
		sed -i 's/TAGSTRING/$(GTAG)/g' $<.cpp; \
		sed -i 's/HASHSTRING/$(GHSH)/g' $<.cpp; \
	fi
	@if [ $(OST) = "Darwin" ]; then\
		sed -i '' 's/TAGSTRING/$(GTAG)/g' $<.cpp; \
		sed -i '' 's/HASHSTRING/$(GHSH)/g' $<.cpp; \
	fi
	$(CC) -c $(OPT) -I $(LIB) -I lib/ $<.cpp -o $@
	@rm $<.cpp

$(SRC).o : lib/$(SRC).cpp lib/$(SRC).hpp $(HPP)
	$(CC) -c $(OPT) -I $(LIB) $< -o $@

clean-cpp :
	rm -f $(EXE) *.o src/main.cpp.cpp

# --------------------------------------------------------------------------- #
# check process

checkps :
	@ps aux | head -n1
	@ps aux | grep $(EXE) | grep -v "grep"

# --------------------------------------------------------------------------- #
# python/cython module

cython-requirements: cython/requirements.txt
	python3 -m pip install -r $<

cython-help : cython/setup.py
	python3 $< --help

cython-list : cython/setup.py
	python3 $< --name --description --author --author-email --url

cython-build : cython/setup.py cython/tdm_reaper.pxd cython/py_tdm_reaper.pyx $(HPP) lib/tdm_reaper.cpp
	python3 $< build_ext --inplace
	cp -v tdm_reaper.cpython-*.so python/

cython-install : cython/setup.py cython/tdm_reaper.pyx cython/tdm_reaper.pyx
	python3 $< install

clean-cython :
	rm -vf cython/py_tdm_reaper.cpp
	rm -vf tdm_reaper.cpython-*.so python/tdm_reaper.cpython-*.so
	rm -rf build

# --------------------------------------------------------------------------- #

clean : clean-cpp clean-cython

# --------------------------------------------------------------------------- #
