# cython: language_level = 3
# distutils: language = c++

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "tdm_ripper.cpp":
    pass

cdef extern from "tdm_ripper.hpp":
    cdef cppclass tdm_ripper:
        tdm_ripper(string,string) except +
        void list_channels()
        void list_groups()
        int num_channels()
        int no_channels(int)
        int num_groups()
        int no_channel_groups()
        string channel_name(int,int)
        string group_name(int)
        string channel_unit(int,int)
        int channel_exists(int,string)
        int obtain_channel_id(int,int)
        vector[double] get_channel(int)
        int channel_length(int,int)
        string time_stamp(int,bool)
        double get_min(int,int)
        double get_max(int,int)
        vector[double] channel(int,int)
        void print_channel(int,const char*)
        string get_meta(string attribute_name)
        void print_meta(const char*)
        # dummy method for compatibility
        void close()