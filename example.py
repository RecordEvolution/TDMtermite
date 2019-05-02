import tdm_ripper
import numpy as np
import matplotlib.pyplot as plt

tdmpath = b"samples/SineData.tdm"
tdxpath = b"samples/SineData.tdx"

# create instance of ripper class
RP = tdm_ripper.pytdmripper(tdmpath)
RP = tdm_ripper.pytdmripper(b"/Users/mariofink/git/Conti_HBS/data_science/python/features/tdm_tmp_slow/75_1/Messung.tdm")

# provide overview of available channels
RP.show_channels()

print(RP.num_channels())
print(RP.num_groups())

for i in range(0,RP.num_groups()):
    print(str(i+1).rjust(10)+str(RP.no_channels(i)).rjust(10))

# print particular channel to file
# RP.print_channel(1,b"SineData_extract.dat")

# extract channel and return it to numpy array
# channels = RP.get_channel(1)
# Nlen = len(channels)
# channels = np.append(channels,RP.get_channel(2))
# channels = np.append(channels,RP.get_channel(3))
# channels = np.append(channels,RP.get_channel(4))
# channels = np.append(channels,RP.get_channel(5))
# channels = np.append(channels,RP.get_channel(6))
# channels = np.append(channels,RP.get_channel(7))
# channels = np.append(channels,RP.get_channel(8))
# print(channels.shape)
# print("\n\n")
# print(channels[0:40])
#
# x = np.linspace(0,Nlen,Nlen)
# plt.plot(x,channels[0:Nlen])
# plt.plot(x,channels[Nlen:2*Nlen])
# plt.plot(x,channels[2*Nlen:3*Nlen])
#
# plt.grid()
# plt.show()
