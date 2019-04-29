
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
#include "../pugixml/pugixml.hpp"

class tdm_ripper
{
  // .tdm and .tdx filenames
  std::string tdmfile_;
  std::string tdxfile_;

  // endianness (true = little, false = big)
  bool endianness_, machine_endianness_;

  // number/names/ids of channels, channelgroups and channels's assignment to groups
  int num_channels_, num_groups_;
  std::vector<std::string> channel_id_, group_id_, channel_name_, group_name_;
  std::vector<int> num_channels_group_;
  std::vector<int> channels_group_;

  // byteoffset, length and datatype of channels
  std::vector<int> byteoffset_;
  std::vector<int> length_;
  std::vector<std::string> type_;

  // mapping of NI datatype to size (in bytes) of type
  std::map<std::string, int> datatypes_;

  // xml parser
  pugi::xml_document xml_doc_;
  pugi::xml_parse_result xml_result_;

  // binary data container
  std::vector<unsigned char> tdxbuf_;

public:

  tdm_ripper(std::string tdmfile, std::string tdxfile = "");

  void parse_structure();

  void list_channels(std::ostream& gout = std::cout, int width = 15, int maxshow = 300);

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

  // provide number of channels and group
  const int& num_channels()
  {
    return num_channels_;
  }
  const int& num_groups()
  {
    return num_groups_;
  }

  const std::string& channel_name(int channelid)
  {
    assert( channelid > 0 && channelid <= num_channels_ );

    return channel_name_[channelid-1];
  }

  const std::string& group_name(int groupid)
  {
    assert( groupid > 0 && groupid <= num_channels_ );

    return group_name_[groupid-1];
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
  std::vector<double> convert_channel(int byteoffset, int length, std::string type);

  std::vector<double> get_channel(int channelid);

  void print_channel(int channelid, const char* filename, int width = 15);

  // TODO add elements/methods to build .tdm and write .tdx files for your own data
  // by constructing xml document tree and write data to binary .tdx
  // void set_channels(std::vector<std::string> channels);
  // void set_groups(std::vector<std::string> groups);
  // void set_assigment(std::vector<int> assignment);
  // void set_channel(int i, std::vector<double> data);

};

#endif
