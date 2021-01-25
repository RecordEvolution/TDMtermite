from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = Extension(
    name="tdm_reaper",
    sources=["cython/tdm_reaper.pyx"],
    # libraries=[""],
    library_dirs=["lib"],
    include_dirs=["lib"],
    language='c++',
    extra_compile_args=['-std=c++17','-Wno-unused-variable'],
    extra_link_args=['-std=c++17'],
)

setup(
    name="tdm_reaper",
    ext_modules=cythonize(extensions)
)
