import tdm_ripper
import numpy as np
import matplotlib.pyplot as plt

tdmpatha = b"samples/SineData.tdm"
tdxpatha = b"samples/SineData.tdx"
tdmpathb = b"/Users/mariofink/Downloads/CONTI_HBS_samples/config_id_1/001_Test_DIAdem/Messung.tdm"
tdxpathb = b"/Users/mariofink/Downloads/CONTI_HBS_samples/config_id_1/001_Test_DIAdem/Messung.tdx"

tdmpathchz = tdmpathb
tdxpathchz = tdxpathb

# create instance of ripper class
# RP = tdm_ripper.pytdmripper(tdmpath)
RP = tdm_ripper.pytdmripper(tdmpathchz,tdxpathchz)

# provide overview of available channels
RP.show_channels()

print(RP.num_channels())
print(RP.num_groups())

for i in range(0,RP.num_groups()):
    print(str(i+1).rjust(10)+str(RP.no_channels(i)).rjust(10))

# print particular channel to file
RP.print_channel(1,b"SineData_extract.dat")

# show some meta information
print(RP.meta_info(b"SMP_Name").decode('utf-8'))
print(RP.meta_info(b"Location").decode('utf-8'))
print('\n')

RP.print_meta(b"meta_information.dat")

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
