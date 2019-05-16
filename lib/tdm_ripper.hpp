
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

#include "../pugixml/pugixml.hpp"

class tdm_ripper
{
  // .tdm and .tdx filenames
  std::string tdmfile_;
  std::string tdxfile_;
  bool suppress_status_;

  // endianness (true = little, false = big)
  bool endianness_, machine_endianness_;

  // evtl. neglect groups with no actual channels
  bool neglect_empty_groups_;
  int num_empty_groups_;

  // number/names/ids of channels, channelgroups and channels's assignment to groups
  int num_channels_, num_groups_;
  std::vector<std::string> channel_id_, inc_id_, units_, channel_name_;
  std::vector<std::string> group_id_, group_name_;
  std::vector<std::pair<std::string,std::string>> group_timestamp_;
  std::vector<int> num_channels_group_;
  std::vector<int> channels_group_;
  std::vector<int> channel_ext_;

  // minimum/maximum value in particular channel (is provided in .tdm file as float)
  std::vector<std::pair<double,double>> minmax_;

  // use xpointers and ids to assign channels to byteoffsets
  std::map<std::string,std::string> xml_local_columns_, xml_values_, xml_double_sequence_;

  // byteoffset, length and datatype of channels
  std::vector<int> byteoffset_;
  std::vector<int> length_;
  std::vector<std::string> type_;
  std::vector<std::string> external_id_;

  // mapping of NI datatype to size (in bytes) of type
  std::map<std::string, int> datatypes_;

  // xml parser
  pugi::xml_document xml_doc_;
  pugi::xml_parse_result xml_result_;

  // binary data container
  std::vector<unsigned char> tdxbuf_;

public:

  tdm_ripper(std::string tdmfile, std::string tdxfile = "",
             bool suppress_status = true, bool neglect_empty_groups = true);

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
    assert( (numsum + channelid) > 0 && (numsum + channelid) <= num_channels_ );

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

  // evtl. get group time_stamp of .tdm file given in unix format
  static std::string unix_timestamp(std::string unixts)
  {
    // average year of Gregorian calender
    const double avgdaysofyear = 365.0 + 1./4 - 1./100 + 1./400
                                 - 8./24561; // gauge timestamp according to DIADEM result

    // convert string to long int = number of second since 0000/01/01 00:00
    long int ts = atol(unixts.c_str());
    assert( ts >= 0 );

    // use STL to convert timestamp (epoch usually starts from 01.01.1970)
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

  // TODO add elements/methods to build .tdm and write .tdx files for your own data
  // by constructing xml document tree and write data to binary .tdx
  // void set_channels(std::vector<std::string> channels);
  // void set_groups(std::vector<std::string> groups);
  // void set_assigment(std::vector<int> assignment);
  // void set_channel(int i, std::vector<double> data);

};

#endif
