import tdm_ripper
import numpy as np
import argparse
import re

parser = argparse.ArgumentParser(description='provide path of both .tdm and corresponding .tdx file')
parser.add_argument('tdm_file',type=str,help='path of .tdm file')
parser.add_argument('tdx_file',type=str,help='path of .tdx file')
#parser.add_argument('--tdx_file',type=str,help='path of .tdx file',default='')
parser.add_argument('--out_directory',type=str,help='choose directory where to write data',default='./')
parser.add_argument('--prefix_name',type=str,help='provide dataset name used as filename prefix',default='')
args = parser.parse_args()
#print(args)

# process arguments
tdmpath = args.tdm_file #"samples/SineData.tdm"
tdxpath = args.tdx_file #"samples/SineData.tdx"
outdirx = args.out_directory
fprefix = args.prefix_name

# if no prefix is given, .tdm filename will be used
if fprefix == '' :
    fprefix = tdmpath.rstrip('.tdm').split('/')[-1]
    # TODO better use os.path !!

#print(fprefix)

# create instance of ripper class
RP = tdm_ripper.pytdmripper(tdmpath.encode('utf-8'),tdxpath.encode('utf-8'))

# provide overview over available groups and channels
#RP.show_groups()
#RP.show_channels()

# obtain number of available groups and channels
numgr = RP.num_groups()
numch = RP.num_channels()

# dump all meta information
RP.print_meta((outdirx+fprefix+'.csv').encode('utf-8'))

# dump all available groups and channels
for g in range(0,numgr):
    numgrch = RP.no_channels(g)
    for c in range(0,numgrch):
        #print(str(g).rjust(10)+str(c).rjust(10))
        # print(str(RP.channel_length(g,c)))
        # print(RP.channel_name(g,c))
        #print(RP.channel(g,c))
        # obtained overall channel id
        chid = RP.obtain_channel_id(g,c)
        # get group's and channel's name
        gname = RP.group_name(g)
        cname = RP.channel_name(g,c)
        #print(gname.rjust(30)+cname.rjust(30))
        # use regular expression replacement to sanitize group and channel names
        gname = re.sub('[!@#$%^&*()-+= ,]','',gname)
        cname = re.sub('[!@#$%^&*()-+= ,]','',cname)
        # generate filename
        fichan = fprefix + '_' + str(g+1) + '_' + str(c+1) + '_' + gname + '_' + cname + '.csv'
        # print channel
        RP.print_channel(chid,(outdirx+fichan).encode('utf-8'))
