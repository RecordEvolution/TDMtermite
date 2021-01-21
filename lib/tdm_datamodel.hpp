// ------------------------------------------------------------------------- //
/*
  for reference of the tdm data model, see

  https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_headerfile/
  https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_datamodel/
*/

#ifndef TDM_DATAMODEL
#define TDM_DATAMODEL

#include <iostream>
#include <iterator>
#include <vector>
#include <chrono>
#include <sstream>

// -------------------------------------------------------------------------- //
// format output of info strings for terminal and file table

class format
{

private:

  unsigned int width_;
  bool tabular_;
  bool header_;
  char sep_;
  std::vector<std::pair<std::string,std::string>> columns_;

public:

  format(int width = 25, bool tabular = false, bool header = false, char sep = ' '):
    width_(width), tabular_(tabular), header_(header), sep_(sep)
  {

  }

  void set_width(int width)
  {
    width_ = width;
  }

  void set_tabular(bool tabular)
  {
    tabular_ = tabular;
  }

  void set_header(bool header)
  {
    header_ = header;
  }

  void set_sep(char sep)
  {
    sep_ = sep;
  }

  void set_columns(std::vector<std::pair<std::string,std::string>> columns)
  {
    columns_ = columns;
  }

  std::string get_info()
  {
    std::stringstream ss;

    for ( std::vector<std::pair<std::string,std::string>>::iterator it = columns_.begin();
                                                                    it != columns_.end(); ++it )
    {
      if ( tabular_ )
      {
        // header or body of table
        std::string entry = header_? it->first : it->second;

        // make broad aligned columns for human reader
        if ( sep_ == ' ' )
        {
          entry = entry.size() > width_-2 ? entry.substr(0,width_-2) : entry;
          // if ( it == columns_.begin() && !header_ ) ss<<"  ";
          ss<<std::setw(width_)<<std::left<<entry;
        }
        // make compressed csv like columns
        else
        {
          ss<<entry;
          if ( std::next(it,1) != columns_.end() ) ss<<sep_;
        }
      }
      else
      {
        ss<<std::setw(width_)<<std::left<<(it->first+std::string(":"))<<it->second<<"\n";
      }
    }

    return ss.str();
  }
};

// define default formatter
static format defformat(25,false,false,',');

// join a list of strings
static std::string join_strings(std::vector<std::string> &thestring, const char* sep = " ")
{
  std::string joined;
  for ( std::vector<std::string>::iterator it = thestring.begin();
                                           it != thestring.end(); ++it )
  {
    joined += std::next(it,1) != thestring.end() ? ( *it + std::string(sep) ) : *it;
  }

  return joined;
}

// -------------------------------------------------------------------------- //
// tdm datatypes

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_header_tdx_data/

// define mapping of locally supported datatypes to tdm datatypes
typedef short int eInt16Usi;
typedef int eInt32Usi;
typedef unsigned char eUInt8Usi;
typedef unsigned short int eUInt16Usi;
typedef unsigned int eUInt32Usi;
typedef float eFloat32Usi;
typedef double eFloat64Usi;

enum class tdmdatatype {
  eInt16Usi,
  eInt32Usi,
  eUInt8Usi,
  eUInt16Usi,
  eUInt32Usi,
  eFloat32Usi,
  eFloat64Usi
};

struct tdm_datatype {

  std::string name_;
  std::string channel_datatype_;
  int numeric_;
  std::string value_sequence_;
  int size_;
  std::string description_;

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns( { std::make_pair("name",name_),
                             std::make_pair("channel_datatype",channel_datatype_),
                             std::make_pair("name",name_),
                             std::make_pair("value_sequence",value_sequence_),
                             std::make_pair("size",std::to_string(size_)),
                             std::make_pair("description",description_) } );

    return formatter.get_info();
  }
};

const std::vector<tdm_datatype> tdm_datatypes = {

  {"eInt16Usi","DT_SHORT",2,"short_sequence",2,"signed 16 bit integer"},
  {"eInt32Usi","DT_LONG",6,"long_sequence",4,"signed 32 bit integer"},

  {"eUInt8Usi","DT_BYTE",5,"byte_sequence",1,"unsigned 8 bit integer"},
  {"eUInt16Usi","DT_SHORT",2,"short_sequence",2,"unsigned 16 bit integer"},
  {"eUInt32Usi","DT_LONG",6,"long_sequence",4,"unsigned 32 bit integer"},

  {"eFloat32Usi","DT_FLOAT",3,"float_sequence",4,"32 bit float"},
  {"eFloat64Usi","DT_DOUBLE",7,"double_sequence",8,"64 bit double"},

  {"eStringUsi","DT_STRING",1,"string_sequence",0,"text"}

};

// -------------------------------------------------------------------------- //
// block of data

struct block {

  std::string id_;
  unsigned long int byte_offset_;
  unsigned long int length_;
  unsigned long int block_offset_, block_size_;
  std::string value_type_;

  block () {
    id_ = std::string("");
    byte_offset_ = 0;
    length_ = 0;
    block_offset_ = 0;
    block_size_ = 0;
    value_type_ = std::string("");
  }

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("block-id",id_),
                            std::make_pair("byteOffset",std::to_string(byte_offset_)),
                            std::make_pair("length",std::to_string(length_)),
                            std::make_pair("blockOffset",std::to_string(block_offset_)),
                            std::make_pair("blockSize",std::to_string(block_size_)),
                            std::make_pair("valueType",value_type_) });

    return formatter.get_info();
  }

};

// -------------------------------------------------------------------------- //
// tdm_root

struct tdm_root {

  std::string id_;
  std::string name_;
  std::string description_;
  std::string title_;
  std::string author_;

