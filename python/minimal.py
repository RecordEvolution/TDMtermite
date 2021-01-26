
import tdm_reaper

# create 'tdm_reaper' instance object
try :
    jack = tdm_reaper.tdmreaper(b'samples/SineData.tdm',b'samples/SineData.tdx')
    # list ids of channelgroups
    grpids = jack.get_channelgroup_ids()
    # iterate through groups
    for grp in grpids :
        # write channelgroup to file (prepend './' for local directory!!)
        grpfile = "./channelgroup_" + str(grp.decode()) + ".csv"
        jack.print_channelgroup(grp,grpfile.encode(),True,ord(' '))
except RuntimeError as e :
    print("failed to load/decode/write TDM files: " + str(e))
