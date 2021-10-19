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
OPT = -O3 -Wall -Wconversion -Wpedantic -Wunused-variable -Wsign-compare

# include 3rd party libraries paths
LIBB := 3rdparty/
LIB := $(foreach dir,$(shell ls $(LIBB)),-I $(LIBB)$(dir))

# determine git version/commit tag
GTAG := $(shell git tag -l --sort=version:refname | tail -n1 | sed "s/$^v//g")
GHSH := $(shell git rev-parse HEAD | head -c8)
GVSN := $(shell cat python/VERSION | tr -d ' \n')

# current timestamp
TMS = $(shell date +%Y%m%dT%H%M%S)

# define install location
INST := /usr/local/bin

# platform and current working directory
OST := $(shell uname)
CWD := $(shell pwd)

# --------------------------------------------------------------------------- #
# C++ and CLI tool

# check tags and build executable
exec: check-tags $(GVSN) $(EXE)

# build executable
$(EXE): $(SRC).o main.o
	$(CC) $(OPT) $^ -o $@

$(SRC).o : lib/$(SRC).cpp lib/$(SRC).hpp $(HPP)
	$(CC) -c $(OPT) $(LIB) $< -o $@

# build main.cpp object file and include git version/commit tag
main.o: src/main.cpp lib/$(SRC).hpp $(HPP)
	@cp $< $<.cpp
	@if [ $(OST) = "Linux" ]; then\
		sed -i 's/TAGSTRING/$(GTAG)/g' $<.cpp; \
		sed -i 's/HASHSTRING/$(GHSH)/g' $<.cpp; \
		sed -i 's/TIMESTAMPSTRING/$(TMS)/g' $<.cpp; \
	fi
	@if [ $(OST) = "Darwin" ]; then\
		sed -i '' 's/TAGSTRING/$(GTAG)/g' $<.cpp; \
		sed -i '' 's/HASHSTRING/$(GHSH)/g' $<.cpp; \
		sed -i '' 's/TIMESTAMPSTRING/$(TMS)/g' $<.cpp; \
	fi
	$(CC) -c $(OPT) $(LIB) -I lib/ $<.cpp -o $@
	@rm $<.cpp

install: $(EXE)
	cp $< $(INST)/

uninstall : $(INST)/$(EXE)
	rm $<

tdmtest : tdmtest.o
	$(CC) $(OPT) $^ -o $@

tdmtest.o : src/test.cpp lib/$(SRC).hpp $(HPP)
	$(CC) -c $(OPT) $(LIB) -I lib/ $< -o $@

cpp-clean :
	rm -vf $(EXE)
	rm -vf *.o src/main.cpp.cpp tdmtest

#-----------------------------------------------------------------------------#
# versions

$(GTAG):
	@echo "consistent versions check successful: building $(GTAG)"

check-tags:
	@echo "latest git tag:  $(GTAG)"
	@echo "latest git hash: $(GHSH)"
	@echo "python version:  $(GVSN)"

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
# check process

checkps :
	@ps aux | head -n1
	@ps aux | grep $(EXE) | grep -v "grep"

# --------------------------------------------------------------------------- #
# python

python-build: check-tags $(GVSN)
	make -C python/ build-inplace
	cp python/TDMtermite*.so ./ -v

python-install: check-tags $(GVSN)
	make -C python/ install

python-clean:
	make -C python/ clean
	rm -vf TDMtermite*.so

python-test:
	PYTHONPATH=./ python python/examples/usage.py

# --------------------------------------------------------------------------- #
# clean

clean : cpp-clean python-clean

# --------------------------------------------------------------------------- #
