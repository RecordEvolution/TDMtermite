import tdm_ripper

tdmpath = b"samples/SineData.tdm"
tdxpath = b"samples/SineData.tdx"

rippobj = tdm_ripper.pytdmripper(tdmpath)

rippobj.show_channels()

print(rippobj.num_channels())
print(rippobj.num_groups())

rippobj.print_channel(1,b"SineData_extract.dat")
