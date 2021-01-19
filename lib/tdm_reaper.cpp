// -------------------------------------------------------------------------- //

#include "tdm_reaper.hpp"

// -------------------------------------------------------------------------- //

tdm_ripper::tdm_ripper(std::string tdmfile, std::string tdxfile, bool showlog):
  tdmfile_(tdmfile), tdxfile_(tdxfile)
{
  // check both tdm, tdx files
  std::filesystem::path ptdm(tdmfile_), ptdx(tdxfile_);
  if ( !std::filesystem::exists(ptdm) )
  {
    throw std::runtime_error(std::string("*.tdm file ") + tdmfile_ + " does not exist!");
  }
  if ( !std::filesystem::exists(ptdx) )
  {
    throw std::runtime_error(std::string("*.tdx file ") + tdxfile_ + " does not exist!");
  }

  // set up xml-parser and load tdm file
  try {
    xml_result_ = xml_doc_.load_file(tdmfile_.c_str());

    if ( !showlog )
    {
      std::cout<<"loading and parsing file: "<<xml_result_.description()<<"\n";
      std::cout<<"encoding: "<<(pugi::xml_encoding)xml_result_.encoding<<"\n";
    }

  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("failed to load tdm file: ") + e.what());
  }

  // pugi::xml_node xmlusiincl = xml_doc_.child("usi:tdm").child("usi:include");
  // pugi::xml_node xmlusidata = xml_doc_.child("usi:tdm").child("usi:data");
  // pugi::xml_node xmltdmroot = xml_doc_.child("usi:tdm").child("usi:data").child("tdm_root");

}

void tdm_ripper::print_channel(int idx, char const* name, int width)
{

}

void tdm_ripper::list_groups(std::ostream& out, int g, int c)
{

}

void tdm_ripper::list_channels(std::ostream& out, int g, int c)
{

}

// -------------------------------------------------------------------------- //
