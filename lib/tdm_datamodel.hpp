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

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"id:"<<id_<<"\n"
      <<std::setw(width)<<std::left<<"byteOffset:"<<std::to_string(byte_offset_)<<"\n"
      <<std::setw(width)<<std::left<<"length:"<<std::to_string(length_)<<"\n"
      <<std::setw(width)<<std::left<<"blockOffset:"<<std::to_string(block_offset_)<<"\n"
      <<std::setw(width)<<std::left<<"blockSize:"<<std::to_string(block_size_)<<"\n"
      <<std::setw(width)<<std::left<<"valueType:"<<value_type_<<"\n";
    return ss.str();
  }
};

// -------------------------------------------------------------------------- //
// tdm datatypes

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_header_tdx_data/

struct tdm_datatype {

  std::string name_;
  std::string channel_datatype_;
  int numeric_;
  std::string value_sequence_;
  int size_;
  std::string description_;

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
      <<std::setw(width)<<std::left<<"channel_datatype:"<<channel_datatype_<<"\n"
      <<std::setw(width)<<std::left<<"numeric:"<<numeric_<<"\n"
      <<std::setw(width)<<std::left<<"value_sequence:"<<value_sequence_<<"\n"
      <<std::setw(width)<<std::left<<"size:"<<size_<<"\n"
      <<std::setw(width)<<std::left<<"description:"<<description_<<"\n";
    return ss.str();
  }
};

const std::map<std::string,tdm_datatype> tdm_datatypes = {

  {"eInt16Usi",{"eInt16Usi","DT_SHORT",2,"short_sequence",2,"signed 16 bit integer"}},
  {"eInt32Usi",{"eInt32Usi","DT_LONG",6,"long_sequence",4,"signed 32 bit integer"}},

  {"eUInt8Usi",{"eUInt8Usi","DT_BYTE",5,"byte_sequence",1,"unsigned 8 bit integer"}},
  {"eUInt16Usi",{"eUInt16Usi","DT_SHORT",2,"short_sequence",2,"unsigned 16 bit integer"}},
  {"eUInt32Usi",{"eUInt32Usi","DT_LONG",6,"long_sequence",4,"unsigned 32 bit integer"}},

  {"eFloat32Usi",{"eFloat32Usi","DT_FLOAT",3,"float_sequence",4,"32 bit float"}},
  {"eFloat64Usi",{"eFloat64Usi","DT_DOUBLE",7,"double_sequence",8,"64 bit double"}},

  {"eStringUsi",{"eStringUsi","DT_STRING",1,"string_sequence",0,"text"}}

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

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"id:"<<id_<<"\n"
      <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
      <<std::setw(width)<<std::left<<"description:"<<description_<<"\n"
      <<std::setw(width)<<std::left<<"title:"<<title_<<"\n"
      <<std::setw(width)<<std::left<<"author:"<<author_<<"\n"
      <<std::setw(width)<<std::left<<"timestamp:"<<timestamp_<<"\n"
      <<std::setw(width)<<std::left<<"channelgroups:";
    for ( auto el: channelgroups_ ) ss<<el<<",";
    std::string infostr = ss.str();
    infostr.pop_back();
    return ( infostr + std::string("\n") );
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
  std::vector<long int> channels_; // referenced by id

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"id:"<<id_<<"\n"
      <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
      <<std::setw(width)<<std::left<<"description:"<<description_<<"\n"
      <<std::setw(width)<<std::left<<"root:"<<root_<<"\n"
      <<std::setw(width)<<std::left<<"channels:";
    for ( auto el: channels_ ) ss<<el<<",";
    std::string infostr = ss.str();
    infostr.pop_back();
    return ( infostr + std::string("\n") );
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

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"id:"<<id_<<"\n"
      <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
      <<std::setw(width)<<std::left<<"description:"<<description_<<"\n"
      <<std::setw(width)<<std::left<<"unit_string:"<<unit_string_<<"\n"
      <<std::setw(width)<<std::left<<"datatype:"<<datatype_<<"\n"
      <<std::setw(width)<<std::left<<"minimum:"<<std::to_string(minimum_)<<"\n"
      <<std::setw(width)<<std::left<<"maximum:"<<std::to_string(maximum_)<<"\n"
      <<std::setw(width)<<std::left<<"group:"<<group_<<"\n"
      <<std::setw(width)<<std::left<<"local_columns:";
    for ( auto el: local_columns_ ) ss<<el<<",";
    std::string infostr = ss.str();
    infostr.pop_back();
    return ( infostr + std::string("\n") );
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

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"id:"<<id_<<"\n"
      <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
      <<std::setw(width)<<std::left<<"description:"<<description_<<"\n"
      <<std::setw(width)<<std::left<<"measurement:"<<measurement_<<"\n"
      <<std::setw(width)<<std::left<<"number_of_rows:"<<number_of_rows_<<"\n"
      <<std::setw(width)<<std::left<<"local_columns:";
    for ( auto el: local_columns_ ) ss<<el<<",";
    std::string infostr = ss.str();
    infostr.pop_back();
    return ( infostr + std::string("\n") );
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

  const std::string get_info(int width = 20)
  {
    std::stringstream ss;
    ss<<std::setw(width)<<std::left<<"id:"<<id_<<"\n"
      <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
      <<std::setw(width)<<std::left<<"description:"<<description_<<"\n"
      <<std::setw(width)<<std::left<<"measurement_quantity:"<<measurement_quantity_<<"\n"
      <<std::setw(width)<<std::left<<"submatrix_:"<<submatrix_<<"\n"
      <<std::setw(width)<<std::left<<"minimum:"<<std::to_string(minimum_)<<"\n"
      <<std::setw(width)<<std::left<<"maximum:"<<std::to_string(maximum_)<<"\n"
      <<std::setw(width)<<std::left<<"sequence_representation:"<<sequence_representation_<<"\n"
      <<std::setw(width)<<std::left<<"generation_parameters_:"<<"{"<<generation_parameters_[0]
                                                              <<","<<generation_parameters_[1]<<"}"<<"\n"
      <<std::setw(width)<<std::left<<"values:"<<values_<<"\n";
    return ss.str();
  }
};

#endif

// -------------------------------------------------------------------------- //
