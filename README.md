
<p align="center">
  <a href="https://github.com/RecordEvolution/tdm_ripper.git">
    <img
      alt="tdmripper.svg"
      src="tdmripper.svg"
      width="400"
    />
  </a>
</p>

The _tdm_ripper_ is a C++ based library that decodes the proprietary
file format _TDM/TDX_ for measurement data, which relies upon the
_technical data management_ data model. The TDM format was introduced by
[National Instruments](https://www.ni.com) and is employed by
[LabVIEW](https://www.ni.com/de-de/shop/labview.html), LabWindows™/CVI™,
Measurement Studio, SignalExpress, and [DIAdem](https://www.ni.com/de-de/shop/data-acquisition-and-control/application-software-for-data-acquisition-and-control-category/what-is-diadem.html).

## Data Format

Datasets encoded in the TDM/TDX format come along in pairs comprised of a
.tdm (header) and a .tdx (data) file. While the .tdm file is a human-readable
file providing meta information about the data set, the .tdx is a binary
containing the actual data. The .tdm based on the _technical data management_
model is an XML file and basically describes what data the .tdx contains and how
to read it. The
[TDM data model](https://www.ni.com/de-de/support/documentation/supplemental/10/ni-tdm-data-model.html)
structures the data hierarchically with respect to _file_, (channel) _groups_ and
_channels_. The file level XML may contain any number of (channel) groups each
of which is made up of an arbitrary number of channels. Thus, the XML tree in
the [TDM header file](https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_headerfile/)
looks basically like this:

```xml
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<usi:tdm xmlns:usi="http://www.ni.com/Schemas/USI/1_0" version="1.0">

  <usi:documentation>
    <usi:exporter>National Instruments USI</usi:exporter>
    <usi:exporterVersion>1.5</usi:exporterVersion>
  </usi:documentation>

  <usi:model modelName="National Instruments USI generated meta file" modelVersion="1.0">
    <usi:include nsUri="http://www.ni.com/DataModels/USI/TDM/1_0"/>
  </usi:model>

  <usi:include>
    <file byteOrder="littleEndian" url="example.tdx">
    ...
    <block byteOffset="0" id="inc0" length="1000" valueType="eFloat64Usi"/>
    ...
    <block_bm id="inc4" blockOffset="100" blockSize="7" byteOffset="0" length="4" valueType="eInt8Usi"/>
    ...
  </usi:include>

  <usi:data>
    ...
  </usi:data>

</usi:tdm>
```

and is comprised of _four_ main XML elements: `usi:documentation`, `usi:model`,
`usi:include` and `usi:data`. The element `usi:include` reveals one of _two_
possible orderings of the mass data (.tdx):

1. either _channel wise_ (`<block>`) - all values of a specific channel follow subsequently -
1. or _block wise_ (`<block_bm>`) - all values of a specific measurement time follow subsequently -

ordering. The supported _numerical data types_ are

| datatype    | channel datatype | numeric | value sequence  | size  | description             |
|-------------|------------------|---------|-----------------|-------|-------------------------|
| eInt16Usi   | DT_SHORT         | 2       | short_sequence  | 2byte | signed 16 bit integer   |
| eInt32Usi   | DT_LONG          | 6       | long_sequence   | 4byte | signed 32 bit integer   |
| eUInt8Usi   | DT_BYTE          | 5       | byte_sequence   | 1byte | unsigned 8 bit integer  |
| eUInt16Usi  | DT_SHORT         | 2       | short_sequence  | 2byte | unsigned 16 bit integer |
| eUInt32Usi  | DT_LONG          | 6       | long_sequence   | 4byte | unsigned 32 bit integer |
| eFloat32Usi | DT_FLOAT         | 3       | float_sequence  | 4byte | 32 bit float            |
| eFloat64Usi | DT_DOUBLE        | 7       | double_sequence | 8byte | 64 Bit double           |
| eStringUsi  | DT_STRING        | 1       | string_sequence |       | text                    |

## Installation

The library can be used both as a _CLI_ based tool and as a _Python_ module.

### CLI tool

To install the CLI tool _tdmripper_ do

```Shell
make install
```

which uses `/usr/local/bin` as installation directory. On _macOSX_ please first
build the binary locally with `make` and install it to in your preferred location.

### Python

...tbc...

## Usage

The usage of the CLI tool is sufficiently clarified by its help message displayed
by `tdmripper --help`. For instance, to extract the data decoded in the pair of
files `samples/SineData.tdm` and `samples/SineData.tdx` into the directory
`/home/jack/data/`:

```Shell
tdmripper --output /home/jack/data samples/SineData.tdm samples/SineData.tdx
```

### CLI tool

### Python

...tbc...


## !!! Deprecated !!!

The makefile provides targets for using the library both as native C++ extension
and as Python module. The package supports usage on Linux and MacOSX.
The tdm_ripper module is built on these platforms by

```Shell
# Linux
pip install Cython
make install
```

and

```Shell
# macOS
pip install Cython
make install_osx
```

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
- The core of the library takes care of the decoding by reinterpretation of the
  binary in the buffer as the required datatype implemented by

  ```C++
  uint8_t *dfcast = reinterpret_cast<uint8_t*>(&df);

  for ( int i = 0; i < (int)sizeof(double); i++ )
  {
    dfcast[i] = (int)bych[i];
  }
  ```

  where for instance df is the resulting float and bych contains the binary
  data as an array of chars.
- main.cpp contains an example of how the C++ library might be used to provide
  the channels and groups of the dataset. It is simply build by

  ```Shell
  make
  ```

- extract_all.cpp takes the .tdm, the .tdx file and some output directory as arguments
  to provide all given information in .csv format without any logging. To build:

  ```Shell
  make extall
  ```

  For instance, the executable accepts the following arguments:

  ```Shell
  ./extract_all samples/SineData.tdm samples/SineData.tdx data/
  ```

### Python module

- The library may also be used as a Python module and supports the use of
  group channels in NumPy arrays as shown in example.py .
- To extract all available information and data in the TDM files without any
  further interaction, the use of extract_all.py is recommended. To exhibit the
  required arguments:

  ```Shell
  python extract_all.py --help
  ```
- The same functionality may be obtained from an existing python script by
  importing the tdm_ripper module and calling the extract_all function. For
  instance

  ```Python
  import tdm_ripper as td

  files = td.extract_all(b"samples/SineData.tdm",b"samples/SineData.tdx",b"data/",b"my_tdm")
  ```

  where the arguments "data/" and "my_tdm" are optional. "data/" specifies the
  directory where all .csv output is dumped while "my_tdm" represents a name
  prefix for all csv. files.
  Note, that all string arguments must be converted to bytes before passing to
  the argument list by prepending "b".

## References

- https://www.ni.com/de-de/support/documentation/supplemental/10/ni-tdm-data-model.html
- https://zone.ni.com/reference/en-XX/help/371361R-01/lvconcepts/fileio_tdms_model/
- https://zone.ni.com/reference/en-XX/help/371361R-01/lvhowto/ni_test_data_exchange/
- https://www.ni.com/de-de/support/documentation/supplemental/06/the-ni-tdms-file-format.html
- https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_headerfile/

### Code example
- https://www.ni.com/content/dam/web/product-documentation/c_dll_tdm.zip
