# cython: language_level = 3

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "tdm_termite.cpp":
  pass

cdef extern from "tdm_termite.hpp":
  cdef cppclass tdm_termite:
    # constructor(s)
    tdm_termite() except +
    tdm_termite(string tdmfile, string tdxfile) except +
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
    # print a channel(-group)
    void print_group(string id, const char* filename, bool include_meta,
                     char delimiter, string column_header) except+
    void print_channel(string id, const char* filename, bool include_meta) except+
