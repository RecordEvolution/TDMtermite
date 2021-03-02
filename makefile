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

# include library path
LIB = pugixml/

# determine git version/commit tag
GTAG := $(shell git tag | tail -n1)
GHSH := $(shell git rev-parse HEAD | head -c8)

# define install location
INST := /usr/local/bin

# platform and current working directory
OST := $(shell uname)
CWD := $(shell pwd)

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

cpp-clean :
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

cython-build : cython/setup.py cython/tdm_termite.pxd cython/py_tdm_termite.pyx $(HPP) lib/tdm_termite.cpp
	python3 $< build_ext --inplace
	cp -v tdm_termite.cpython-*.so python/

cython-install : cython/setup.py cython/tdm_termite.pxd cython/py_tdm_termite.pyx $(HPP) lib/tdm_termite.cpp
	python3 $< install

cython-clean :
	rm -vf cython/py_tdm_termite.cpp
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

pip-setup:
	apt-get install -y python3-setuptools \
	                   python3-pip \
	                   python3-venv
	python3 -m pip install --upgrade build
	python3 -m pip install twine wheel auditwheel cython
	python3 -m pip install --user --upgrade twine

pip-build:
	#python3 -m build
	# python3 setup.py sdist bdist_wheel
	python3 setup.py bdist_wheel

# actually it seems we have to use CentOS container
# docker run -i -t -v `pwd`:/io quay.io/pypa/manylinux1_x86_64 /bin/bash
# see https://github.com/neuronsimulator/nrn/issues/329 for setup of the container

pip-audit:
	auditwheel repair $(shell find dist/ -name "*-cp38-cp38-linux_x86_64.whl")

# username: __token__
# password: API-token including "pypi-"
# !! RUN AS ROOT!!
pip-upload:
	python3 -m twine upload --repository testpypi dist/*

pip-test-install:
	python3 -m pip install --index-url https://test.pypi.org/simple --no-deps TDMtermite-RecordEvolution
	# python3 -m pip install -i https://test.pypi.org/simple/ TDMtermite-RecordEvolution==0.5

pip-clean:
	rm -rvf dist/
	rm -rvf *.egg-info
	rm -rvf build/ wheelhouse/

# --------------------------------------------------------------------------- #

clean : cpp-clean cython-clean pip-clean

# --------------------------------------------------------------------------- #
