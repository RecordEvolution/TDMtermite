# cython: language_level=3
# distutils: language = c++
## distutils: sources = lib/tdm_ripper.cpp

# from libcpp.string cimport string
# from libcpp.vector cimport vector

from tdm_ripper cimport tdm_ripper

cdef class pytdmripper:
    # C++ instance
    cdef tdm_ripper cripp

    def __cinit__(self, string tdmfile, string tdxfile):
        self.cripp = tdm_ripper(tdmfile,tdxfile)

    def setfiles(self, string tdmfile, string tdxfile):
        self.cripp(tdmfile,tdxfile)
