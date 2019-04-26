# cython: language_level = 3
# distutils: language = c++

## distutils: sources = lib/tdm_ripper.cpp

from tdm_ripper cimport tdm_ripper

cdef class pytdmripper:

    # use pointer since there's no nullary constructor
    cdef tdm_ripper*cripp

    def __cinit__(self, string tdmfile):
        self.cripp = new tdm_ripper(tdmfile)

    def __dealloc__(self):
        del self.cripp

    # def setfiles(self, string tdmfile, string tdxfile):
    #     self.cripp(tdmfile,tdxfile)
