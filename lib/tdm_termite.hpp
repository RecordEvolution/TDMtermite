// ------------------------------------------------------------------------- //

#ifndef TDM_TERMITE
#define TDM_TERMITE

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
#include <regex>

#include "pugixml.hpp"
#include "tdm_datamodel.hpp"
#include "tdm_datatype.hpp"

// -------------------------------------------------------------------------- //

class tdm_termite
{
  // .tdm and .tdx paths/filenames
  std::string tdmfile_;
  std::string tdxfile_;

  // set of .csv files (encoding mode)
  std::vector<std::string> csvfile_;

  // endianness (true = little, false = big)
  bool endianness_, machine_endianness_;

  // tdm meta-data
  tdm_meta meta_data_;

  // resconstruct "tdm_datatype.hpp: tdm_datatypes" as map to quickly map
  // "valueType"/"channel_datatype" to full datatype
  std::map<std::string,tdm_datatype> tdmdt_name_, tdmdt_chan_;

  // blocks of data in .tdx file
  std::map<std::string,block> tdx_blocks_;

  // tdm root
  tdm_root tdmroot_;

  // tdm channelgroups
  std::map<std::string,tdm_channelgroup> tdmchannelgroups_;

  // tdm channels
  std::map<std::string,tdm_channel> tdmchannels_;
  std::map<std::string,std::vector<tdm_datatype>> tdmchannels_data_;

  // submatrices and local_columns
  std::map<std::string,submatrix> submatrices_;
  std::map<std::string,localcolumn> localcolumns_;

  // binary data container/file stream
  std::vector<unsigned char> tdxbuffer_;
  std::ifstream *tdx_ifstream_;

  // extract list of identifiers from e.g. "#xpointer(id("usi12") id("usi13"))"
  std::vector<std::string> extract_ids(std::string idstring)
  {
    // collect group identifiers by means of regex pattern "usi[0-9]+"
    std::regex regid("(usi[0-9]+)");

    // declare match instance and regex iterator (to find ALL matches)
    std::smatch usi_match;
    std::sregex_iterator pos(idstring.begin(), idstring.end(), regid);
    std::sregex_iterator end;

    // iterate through all matches
    std::vector<std::string> listofids;
    for ( ; pos != end; ++pos) listofids.push_back(pos->str());

    return listofids;
  }

  // split string into substrings by delimiting string
  std::vector<std::string> split(std::string fullstring, std::string delstr)
  {
    // declare array of resulting strings
    std::vector<std::string> splitstrings(0);

    // parse input string for substring
    while ( fullstring.find(delstr) != std::string::npos )
    {
      // find first occurence of delimiting string in 'mystring'
      std::size_t delpos = fullstring.find(delstr);

      // extract substring
      std::string stringel = fullstring.substr(0,delpos);

      // append first word to array
      if ( !stringel.empty() )
      {
        splitstrings.push_back(stringel);
      }

      // remove first word from 'fullstring'
      fullstring = fullstring.substr(delpos+delstr.size(),fullstring.size());
    }

    // append last word to array
    splitstrings.push_back(fullstring);

    return splitstrings;
  }

public:

  // encoding
  // tdm_termite(std::vector<std::string> csvfile);

  // decoding
  tdm_termite();
  tdm_termite(std::string tdmfile, std::string tdxfile = std::string(""),
             bool showlog = false);
  ~tdm_termite();
  tdm_termite(const tdm_termite& other);
  tdm_termite& operator=(const tdm_termite& other);

  // provide (tdm,tdx) files
  void submit_files(std::string tdmfile, std::string tdxfile = std::string(""),
                    bool showlog = false);

  // process TDM data model in tdm file
  void process_tdm(bool showlog);

  // process <usi:include> element
  void process_include(bool showlog, pugi::xml_document& xml_doc);

  // extract tdm_root
  void process_root(bool showlog, pugi::xml_document& xml_doc);

  // process/list all channels and groups
  void process_channelgroups(bool showlog, pugi::xml_document& xml_doc);
  void process_channels(bool showlog, pugi::xml_document& xml_doc);

