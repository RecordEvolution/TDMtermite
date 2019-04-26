import tdm_ripper
import numpy as np
import matplotlib.pyplot as plt

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
channels = RP.get_channel(1)
Nlen = len(channels)
channels = np.append(channels,RP.get_channel(2))
channels = np.append(channels,RP.get_channel(3))
channels = np.append(channels,RP.get_channel(4))
channels = np.append(channels,RP.get_channel(5))
channels = np.append(channels,RP.get_channel(6))
channels = np.append(channels,RP.get_channel(7))
channels = np.append(channels,RP.get_channel(8))
print(channels.shape)
print("\n\n")
print(channels[0:40])

x = np.linspace(0,Nlen,Nlen)
plt.plot(x,channels[0:1000])
plt.plot(x,channels[1000:2000])
plt.plot(x,channels[2000:3000])
plt.plot(x,channels[3000:4000])
plt.plot(x,channels[4000:5000])
plt.plot(x,channels[5000:6000])
plt.plot(x,channels[6000:7000])
plt.plot(x,channels[7000:8000])
plt.grid()
plt.show()
