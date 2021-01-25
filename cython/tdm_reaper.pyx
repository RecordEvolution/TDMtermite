
from tdm_reaper cimport tdmreaper
import numpy as np
import re
import os

cdef class tdmripper:

  # C++ instance of class => stack allocated (requires nullary constructor!)
  cdef tdm_reaper tdminst

  # constructor
  def __cinit__(self):
    self.tdminst = tdm_reaper()

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
