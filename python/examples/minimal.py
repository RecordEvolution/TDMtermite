
import TDMtermite

# create 'tdm_termite' instance object
try :
    jack = TDMtermite.tdmtermite(b'samples/SineData.tdm',b'samples/SineData.tdx')
    # list ids of channelgroups
    grpids = jack.get_channelgroup_ids()
    # iterate through groups
    for grp in grpids :
        # write channelgroup to file (prepend './' for local directory!!)
        grpfile = "./channelgroup_" + str(grp.decode()) + ".csv"
        jack.print_channelgroup(grp,              # id of group to be written
                                grpfile.encode(), # filename
                                False,            # include meta-data fileheader?
                                ord(','),         # csv separator character
                                b""               # (empty=default) column header
                                )
except RuntimeError as e :
    print("failed to load/decode/write TDM files: " + str(e))
