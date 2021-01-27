
import tdm_reaper
import json
import re

# create 'tdm_reaper' instance object
try :
    jack = tdm_reaper.tdmreaper(b'samples/SineData.tdm',b'samples/SineData.tdx')
except RuntimeError as e :
    print("failed to load/decode TDM files: " + str(e))

# list ids of channelgroups
grpids = jack.get_channelgroup_ids()

# iterate through group-ids
for grp in grpids[0:2] :

    # obtain meta data of channelgroups
    grpinfo = jack.get_channelgroup_info(grp)
    print( json.dumps(grpinfo,sort_keys=False,indent=4) )

    # get channel meta-data and compose header
    column_header = ""
    chns = grpinfo['channels'].split(' ')
    for chn in chns :
        # obtain channel's meta-data as dictionary (JSON)
        chninfo = jack.get_channel_info(chn.encode())
        # print( json.dumps(chninfo,sort_keys=False,indent=4) )
        # choose e.g. channel-id and name to compose column header
        chnhead = ( str(chninfo['channel-id']) + "("
                  + str(chninfo['name']).replace(',',' ') + ")" )
        # append to full header
        column_header = column_header + chnhead + ","
    # remove last comma
    column_header = column_header[0:-1]

    print("column_header:\n"+column_header+"\n")

    # write channelgroup to file
    try :
        grpname = re.sub('[^A-Za-z0-9]','',grpinfo['name'])
        grpfile = str("./channelgroup_") + str(grp.decode()) + "_" + str(grpname) + str(".csv")
        jack.print_channelgroup(grp,                   # id of group to be written
                                grpfile.encode(),      # filename
                                False,                 # no meta-data header
                                ord(','),              # csv separator character
                                column_header.encode() # provide column header
                                )
    except RuntimeError as e :
        print("failed to print channelgroup: " + str(grp) + " : " + str(e))
