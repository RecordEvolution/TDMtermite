from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = Extension(
    name="tdm_ripper",
    sources=["pytdm_ripper.pyx"],
    # libraries=[""],
    library_dirs=["lib"],
    include_dirs=["lib"],
    language='c++',
    extra_compile_args=['-stdlib=libc++','-std=c++11','-Wno-unused-variable'],
    extra_link_args=['-stdlib=libc++','-std=c++11'],
)

setup(
    name="tdm_ripper",
    ext_modules=cythonize(extensions)
)
