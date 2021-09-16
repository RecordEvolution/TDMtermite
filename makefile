# --------------------------------------------------------------------------- #

SHELL := /bin/bash

# declare name of executable
EXE = tdmtermite

# sources and headers
SRC := tdm_termite
HPP = $(wildcard lib/*.hpp)

# compiler and C++ standard
CC = g++ -std=c++17

# compiler options and optimization flags
OPT = -O3 -Wall -Werror -Wunused-variable -Wsign-compare

# include 3rd party libraries paths
LIBB := 3rdparty/
LIB := $(foreach dir,$(shell ls $(LIBB)),-I $(LIBB)$(dir))

# determine git version/commit tag
GTAG := $(shell git tag | tail -n1)
GTAGV := $(shell git tag | tail -n1 | tr -d 'v')
GHSH := $(shell git rev-parse HEAD | head -c8)
PIPYVS := $(shell grep "version" pip/setup.py | tr -d 'version=,\#\" ')
PICGVS := $(shell grep "version" pip/setup.cfg | tr -d 'version=,\#\" ')

# current timestamp
TMS = $(shell date +%Y%m%dT%H%M%S)

# define install location
INST := /usr/local/bin

# platform and current working directory
OST := $(shell uname)
CWD := $(shell pwd)

# --------------------------------------------------------------------------- #
# version/tag check

checkversion:
	@echo "git tag:                "$(GTAG)
	@echo "git head:               "$(GHSH)
	@echo "pip setup.py version:   "$(PIPYVS)
	@echo "pip setup.cfg version:  "$(PICGVS)

$(GTAGV):
	@echo "check consistent versions (git tag vs. setup.py): "$(GTAG)" <-> "$(PIPYVS)" "

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
		sed -i 's/TIMESTAMPSTRING/$(TMS)/g' $<.cpp; \
	fi
	@if [ $(OST) = "Darwin" ]; then\
		sed -i '' 's/TAGSTRING/$(GTAG)/g' $<.cpp; \
		sed -i '' 's/HASHSTRING/$(GHSH)/g' $<.cpp; \
	fi
	$(CC) -c $(OPT) $(LIB) -I lib/ $<.cpp -o $@
	@rm $<.cpp

$(SRC).o : lib/$(SRC).cpp lib/$(SRC).hpp $(HPP)
	$(CC) -c $(OPT) $(LIB) $< -o $@

tdmtest : tdmtest.o
	$(CC) $(OPT) $^ -o $@

tdmtest.o : src/test.cpp lib/$(SRC).hpp $(HPP)
	$(CC) -c $(OPT) $(LIB) -I lib/ $< -o $@

cpp-clean :
	rm -f $(EXE) *.o src/main.cpp.cpp tdmtest

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

cython-build : cython/setup.py cython/tdm_termite.pxd cython/py_tdm_termite.pyx $(HPP) lib/tdm_termite.cpp
	python3 $< build_ext --inplace
	cp -v tdm_termite.cpython-*.so python/

cython-install : cython/setup.py cython/tdm_termite.pxd cython/py_tdm_termite.pyx $(HPP) lib/tdm_termite.cpp
	python3 $< install

cython-clean :
	rm -vf cython/py_tdm_termite.c* cython/tdm_termite.c*
	rm -vf tdm_termite.cpython-*.so python/tdm_termite.cpython-*.so
	rm -rf build

# --------------------------------------------------------------------------- #
# docker

docker-build:
	docker build . --tag tdmtermite:latest

docker-run:
	mkdir -pv data/{input,output}
	docker run -it --rm --volume $(CWD)/data:/home/data tdmtermite:latest /bin/bash

docker-clean:
	rm -rv data
	docker image remove tdmtermite

# --------------------------------------------------------------------------- #
# pip

pip-publish: $(PIPYVS) cython-build
	cd pip/ && make pip-publish

pip-test:
	cd pip/ && make pip-test

# --------------------------------------------------------------------------- #

clean : cpp-clean cython-clean
	cd pip/ && make pip-clean

# --------------------------------------------------------------------------- #
