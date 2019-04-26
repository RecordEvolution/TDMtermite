# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "tdm_ripper.cpp":
    pass

cdef extern from "tdm_ripper.hpp":
    cdef cppclass tdm_ripper:
        tdm_ripper(string) except +
        void show_channels()
        int num_channels()
        int num_groups()
        vector[double] get_channel(int)
        void print_channel(int,const char*)
