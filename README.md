
<p align="center">
  <a href="https://github.com/RecordEvolution/tdm_ripper.git">
    <img
      alt="tdmreaper.svg"
      src="assets/tdmreaper.svg"
      width="400"
    />
  </a>
</p>

The _tdm_reaper_ is a C++ based library that decodes (encodes) the proprietary
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
`usi:include` and `usi:data`. The element `usi:include` references the data file
`example.tdx` and reveals one of _two_ possible orderings of the mass data (.tdx):

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

The XML element `<usi:data>` is basically comprised of _five_ different types of
elements that are `<tdm_root>`, `<tdm_channelgroup>`, `<tdm_channel>`, `<localcolumn>`
and `<submatrix>`. The root element `<tdm_root>` describes the general properties
of the dataset and lists the _id's_ of all channel groups that belong to
the dataset. The element `<tdm_channelgroup>` divides the _channels_ into groups
and has a unique _id_ that is referenced by its root element. The `<channels>`
element in `<tdm_channelgroup>` lists the unique ids of all channels that belong
to that group. Finally, the element `<tdm_channel>` describes a single column of
actual data including its datatype. The remaining element types are
`<localcolumn>`

```xml
<localcolumn id="usiXY">
  <name>Untitled</name>
  <measurement_quantity>#xpointer(id("usiAB"))</measurement_quantity>
  <submatrix>#xpointer(id("usiMN"))</submatrix>
  <global_flag>15</global_flag>
  <independent>0</independent>
  <sequence_representation> ... </sequence_representation>
  <values>#xpointer(id("usiZ"))</values>
</localcolumn>
```

with a unique id, the `<measurement_quantity>` refering to one specific channel,
the `<submatrix>` and its id respectively, the type of representation in
`<sequence_representation>` - being one of _explicit_, _implicit linear_ or
_rawlinear_ - and the `<values>` element, which refers to one _value sequence_,
and the element `<submatrix>`

```xml
<submatrix id="usiXX">
  <name>Untitled</name>
  <measurement>#xpointer(id("usiUV"))</measurement>
  <number_of_rows>N</number_of_rows>
  <local_columns>#xpointer(id("usiMN"))</local_columns>
</submatrix>
```

that references the channel group in `<measurement>` it belongs to and provides
the _number of rows_ in the channels listed in `<local_columns>`.

## Installation

The library can be used both as a _CLI_ based tool and as a _Python_ module.

### CLI tool

To install the CLI tool _tdmreaper_ do

```Shell
make install
```

which uses `/usr/local/bin` as installation directory. On _macOSX_ please first
build the binary locally with `make` and install it in your preferred location.

### Python

In order to build a _Python module_ from the _C++_ code base the
[Cython](https://cython.readthedocs.io/en/latest/index.html) package must be
available, which may be installed via `python3 -m pip install cython` .
Furthermore, the [Numpy](https://numpy.org) package is recommended to be able
to pass arrays of data from the C++ kernel to Python. The _makefile_ provides
the target `make cython-requirements` to install all required Python modules.
Finally, to build the Python extension _tdm_reaper_ either locally or install
it the targets `make cython-build` and `make cython-install` are provided.
Hence, to install the Python module on the system simply do

```Shell
make cython-requirements
make cython-install
```

that makes the module available to be imported as `import tdm_reaper` .

## Usage

### CLI tool

The usage of the CLI tool is sufficiently clarified by its help message displayed
by `tdmreaper --help`. For instance, to extract the data decoded in the pair of
files `samples/SineData.tdm` and `samples/SineData.tdx` into the directory
`/home/jack/data/`:

```Shell
tdmreaper samples/SineData.tdm samples/SineData.tdx --output /home/jack/data
```

The tool can also be used to list the available objects in the TDM dataset, which
are i.a. _channels_, _channelgroups_ and TDX _blocks_. For instance, to list
all channels and channelgroups (without writing any file output):

```Shell
tdmreaper samples/SineData.tdm samples/SineData.tdx --listgroups --listchannels
```

The user may also submit a _filenaming rule_ to control the names of the files the
channel(-group)s are written to. To this end, the _magic flags_ `%G` `%g`, `%C`
and `%c` representing the group id, group name, channel index and channel name
are defined. The default filenaming option is

```Shell
tdmreaper samples/SineData.tdm samples/SineData.tdx --output /home/jack/data --filenames channelgroup_%G.csv
```

which makes the tool write _all channels_ grouped into files according to their
group association, while all channelgroup filenames obey the pattern `channelgroup_%G.csv`
with `%G` being replaced by the group id. The filenaming rule also enables the user
to extract only a single channel(group) by providing a particular channel(-group)
id in the filenaming flag. For example,

```Shell
tdmreaper samples/SineData.tdm samples/SineData.tdx --output /home/jack/data -f channel_usi16_%c.csv --includemeta
```

will write the single channel with id `usi16` to the file
`/home/jack/data/channel_usi16_A4.csv` including its meta-data as a file header.

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

### TDM

- https://www.ni.com/de-de/support/documentation/supplemental/10/ni-tdm-data-model.html
- https://zone.ni.com/reference/en-XX/help/371361R-01/lvconcepts/fileio_tdms_model/
- https://zone.ni.com/reference/en-XX/help/371361R-01/lvhowto/ni_test_data_exchange/
- https://www.ni.com/de-de/support/documentation/supplemental/06/the-ni-tdms-file-format.html
- https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_headerfile/
- https://www.ni.com/content/dam/web/product-documentation/c_dll_tdm.zip

### IEEE Standard and datatypes

- https://en.wikipedia.org/wiki/IEEE_754
- https://www.ias.ac.in/public/Volumes/reso/021/01/0011-0030.pdf
- https://en.cppreference.com/w/cpp/language/types

### Implementation

- https://pugixml.org/
- https://github.com/zeux/pugixml
- https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html
