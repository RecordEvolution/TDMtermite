# distutils: language = c++

from tdm_reaper cimport tdm_reaper
import json as jsn
import numpy as np
# import re
# import os

cdef class tdmreaper:

  # C++ instance of class => stack allocated (requires nullary constructor!)
  cdef tdm_reaper cpp_tdm

  # constructor
  def __cinit__(self, string tdmfile, string tdxfile):
    self.cpp_tdm = tdm_reaper(tdmfile,tdxfile)

  # provide TDM files
  def submit_files(self,string tdmfile, string tdxfile):
    self.cpp_tdm.submit_files(tdmfile,tdxfile)

  # get list of channel(-group) ids
  def get_channelgroup_ids(self):
    return self.cpp_tdm.get_channelgroup_ids()
  def get_channel_ids(self):
    return self.cpp_tdm.get_channel_ids()

  # get data of specific channel
  def get_channel(self, string id):
    return self.cpp_tdm.get_channel_as_double(id)

  # get meta-data of channel(-group)
  def get_channelgroup_info(self, string id):
    return self.cpp_tdm.get_channelgroup_info(id)
  def get_channel_info(self, string id):
    return self.cpp_tdm.get_channel_info(id)

  # def set_file(self, string rawfile):
  #   if not os.path.isfile(rawfile) :
  #     raise ValueError("'" + str(rawfile) + "' does not exist")
  #   self.rawit.set_file(rawfile)
  #
  # def do_conversion(self):
  #     self.rawit.setup_and_conversion()
  #
  # def validity(self):
  #   return self.rawit.get_valid()
  #
  # def channel_name(self):
  #   return self.rawit.get_name()
  #
  # def unit(self):
  #   return self.rawit.get_unit()
  #
  # def dt(self):
  #   return self.rawit.get_dt()
  #
  # def time_unit(self):
  #   return self.rawit.get_temp_unit()
  #
  # def get_time(self):
  #   return self.rawit.get_time()
  #
  # def get_channel(self):
  #   return self.rawit.get_data()
  #
  # def write_table(self, const char* csvfile, char delimiter):
  #   self.rawit.write_table(csvfile,delimiter)
