
from setuptools import setup, Extension

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="TDMtermite",
    version="1.0.1", #version,
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
    ext_modules=[Extension("tdm_termite",
                           ["py_tdm_termite.cpp"],
                           # libraries_dirs=["cython/"],
                           # include_dirs=["3rdparty/pugixml/","lib/"],
                           # depends=["../lib/tdm_termite.hpp"]
                           language='c++',
                           #extra_compile_args=['-std=c++17','-Wno-unused-variable'],
                           extra_compile_args=['-std=c++17'],
                           extra_link_args=['-std=c++17'],
                           )],
)