  std::string timestamp_;
  // std::chrono::time_point timepoint_; // from string 2008-05-06T17:20:12.65074539184570313

  // std::stringstream ss;
  // ss<<"2008-05-06T17:20:12.65074539184570313";
  // std::cout<<ss.str()<<"\n";
  //
  // std::chrono::time_point start = std::chrono::high_resolution_clock::now();
  // std::time_t tt = std::chrono::system_clock::to_time_t(start);
  // // https://en.cppreference.com/w/cpp/io/manip/put_time
  // std::cout<<std::put_time(std::localtime(&tt),"%Y-%m-%dT%H:%M:%S")<<"\n";// "%F %T")<<"\n";
  //
  // // std::tm ts;
  // // // https://en.cppreference.com/w/cpp/io/manip/get_time
  // // std::get_time(&ts, "%Y-%m-%dT%H:%M:%S");
  // // auto tp = std::chrono::system_clock::from_time_t(std::mktime(&ts));
  // // std::time_t tt = std::chrono::system_clock::to_time_t(tp);
  // // // std::cout<<tt.strftime("%Y")<<"\n";
  // // std::cout<<ctime(&tt)<<"\n";

  std::vector<std::string> channelgroups_;

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("root-id",id_),
                            std::make_pair("name",name_),
                            std::make_pair("description",description_),
                            std::make_pair("title",title_),
                            std::make_pair("author",author_),
                            std::make_pair("timestamp",timestamp_),
                            std::make_pair("channelgroups",join_strings(channelgroups_)) });

    return formatter.get_info();
  }

};

// -------------------------------------------------------------------------- //
// tdm_channelgroup

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_metadata_chngroup/

struct tdm_channelgroup {

  std::string id_;
  std::string name_;
  std::string description_;
  std::string root_;
  std::vector<std::string> channels_; // referenced by id
  std::vector<std::string> submatrices_;

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("group-id",id_),
                            std::make_pair("name",name_),
                            std::make_pair("description",description_),
                            std::make_pair("root",root_),
                            std::make_pair("channels",join_strings(channels_)),
                            std::make_pair("submatrices",join_strings(submatrices_)) });

    return formatter.get_info();
  }

};

// -------------------------------------------------------------------------- //
// tdm_channel

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_metadata_channel/

// waveform channel type
enum class wf_time_pref_type {
  absolute,
  relative
};

// additional elements for wave form channels (encoded as attributes in
// <instance_attributes> of <tdm_channel>)
// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_tdxdata_waveform/
struct waveform_channel {
  std::string wf_xname_;
  std::string wf_xunit_string_;
  std::string wf_start_time_;
  double wf_start_offset_;
  double wf_increment_;
  unsigned long wf_samples_;
  wf_time_pref_type wf_time_pref;
};

struct tdm_channel {

  std::string id_;
  std::string name_;
  std::string description_;
  std::string unit_string_;
  std::string datatype_;
  double minimum_, maximum_;
  std::string group_;
  std::vector<std::string> local_columns_;

  // TODO
  waveform_channel wf_channel_;

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("channel-id",id_),
                            std::make_pair("name",name_),
                            std::make_pair("description",description_),
                            std::make_pair("unit_string",unit_string_),
                            std::make_pair("datatype",datatype_),
                            std::make_pair("minimum",std::to_string(minimum_)),
                            std::make_pair("maximum",std::to_string(maximum_)),
                            std::make_pair("group",group_),
                            std::make_pair("local_columns",join_strings(local_columns_)) });

    return formatter.get_info();
  }

};

// -------------------------------------------------------------------------- //
// submatrix

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_tdxdata_submatrix/

struct submatrix {

  std::string id_;
  std::string name_;
  std::string description_;
  std::string measurement_;                 // -> tdm_channelgroup id
  std::vector<std::string> local_columns_;  // -> list of type "localcolumn"
  unsigned long int number_of_rows_;        // -> number of values in channels

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("submatrix-id",id_),
                            std::make_pair("name",name_),
                            std::make_pair("description",description_),
                            std::make_pair("measurement",measurement_),
                            std::make_pair("local_columns",join_strings(local_columns_)),
                            std::make_pair("number_of_rows",std::to_string(number_of_rows_)) });

    return formatter.get_info();
  }

};

// -------------------------------------------------------------------------- //
// localcolumn

enum class representation {
  explicit_,          // !! explicit is C++ keyword!!
  implicit_linear_,   // datatype is always DT_DOUBLE, no <value_sequence> for implicit_linear_!!
  raw_linear_         // datatype is always DT_DOUBLE
};

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_tdxdata_localcolumn/

struct localcolumn {

  std::string id_;
  std::string name_;
  std::string description_;
  std::string measurement_quantity_;        // -> tdm_channel
  std::string submatrix_;

  unsigned long int global_flag_;
  unsigned long int independent_;

  double minimum_, maximum_;
  // representation sequence_representation_;
  std::string sequence_representation_;
  std::vector<double> generation_parameters_; // { offset, factor }

  std::string values_;                        // -> refers to usi:data -> _sequence
  std::string external_id_;

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("localcolumn-id",id_),
                            std::make_pair("name",name_),
                            std::make_pair("description",description_),
                            std::make_pair("measurement_quantity",measurement_quantity_),
                            std::make_pair("submatrix_",submatrix_),
                            std::make_pair("minimum",std::to_string(minimum_)),
                            std::make_pair("maximum",std::to_string(maximum_)),
                            std::make_pair("sequence_representation",sequence_representation_),
                            std::make_pair("values",values_),
                            std::make_pair("external",external_id_) });

    return formatter.get_info();
  }

};

#endif

// -------------------------------------------------------------------------- //
