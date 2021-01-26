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

#include "tdm_format.hpp"

// -------------------------------------------------------------------------- //
// tdm meta data

struct tdm_meta {

  // usi:documentation
  std::string docu_expo_, docu_expover_;

  // usi:model
  std::string model_name_, model_version_;
  std::string model_include_uri_;

  // usi:include
  std::string byte_order_;  // little versus big endian
  std::string file_url_;    // path/URL of corresponding .tdx file

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns({ std::make_pair("exporter",docu_expo_),
                            std::make_pair("exporterVersion",docu_expover_),
                            std::make_pair("modelName",model_name_),
                            std::make_pair("modelVersion",model_version_),
                            std::make_pair("modelnsURI",model_include_uri_),
                            std::make_pair("byteOrder",byte_order_),
                            std::make_pair("fileURL",file_url_) } );

    return formatter.get_info();
  }

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

  const std::string get_json()
  {
    std::stringstream ss;
    ss<<"{"<<"\"group-id\":\""<<id_
           <<"\",\"name\":\""<<name_
           <<"\",\"description\":\""<<description_
           <<"\",\"root\":\""<<root_
           <<"\",\"channels\":\""<<join_strings(channels_)<<"\"}";
    return ss.str();
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

  const std::string get_json()
  {
    std::stringstream ss;
    ss<<"{"<<"\"channel-id\":\""<<id_
           <<"\",\"name\":\""<<name_
           <<"\",\"description\":\""<<description_
           <<"\",\"unit_string\":\""<<unit_string_
           <<"\",\"datatype\":\""<<datatype_
           <<"\",\"minimum\":\""<<minimum_
           <<"\",\"maximum\":\""<<maximum_
           <<"\",\"group\":\""<<group_<<"\"}";
    return ss.str();
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

  localcolumn () {
    id_ = std::string("");
    name_ = std::string("");
    description_ = std::string("");
    measurement_quantity_ = std::string("");
    submatrix_ = std::string("");
    global_flag_ = 15;
    independent_ = 0;
    minimum_ = 0.0;
    maximum_ = 0.0;
    sequence_representation_ = std::string("explicit");
    generation_parameters_ = { 0.0, 1.0 };
    values_ = std::string("");
    external_id_ = std::string("");
  }

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
                            std::make_pair("generation_parameters",join<double>(generation_parameters_)),
                            std::make_pair("values",values_),
                            std::make_pair("external",external_id_) });

    return formatter.get_info();
  }

};

#endif

// -------------------------------------------------------------------------- //
