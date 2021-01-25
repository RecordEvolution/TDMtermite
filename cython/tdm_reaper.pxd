# cython: language_level = 3
# distutils: language = c++

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "tdm_reaper.hpp":
    cdef cppclass tdmreaper:
        # constructor(s)
        tdmreaper() except +
        # set new file for decoding
        # void set_file(string)
        # # perform conversion (pass any C++ exceptions to Python)
        # void setup_and_conversion() except +
        # # get validity of data format
        # bool get_valid()
        # # get channel name and unit
        # string get_name()
        # string get_unit()
        # # get time step and time unit
        # double get_dt()
        # string get_temp_unit()
        # # get data array of time and measured quantity's channel
        # vector[double] get_time()
        # vector[double] get_data()
        # # dump all data to .csv
        # void write_table(const char*,char delimiter)
