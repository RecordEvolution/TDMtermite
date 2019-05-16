
from tdm_ripper cimport tdm_ripper
import numpy as np

cdef class pytdmripper:

    # pointer to C++ instance (since there's no nullary constructor)
    cdef tdm_ripper *cripp

    def __cinit__(self, string tdmfile):
        self.cripp = new tdm_ripper(tdmfile)

    def __dealloc__(self):
        del self.cripp

    def show_channels(self):
        self.cripp.list_channels()

    def show_groups(self):
      self.cripp.list_groups()

    def num_channels(self):
        return self.cripp.num_channels()

    def no_channels(self, int groupid):
        assert (groupid >= 0 and groupid < self.cripp.num_groups()), "index of group must be in [0,n-1]"
        return self.cripp.no_channels(groupid)

    def num_groups(self):
        return self.cripp.num_groups()

    def no_channel_groups(self):
        return self.cripp.num_groups()

    def channel_name(self,int groupid,int channelid):
        return self.cripp.channel_name(groupid,channelid).decode('utf-8')

    def channel_unit(self,int groupid,int channelid):
        return (self.cripp.channel_unit(groupid,channelid))

    def channel_exists(self,int groupid, string channelname):
        return self.cripp.channel_exists(groupid,channelname)

    def get_channel(self, int channelid):
        return np.asarray(self.cripp.get_channel(channelid))

    def channel(self,int groupid,int channelid):
        return self.cripp.channel(groupid,channelid)

    def channel_length(self,int groupid, int channelid):
        return self.cripp.channel_length(groupid,channelid)

    def time_stamp(self,int groupid, bool startstop):
        return self.cripp.time_stamp(groupid,startstop)

    def get_min(self,int groupid, int channelid):
        return self.cripp.get_min(groupid,channelid)

    def get_max(self,int groupid, int channelid):
        return self.cripp.get_max(groupid,channelid)

    def print_channel(self, int channelid, const char* filename):
        self.cripp.print_channel(channelid,filename)

    def close(self):
        dummy = ""

    # integrate into CONTI_HBS workflow by adding methods: (29.04.2019)
    #   tdm_loader.OpenFile(tmp_path + name_prefix + '/Messung.tdm', encoding='utf-8')
    #   .no_channel_groups()
    #   .no_channels(i)
    #   .channel_name(i, j)
    #   .channel(i, j)
    #   .close()
