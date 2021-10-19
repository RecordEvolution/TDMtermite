# distutils: language = c++
# cython: language_level = 3

from TDMtermite cimport cpptdmtermite

import json as jn

cdef class tdmtermite:

  # C++ instance of class => stack allocated (requires nullary constructor!)
  cdef cpptdmtermite cpptdm

  # constructor
  def __cinit__(self, string tdmfile, string tdxfile):
    self.cpptdm = cpptdmtermite(tdmfile,tdxfile)

  # provide TDM files
  def submit_files(self,string tdmfile, string tdxfile):
    self.cpptdm.submit_files(tdmfile,tdxfile)

  # get list of channel(-group) ids
  def get_channelgroup_ids(self):
    return self.cpptdm.get_channelgroup_ids()
  def get_channel_ids(self):
    return self.cpptdm.get_channel_ids()

  # get data of specific channel
  def get_channel(self, string id):
    return self.cpptdm.get_channel_as_double(id)

  # get meta-data of channel(-group) (as dictionary)
  def get_channelgroup_info(self, string id):
    grpstr = self.cpptdm.get_channelgroup_info(id)
    return jn.loads(grpstr.decode())
  def get_channel_info(self, string id):
    chnstr = self.cpptdm.get_channel_info(id)
    return jn.loads(chnstr.decode())

  # print a channel(-group)
  def print_channelgroup(self, string id, const char* filename, bool include_meta,
                               char delimiter, string column_header):
    self.cpptdm.print_group(id,filename,include_meta,delimiter,column_header)
  def print_channel(self, string id, const char* filename,
                          bool include_meta):
    self.cpptdm.print_channel(id,filename,include_meta)

  # print all data grouped by channelgroups
  def write_all(self, string outputdir) :
    grpids = self.cpptdm.get_channelgroup_ids()
    for id in grpids :
      grpfile = outputdir.decode() + "/channelgroup_" + id.decode() + ".csv"
      self.cpptdm.print_group(id,grpfile.encode(),True,ord(','),"".encode())
