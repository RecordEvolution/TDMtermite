from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
import os

extensions = Extension(
    name="tdm_termite",
    sources=["cython/py_tdm_termite.pyx"],
    # libraries=[""],
    # library_dirs=["lib"],
    include_dirs=["lib","3rdparty/pugixml"],
    language='c++',
    extra_compile_args=['-std=c++17','-Wno-unused-variable'],
    extra_link_args=['-std=c++17'],
)

os.system("git tag > gittags.log")
with open ("gittags.log","r") as gt:
    taglst = gt.readlines()
os.remove("gittags.log")
version = taglst[-1].replace('\n','').replace('v','')
print("building version: "+version)

setup(
    version=version,
    description='TDMtermite cython extension',
    author='Record Evolution GmbH',
    author_email='mario.fink@record-evolution.de',
    url='https://github.com/RecordEvolution/TDMtermite.git',
    name="tdm_termite",
    ext_modules=cythonize(extensions)
)
