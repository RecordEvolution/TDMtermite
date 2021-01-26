# cython: language_level = 3

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "tdm_reaper.cpp":
  pass

cdef extern from "tdm_reaper.hpp":
  cdef cppclass tdm_reaper:
    # constructor(s)
    tdm_reaper() except +
    tdm_reaper(string tdmfile, string tdxfile) except +
    # provide TDM files
    void submit_files(string tdmfile, string tdxfile) except+
    # get list of channel(-group) ids
    vector[string] get_channelgroup_ids() except+
    vector[string] get_channel_ids() except+
    # get data of specific channel
    vector[double] get_channel_as_double(string id) except+
    # get meta-data
    string get_channelgroup_info(string id) except+
    string get_channel_info(string id) except+

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
