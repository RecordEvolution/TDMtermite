
import tdm_reaper
import numpy as np
import json

# create 'tdm_reaper' instance object
try :
    jack = tdm_reaper.tdmreaper(b'samples/SineData.tdm',b'samples/SineData.tdx')
except RuntimeError as e:
    print("failed to load/decode TDM files: " + str(e))

# list ids of channelgroups
grpids = jack.get_channelgroup_ids()
grpids = [x.decode() for x in grpids]
print("list of channelgroups: ",grpids)

# obtain meta data of channelgroups
for grp in grpids[0:2] :
    grpinfostr = jack.get_channelgroup_info(grp.encode())
    grpinfostr = grpinfostr.decode()
    grpinfo = json.loads(grpinfostr)
    print( json.dumps(grpinfo,sort_keys=False,indent=4) )

# list ids of channels
chnids = jack.get_channel_ids()
chnids = [x.decode() for x in chnids]
print("list of channels: ",chnids)

# get (meta-)data of channels
for chn in chnids[0:3] :

    # obtain meta-data
    chninfostr = jack.get_channel_info(chn.encode())
    chninfostr = chninfostr.decode()
    chninfo = json.loads(chninfostr)
    print( json.dumps(chninfo,sort_keys=False,indent=4) )

    # channel data
    chndata = jack.get_channel(chn.encode())
    print(str(chndata[0:6]) + " ...")
