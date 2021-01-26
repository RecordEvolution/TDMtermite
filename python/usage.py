
import tdm_reaper
# import numpy as np
import json
import re

# create 'tdm_reaper' instance object
try :
    jack = tdm_reaper.tdmreaper(b'samples/SineData.tdm',b'samples/SineData.tdx')
except RuntimeError as e :
    print("failed to load/decode TDM files: " + str(e))

# list ids of channelgroups
grpids = jack.get_channelgroup_ids()
grpids = [x.decode() for x in grpids]
print("list of channelgroups: ",grpids)

for grp in grpids[0:2] :

    # obtain meta data of channelgroups
    grpinfo = jack.get_channelgroup_info(grp.encode())
    print( json.dumps(grpinfo,sort_keys=False,indent=4) )

    # write channelgroup to file
    try :
        grpname = re.sub('[^A-Za-z0-9]','',grpinfo['name'])
        grpfile = "channelgroup_" + str(grp) + "_" + str(grpname) + ".csv"
        jack.print_channelgroup(grp.encode(),grpfile.encode(),True,ord(' '))
    except RuntimeError as e :
        print("failed to print channelgroup: " + str(grp) + " : " + str(e))

# list ids of channels
chnids = jack.get_channel_ids()
chnids = [x.decode() for x in chnids]
print("list of channels: ",chnids)

for chn in chnids[0:2] :

    # obtain meta-data
    chninfo = jack.get_channel_info(chn.encode())
    print( json.dumps(chninfo,sort_keys=False,indent=4) )

    # channel data
    try :
        chndata = jack.get_channel(chn.encode())
    except RuntimeError as e :
        print("failed to extract channel: " + str(chn) + " : " + str(e))

    print(str(chndata[0:6]) + " ...")

    # write channel to file
    chnfile = "channel_" + str(chn) + ".csv"
    try :
        jack.print_channel(chn.encode(),chnfile.encode(),True)
    except RuntimeError as e :
        print("failed to print channel: " + str(chn) + " : " + str(e))
