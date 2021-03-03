# --------------------------------------------------------------------------- #

pip-publish: pip-sdist pip-upload pip-clean

pip-sdist: ../cython/py_tdm_termite.pyx ../cython/tdm_termite.pxd ../cython/py_tdm_termite.cpp
	cp -v ../cython/py_tdm_termite.pyx ../cython/tdm_termite.pxd ./
	cp -v ../cython/py_tdm_termite.cpp ./
	cp -v ../lib/*.hpp ../lib/*.cpp ./
	cp -v ../3rdparty/pugixml/* ./
	cp -v ../README.md ./
	# cython py_tdm_termite.pyx -o py_tdm_termite.cpp
	python3 setup.py sdist

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
# !! RUN AS ROOT!! (only for bdist_wheel + auditwheel)
pip-upload:
	python3 -m twine upload --repository testpypi dist/$(shell ls -t dist/ | head -n1)

pip-test-install:
	python3 -m pip install --index-url https://test.pypi.org/simple --no-deps TDMtermite-RecordEvolution
	# python3 -m pip install -i https://test.pypi.org/simple/ TDMtermite-RecordEvolution==0.5

pip-test:
	docker build . --tag tdmtermite-piptest
	docker run -it --rm tdmtermite-piptest

pip-clean:
	rm -rvf dist/
	rm -rvf *.egg-info
	rm -rvf build/
	rm -rvf cython/
	rm -rvf wheelhouse/
	rm -vf *.pyx *.pxd
	rm -vf *.cpp *.c *.hpp
	rm -vf README.md

# --------------------------------------------------------------------------- #
