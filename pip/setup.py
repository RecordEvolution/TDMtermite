import setuptools
from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

extensions = Extension(
    name="tdm_termite",
    sources=["cython/py_tdm_termite.pyx"],
    # libraries=[""],
    # library_dirs=["lib"],
    include_dirs=["lib","pugixml"],
    language='c++',
    extra_compile_args=['-std=c++17','-Wno-unused-variable'],
    extra_link_args=['-std=c++17'],
)

setuptools.setup(
    name="TDMtermite-RecordEvolution",
    version="0.5",
    author="Record Evolution GmbH",
    author_email="mario.fink@record-evolution.de",
    description="Extract and read data from National Instruments LabVIEW tdx/tdm files and export them as csv files",
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
    ext_modules=cythonize(extensions),
    packages=setuptools.find_packages(),
    python_requires=">=3.6",
)
