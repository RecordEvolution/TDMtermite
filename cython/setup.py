from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
import os
import sys

print("building on platform: "+sys.platform)
os.system("git tag > gittags.log")
with open ("gittags.log","r") as gt:
    taglst = gt.readlines()
os.remove("gittags.log")
if len(taglst) > 0 :
    version = taglst[-1].replace('\n','').replace('v','')
else:
    version = 'unkown'
print("building version: "+version)

if sys.platform == "linux" or sys.platform == "darwin" :
	cmpargs = ['-std=c++17','-Wno-unused-variable']
	lnkargs = ['-std=c++17','-Wno-unused-variable']
elif sys.platform == "win32" :
	cmpargs = ['/EHsc','/std:c++17']
	lnkargs = ['/EHsc','/std:c++17']
else :
	raise RuntimeError("unknown platform") 

extensions = Extension(
    name="tdm_termite",
    sources=["cython/py_tdm_termite.pyx"],
    # libraries=[""],
    # library_dirs=["lib"],
    include_dirs=["lib","3rdparty/pugixml"],
    language='c++',
    #extra_compile_args=['-std=c++17','-Wno-unused-variable'],
    extra_compile_args= cmpargs,
    extra_link_args= lnkargs,
)

setup(
    version=version,
    description='TDMtermite cython extension',
    author='Record Evolution GmbH',
    author_email='mario.fink@record-evolution.de',
    url='https://github.com/RecordEvolution/TDMtermite.git',
    name="tdm_termite",
    ext_modules=cythonize(extensions)
)
