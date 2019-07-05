# tdm_ripper

The tdm_ripper provides convenient access to the TDM/TDMS data format employed by
National Instruments LabView and DIAdem.

## Data Format

Datasets encoded in the TDM/TDMS format come along in pairs comprised of a
.tdm and .tdx file. While the .tdm file is a human-readable document providing
meta information about the dataset, the .tdx is a binary containing the actual data.
The .tdm is represented in XML format and basically reveals what data the .tdx
contains and how to read it. The XML tree is usually made up of several groups,
each containing an arbitrary amount of channels.

## Usage

Although the package is built upon a C++ core, which decodes the data, it may be
used as a Python module, as well, by interfacing the C++ library with a Cython
wrapper.

### C++ core

- In order to parse the XML tree of the .tdm file, the library employs pugixml:
  https://pugixml.org/ and https://github.com/zeux/pugixml
- The package currently supports the following datatypes:
  - eInt8Usi: 8 byte
  - eInt16Usi: 16 byte
  - eInt32Usi: 32 byte
  - eInt64Usi: 64 byte
  - eUInt8Usi: 8 byte
  - eUInt16Usi: 16 byte
  - eUInt32Usi: 32 byte
  - eUInt64Usi: 64 byte
  - eFloat32Usi: 32 byte
  - eFloat64Usi: 64 byte
- main.cpp contains an example of how the C++ library might be used to provide
  the channels and groups of the dataset.

### Python module

- The library may also be used as a Python module and supports the use of
  group channels in NumPy arrays.