  // process submatrices and localcolumns
  void process_submatrices(bool showlog, pugi::xml_document& xml_doc);
  void process_localcolumns(bool showlog, pugi::xml_document& xml_doc);

  // get meta-data
  tdm_meta get_meta()
  {
    return meta_data_;
  }

  // get root element
  tdm_root get_root()
  {
    return tdmroot_;
  }

  // get channel/channelgroup meta object
  tdm_channel& channel(std::string channelid)
  {
    if ( tdmchannels_.count(channelid) == 1 ) {
      return tdmchannels_.at(channelid);
    } else {
      throw std::runtime_error(std::string("channel does not exist: ") + channelid);
    }
  }
  tdm_channelgroup& channelgroup(std::string groupid)
  {
    if ( tdmchannelgroups_.count(groupid) == 1 ) {
      return tdmchannelgroups_.at(groupid);
    } else {
      throw std::runtime_error(std::string("channelgroup does not exist: ") + groupid);
    }
  }

  // get full channel(group) overview
  std::string get_channel_overview(format chformatter);

  // get block/submatrix/localcolumn overview
  template<typename tdmelement>
  std::string get_overview(format formatter);
private:
  void summarize_member(tdm_channelgroup &chp, std::string& summary, format& formatter);
  void summarize_member(submatrix &sbm, std::string& summary, format& formatter);
  void summarize_member(localcolumn &lcc, std::string& summary, format& formatter);
  void summarize_member(block &blk, std::string& summary, format& formatter);
public:

  // get list of channelgroup ids
  std::vector<std::string> get_channelgroup_ids()
  {
    std::vector<std::string> channelgroup_ids;
    for (std::map<std::string,tdm_channelgroup>::iterator it=tdmchannelgroups_.begin();
                    it!=tdmchannelgroups_.end(); ++it) channelgroup_ids.push_back(it->first);

    return channelgroup_ids;
  }

  // get list of channel ids
  std::vector<std::string> get_channel_ids()
  {
    std::vector<std::string> channel_ids;
    for (std::map<std::string,tdm_channel>::iterator it=tdmchannels_.begin();
                    it!=tdmchannels_.end(); ++it) channel_ids.push_back(it->first);

    return channel_ids;
  }

  // extract channel by id
  std::vector<tdmdatatype> get_channel(std::string& id);

  // additional methods for Cython/Python compatibiliy
  //
  // extract and return any channel as datatype double
  std::vector<double> get_channel_as_double(std::string id)
  {
    std::vector<tdmdatatype> tdmchn = this->get_channel(id);
    std::vector<double> chn;
    for ( tdmdatatype el: tdmchn ) chn.push_back(el.as_double());
    return chn;
  }
  // get channel(-group) meta-data
  std::string get_channelgroup_info(std::string id)
  {
    if ( tdmchannelgroups_.count(id) == 1 ) {
      return tdmchannelgroups_.at(id).get_json();
    } else {
      throw std::runtime_error(std::string("channelgroup does not exist: ") + id);
    }
  }
  std::string get_channel_info(std::string id)
  {
    if ( tdmchannels_.count(id) == 1 ) {
      return tdmchannels_.at(id).get_json();
    } else {
      throw std::runtime_error(std::string("channel does not exist: ") + id);
    }
  }

  // dump a single channel/entire group (identified by id) to file
  void print_channel(std::string &id, const char* filename, bool include_meta = true);
  void print_group(std::string &id, const char* filename, bool include_meta = true,
                   char sep = ' ', std::string column_header = std::string(""));
  void check_filename_path(const char* filename);

  // check machine's datatypes
  // https://en.cppreference.com/w/cpp/language/types
  void check_local_datatypes();

private:

  template<typename datatype>
  void convert_data_to_type(std::vector<unsigned char> &buffer,
                            std::vector<tdmdatatype> &channel);

  // check consistency of mapped datatypes between C++ and TDM datatypes
  void check_datatype_consistency();

};

#endif

// -------------------------------------------------------------------------- //
