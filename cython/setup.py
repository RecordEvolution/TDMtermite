from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = Extension(
    name="tdm_reaper",
    sources=["cython/tdm_reaper.pyx"],
    # libraries=[""],
    # library_dirs=["lib"],
    include_dirs=["lib","pugixml"],
    language='c++',
    extra_compile_args=['-std=c++17','-Wno-unused-variable'],
    extra_link_args=['-std=c++17'],
)

setup(
    version='0.1',
    description='TDMReaper cython extension',
    author='Record Evolution GmbH',
    author_email='mario.fink@record-evolution.de',
    url='https://github.com/RecordEvolution/tdm_ripper.git',
    name="tdm_reaper",
    ext_modules=cythonize(extensions)
)
