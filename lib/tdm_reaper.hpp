// ------------------------------------------------------------------------- //

#ifndef TDM_REAPER
#define TDM_REAPER

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

// -------------------------------------------------------------------------- //

class tdm_reaper
{
  // .tdm and .tdx paths/filenames
  std::string tdmfile_;
  std::string tdxfile_;

  // set of .csv files (encoding mode)
  std::vector<std::string> csvfile_;

  // XML parser
  pugi::xml_document xml_doc_;
  pugi::xml_parse_result xml_result_;

  // endianness (true = little, false = big)
  bool endianness_, machine_endianness_;

  // blocks of data in .tdx file
  std::map<std::string,block> tdx_blocks_;

  // tdm root
  tdm_root tdmroot_;

  // tdm channelgroups
  std::map<std::string,tdm_channelgroup> tdmchannelgroups_;

  // tdm channels
  std::map<std::string,tdm_channel> tdmchannels_;

  // submatrices and local_columns
  std::map<std::string,submatrix> submatrices_;
  std::map<std::string,localcolumn> localcolumns_;

  // // binary data container
  // std::vector<unsigned char> tdxbuf_;

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

public:

  // encoding
  tdm_reaper(std::vector<std::string> csvfile);

  // decoding
  tdm_reaper();
  tdm_reaper(std::string tdmfile, std::string tdxfile = std::string(""), bool showlog = false);

  // provide (tdm,tdx) files
  void submit_files(std::string tdmfile, std::string tdxfile = std::string(""), bool showlog = false);

  // process TDM data model in tdm file
  void process_tdm(bool showlog);

  // process <usi:include> element
  void process_include(bool showlog);

  // extract tdm_root
  void process_root(bool showlog);

  // process/list all channels and groups
  void process_channelgroups(bool showlog);
  void process_channels(bool showlog);

  // process submatrices and localcolumns
  void process_submatrices(bool showlog);
  void process_localcolumns(bool showlog);

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
  // (TODO introduce template T to reference specific datatype instead of double in general)
  std::vector<double> get_channel(std::string &id);
};

#endif

// -------------------------------------------------------------------------- //
