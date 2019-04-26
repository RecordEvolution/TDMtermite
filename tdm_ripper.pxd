# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector

# use only in absence of # distutils: sources = lib/tdm_ripper.cpp in .pyx
cdef extern from "tdm_ripper.cpp":
    pass

cdef extern from "tdm_ripper.hpp":
    cdef cppclass tdm_ripper:
    #    tdm_ripper() except +
        tdm_ripper(string, string) except +
        # void setfiles(string, string)
        # void setup()
