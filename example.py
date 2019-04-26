import tdm_ripper
import numpy as np

tdmpath = b"samples/SineData.tdm"
tdxpath = b"samples/SineData.tdx"

# create instance of ripper class
RP = tdm_ripper.pytdmripper(tdmpath)

# provide overview of available channels
RP.show_channels()

print(RP.num_channels())
print(RP.num_groups())

# print particular channel to file
RP.print_channel(1,b"SineData_extract.dat")

# extract channel and return it to numpy array
chann = RP.get_channel(1)

print(dim(chann))
