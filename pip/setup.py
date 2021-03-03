# import setuptools
from setuptools import setup, Extension
# from distutils.core import setup
# from distutils.extension import Extension
# from Cython.Build import cythonize

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# extensions = Extension(
#     name="tdm_termite",
#     sources=["py_tdm_termite.pyx","py_tdm_termite.cpp"],
#     # libraries=[""],
#     # library_dirs=["lib"],
#     include_dirs=["lib","3rdparty/pugixml"],
#     language='c++',
#     extra_compile_args=['-std=c++17','-Wno-unused-variable'],
#     extra_link_args=['-std=c++17'],
# )

setup(
    name="TDMtermite-RecordEvolution",
    version="0.6.3",
    author="Record Evolution GmbH",
    author_email="mario.fink@record-evolution.de",
    maintainer="Record Evolution GmbH",
    license="MIT",
    description="Extract and read data from National Instruments LabVIEW tdx/tdm files and export them as csv files",
    keywords="TDM/TDX NationalInstruments LabVIEW decode",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/RecordEvolution/TDMtermite.git",
    project_urls={
        "Bug Tracker": "https://github.com/RecordEvolution/TDMtermite/issues",
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    # ext_modules=cythonize(extensions),
    ext_modules=[Extension("tdm_termite",
                           ["py_tdm_termite.cpp"],
                           # libraries_dirs=["cython/"],
                           # include_dirs=["3rdparty/pugixml/","lib/"],
                           # depends=["../lib/tdm_termite.hpp"]
                           language='c++',
                           extra_compile_args=['-std=c++17','-Wno-unused-variable'],
                           extra_link_args=['-std=c++17'],
                           )],
    # packages=setuptools.find_packages(),
    # python_requires=">=3.6",
)

# from distutils.core import setup, Extension
#
# moduleA = Extension('tdm_termite',
#                     sources = ['cython/py_tdm_termite.cpp'])
#
# setup (name = 'TDMtermite',
#        version = '0.5.4',
#        description="Extract and read data from National Instruments LabVIEW tdx/tdm files and export them as csv files",
#        long_description=long_description,
#        url="https://github.com/RecordEvolution/TDMtermite.git",
#        author="Record Evolution GmbH",
#        author_email="mario.fink@record-evolution.de",
#        ext_modules = [moduleA])
