
setup:
	cat ../README.md | grep '^# TDMtermite' -A 50000 > ./README.md
	#pandoc -f markdown -t rst -o README.rst README.md
	#python -m rstvalidator README.rst
	cp -r ../lib ./
	cp -r ../3rdparty ./
	cp -v ../LICENSE ./

setup-clean:
	rm -vf README.md README.rst LICENSE
	rm -rf lib/ 3rdparty/

build: setup
	python setup.py build

build-inplace: setup
	python setup.py build_ext --inplace

build-install: setup
	python setup.py install

build-sdist: setup
	python setup.py sdist
	python -m twine check dist/*

build-bdist: setup
	python setup.py bdist
	python -m twine check dist/*

build-clean:
	python setup.py clean --all
	rm -vf TDMtermite*.so TDMtermite*.cpp
	rm -rvf dist/ IMCtermite.egg-info/

cibuildwheel-build: setup
	cibuildwheel --platform linux

cibuildwheel-clean:
	rm -rvf wheelhouse/

pypi-audit:
	auditwheel repair $(shell find dist/ -name "*-linux_x86_64.whl")

# username: __token__
# password: API-token including "pypi-"
pypi-upload-test:
	python -m twine upload --repository testpypi dist/$(shell ls -t dist/ | head -n1)

pypi-install-test:
	python -m pip install --index-url https://test.pypi.org/simple --no-deps TDMtermite-RecordEvolution
	# python3 -m pip install -i https://test.pypi.org/simple/ TDMtermite-RecordEvolution==0.5

pypi-upload:
	python -m twine upload dist/$(shell ls -t dist/ | head -n1)

clean: setup build-clean cibuildwheel-clean setup-clean

run-example:
	PYTHONPATH=$(pwd) python examples/usage.py
