from setuptools import Extension, setup
from Cython.Build import cythonize
import sys

print("building on platform: "+sys.platform)

cmpArgs = {
    "linux": ['-std=c++17','-Wno-unused-variable'],
    "darwin": ['-std=c++17','-Wno-unused-variable'],
    "win32": ['/EHsc','/std:c++17']
}

extension = Extension(
    "TDMtermite",
    language='c++',
    sources=["TDMtermite.pyx"],
    include_dirs=["lib","3rdparty/pugixml"],
    extra_compile_args=cmpArgs[sys.platform]
)

setup(
    ext_modules=cythonize(extension,language_level=3)
)
