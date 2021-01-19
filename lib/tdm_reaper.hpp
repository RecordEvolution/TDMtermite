// ------------------------------------------------------------------------- //

#ifndef TDM_RIPPER
#define TDM_RIPPER

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>
#include <map>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <filesystem>

#include "pugixml.hpp"

// -------------------------------------------------------------------------- //
// define datatypes

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_header_tdx_data/

enum class tdm_datatype {
  eInt16Usi,
  eInt32Usi,
  //
  eUInt8Usi,
  eUInt16Usi,
  eUInt32Usi,
  //
  eFloat32Usi,
  eFloat64Usi,
  //
  eStringUsi
};

struct datatype {
  std::string name_;
  std::string channel_datatype_;
  int numeric_;
  std::string value_sequence_;
  int size_;
  std::string description_;

  std::string get_info()
  {
    std::stringstream ss;
    ss<<"name:             "<<name_<<"\n"
      <<"channel_datatype: "<<channel_datatype_<<"\n"
      <<"numeric:          "<<numeric_<<"\n"
      <<"value_sequence:   "<<value_sequence_<<"\n"
      <<"size:             "<<size_<<"\n"
      <<"description:      "<<description_<<"\n";
    return ss.str();
  }
};

const std::map<std::string,datatype> tdm_datatypes = {

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
// tdm root, tdm group and tdm channel, etc.

// for reference of the tdm data model, see
// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_headerfile/
// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_datamodel/

struct tdm_root {
  std::string name_;
  std::string description_;
  std::string title_;
  std::string author_;
  std::string timestamp_;
  // std::chrono::time_point timestamp_; // from string 2008-05-06T17:20:12.65074539184570313

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

  std::string channelgroups_;
};

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_metadata_chngroup/
struct tdm_channelgroup {
  unsigned long int id_;
  std::string name_;
  std::string description_;
  long int rootid_;
  std::vector<long int> channels_; // referenced by id
};

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

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_metadata_channel/
struct tdm_channel {
  unsigned long int id_;
  std::string name_;
  std::string description_;
  std::string unit_string_;
  tdm_datatype datatype_;
  double minimum_, maximum_;
  unsigned long int groupd_id_;
  waveform_channel wf_channel_;
};

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_tdxdata_submatrix/
struct tdm_submatrix {
  std::string name_;
  unsigned long int number_of_rows_;  // -> number of values in channels
  unsigned long int measurement_id_;  // -> channelgroup
  unsigned long int local_column_id_;
};

enum class representation {
  explicit_,          // !! explicit is C++ keyword!!
  implicit_linear_,   // datatype is always DT_DOUBLE, no <value_sequence> for implicit_linear_!!
  raw_linear_         // datatype is always DT_DOUBLE
};

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_tdxdata_localcolumn/
struct localcolumn {
  std::string name_;
  unsigned long int global_flag_;
  unsigned long int independent_;
  double minimum_, maximum_;
  representation sequence_representation_;
  std::vector<double> generation_parameters_; // { offset, factor }
  unsigned long int measurement_quantity_id_; // -> references single channel
  unsigned long int submatrix_id_;
  unsigned long int values_id_;               // -> refers to usi:data -> _sequence
};

// -------------------------------------------------------------------------- //

class tdm_ripper
{
  // .tdm and .tdx paths/filenames
  std::string tdmfile_;
  std::string tdxfile_;

  // set of .csv files
  std::vector<std::string> csvfile_;

  // endianness (true = little, false = big)
  bool endianness_, machine_endianness_;

  // tdm root
  tdm_root tdmroot_;

  // number/names/ids of channels, channelgroups and channels's assignment to groups
  int num_channels_, num_groups_;
  std::vector<std::string> channel_id_, inc_id_, units_, channel_name_;
  std::vector<std::string> group_id_, group_name_;
  std::vector<std::pair<std::string,std::string>> group_timestamp_;
  std::vector<int> num_channels_group_;
  std::vector<int> channels_group_;
  std::vector<int> channel_ext_;

  // neglect empty groups
  bool neglect_empty_groups_;
  int num_empty_groups_;

  // minimum/maximum value in particular channel (is provided in .tdm file as float)
  std::vector<std::pair<double,double>> minmax_;

  // use xpointers and ids to assign channels to byteoffsets
  std::map<std::string,std::string> xml_local_columns_, xml_values_, xml_double_sequence_;

  // byteoffset, length and datatype of channels
  std::vector<int> byteoffset_;
  std::vector<int> length_;
  std::vector<std::string> type_;
  std::vector<std::string> external_id_;

  // NI datatypes ( )
  std::map<std::string, int> datatypes_;

  // xml parser
  pugi::xml_document xml_doc_;
  pugi::xml_parse_result xml_result_;

  // .tdm-file eventually contains some meta information (about measurement)
  std::map<std::string,std::string> root_info_;
  std::map<std::string,std::string> meta_info_;

  // binary data container
  std::vector<unsigned char> tdxbuf_;

public:

  tdm_ripper(std::string tdmfile, std::string tdxfile = std::string(""), bool showlog = false);

  void parse_structure();

  void list_channels(std::ostream& gout = std::cout, int width = 15, int maxshow = 50);
  void list_groups(std::ostream& gout = std::cout, int width = 15, int maxshow = 50);

  void show_structure();

  // count number of occurences of substring in string
  int count_occ_string(std::string s, std::string sub)
  {
    int num_occs = 0;
    std::string::size_type pos = 0;

    while ( ( pos = s.find(sub,pos) ) != std::string::npos )
    {
      num_occs++;
      pos += sub.length();
    }

    return num_occs;
  }

  // obtain substring of 'entirestr' in between starting and stopping delimiter
  std::string get_str_between(std::string entirestr, std::string startlim, std::string stoplim)
  {
    std::size_t apos = entirestr.find(startlim);
    std::size_t bpos = entirestr.find_last_of(stoplim);
    assert(  apos != std::string::npos && bpos != std::string::npos );
    return entirestr.substr(apos+startlim.length(),bpos-(apos+startlim.length()));
  }

  void print_hash_local(const char* filename, int width = 20)
  {
    std::ofstream fout(filename);

    std::map<std::string,std::string>::iterator it;
    int count = 0;
    for ( it = xml_local_columns_.begin(); it != xml_local_columns_.end(); it++ )
    {
      count++;
      fout<<std::setw(width)<<count;
      fout<<std::setw(width)<<it->first;
      fout<<std::setw(width)<<it->second;
      fout<<"\n";
    }
    fout.close();
  }

  void print_hash_values(const char* filename, int width = 20)
  {
    std::ofstream fout(filename);

    std::map<std::string,std::string>::iterator it;
    int count = 0;
    for ( it = xml_values_.begin(); it != xml_values_.end(); it++ )
    {
      count++;
      fout<<std::setw(width)<<count;
      fout<<std::setw(width)<<it->first;
      fout<<std::setw(width)<<it->second;
      fout<<"\n";
    }
    fout.close();
  }

  void print_hash_double(const char* filename, int width = 20)
  {
    std::ofstream fout(filename);

    std::map<std::string,std::string>::iterator it;
    int count = 0;
    for ( it = xml_double_sequence_.begin(); it != xml_double_sequence_.end(); it++ )
    {
      count++;
      fout<<std::setw(width)<<count;
      fout<<std::setw(width)<<it->first;
      fout<<std::setw(width)<<it->second;
      fout<<"\n";
    }
    fout.close();
  }

  void print_extid(const char* filename, int width = 20)
  {
    std::ofstream fout(filename);

    int count = 0;
    for ( auto extid: channel_ext_ )
    {
      count++;
      fout<<std::setw(width)<<count;
      fout<<std::setw(width)<<extid;
      fout<<"\n";
    }
    fout.close();
  }

  // provide number of channels and group
  const int& num_channels()
  {
    return num_channels_;
  }
  const int& num_groups()
  {
    return num_groups_;
  }

  // get number of channels in specific group
  const int& no_channels(int groupid)
  {
    assert( groupid >= 0 && groupid < num_groups_ );

    return num_channels_group_[groupid];
  }

  const std::string& channel_name(int channelid)
  {
    assert( channelid >= 0 && channelid < num_channels_ );

    return channel_name_[channelid];
  }

  // obtain overall channel id from combined group and group-specific channel id
  int obtain_channel_id(int groupid, int channelid)
  {
    assert( groupid >= 0 && groupid < num_groups_ );
    assert( channelid >= 0 && channelid < num_channels_group_[groupid] );

    // find cummulative number of channels
    int numsum = 0;
    for ( int i = 0; i < groupid; i++ )
    {
      numsum += num_channels_group_[i];
    }
    assert( (numsum + channelid) >= 0 );
    assert( (numsum + channelid) <= num_channels_ );

    return numsum+channelid;
  }

  const std::string& channel_name(int groupid, int channelid)
  {
    return channel_name_[obtain_channel_id(groupid,channelid)];
  }

  const std::string& group_name(int groupid)
  {
    assert( groupid >= 0 && groupid < num_groups_ );

    return group_name_[groupid];
  }

  const std::string& channel_unit(int groupid, int channelid)
  {
    return units_[obtain_channel_id(groupid,channelid)];
  }

  int channel_exists(int groupid, std::string channel_name)
  {
    assert( groupid >= 0 && groupid < num_groups_ );

    int channelid = -1;
    for ( int i = 0; i < num_channels_group_[groupid]; i++)
    {
      if ( comparestrings(channel_name_[obtain_channel_id(groupid,i)],channel_name) )
      {
        channelid = i;
      }
    }
    return channelid;
  }

  bool comparestrings(std::string s1, std::string s2, bool case_sensitive = false)
  {
    if ( case_sensitive )
    {
      return ( s1.compare(s2) == 0 );
    }
    else
    {
      std::transform( s1.begin(), s1.end(), s1.begin(), ::tolower);
      std::transform( s2.begin(), s2.end(), s2.begin(), ::tolower);
      return ( s1.compare(s2) == 0 );
    }
  }

  // get time-stamp of channel-group in .tdm file given in unix format
  static std::string unix_timestamp(std::string unixts)
  {
    // average year of Gregorian calender
    const double avgdaysofyear = 365.0 + 1./4 - 1./100 + 1./400
                                 - 8./24561; // gauge timestamp according to DIADEM result

    // convert string to long int = number of seconds since 0000/01/01 00:00
    long int ts = atol(unixts.c_str());
    assert( ts >= 0 );

    // use STL to convert timestamp (epoch usually starts on 01.01.1970)
    std::time_t tstime = ts - 1970*avgdaysofyear*86400;

    // get rid of linebreak character and return the result
    return strtok(std::ctime(&tstime),"\n");
  }

  std::string time_stamp(int groupid, bool startstop = true)
  {
    assert( groupid >= 0 && groupid < num_groups_ );

    return startstop ? unix_timestamp(group_timestamp_[groupid].first)
                     : unix_timestamp(group_timestamp_[groupid].second);
  }

  void list_datatypes();

  // convert array of chars to single integer or floating point double
  int convert_int(std::vector<unsigned char> bych);
  double convert_double(std::vector<unsigned char> bych);

  // disassemble single integer or double into array of chars
  std::vector<unsigned char> convert_int(int number);
  std::vector<unsigned char> convert_double(double number);

  // convert entire channel, i.e. expert of .tdx binary file
  // std::vector<double> convert_channel(int byteoffset, int length, int typesize);
  std::vector<double> convert_channel(int channelid);

  // obtain channel from overall channel id...
  std::vector<double> get_channel(int channelid);
  // ...or from group id and group-specific channel id
  std::vector<double> channel(int groupid, int channelid)
  {
    return get_channel(obtain_channel_id(groupid,channelid));
  }

  int channel_length(int groupid, int channelid)
  {
    return length_[channel_ext_[obtain_channel_id(groupid,channelid)]];
  }

  double get_min(int groupid, int channelid)
  {
    return minmax_[obtain_channel_id(groupid,channelid)].first;
  }
  double get_max(int groupid, int channelid)
  {
    return minmax_[obtain_channel_id(groupid,channelid)].second;
  }

  void print_channel(int channelid, const char* filename, int width = 15);

  // obtain any meta information about .tdm-file if available
  std::string get_meta(std::string attribute_name)
  {
    // check if key "attribute_name" actually exits
    std::map<std::string,std::string>::iterator positer = meta_info_.find(attribute_name);
    bool ispresent = ( positer == meta_info_.end() ) ? false : true;

    return ispresent ? meta_info_[attribute_name] : "key does not exist";
  }

  // prepare meta information file including all available meta-data
  void print_meta(const char* filename, std::string sep = ",")
  {
    // open file
    std::ofstream fout(filename);

    for ( const auto& it : root_info_ )
    {
      fout<<it.first<<sep<<it.second<<"\n";
    }
    fout<<sep<<"\n";
    for ( const auto& it : meta_info_ )
    {
      fout<<it.first<<sep<<it.second<<"\n";
    }

    // close down file
    fout.close();
  }

  // TODO add elements/methods to build .tdm and write .tdx files for your own data
  // by constructing xml document tree and write data to binary .tdx
  // void set_channels(std::vector<std::string> channels);
  // void set_groups(std::vector<std::string> groups);
  // void set_assigment(std::vector<int> assignment);
  // void set_channel(int i, std::vector<double> data);

};

#endif
