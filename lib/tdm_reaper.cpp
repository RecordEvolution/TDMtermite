// -------------------------------------------------------------------------- //

#include "tdm_reaper.hpp"

// -------------------------------------------------------------------------- //

tdm_reaper::tdm_reaper(std::string tdmfile, std::string tdxfile, bool showlog):
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

  // set up xml-parser and load tdm-file
  try {
    xml_result_ = xml_doc_.load_file(tdmfile_.c_str());

    if ( showlog )
    {
      std::cout<<"\nloading "<<tdmfile_<<": "<<xml_result_.description()<<"\n\n";
      std::cout<<"encoding: "<<(pugi::xml_encoding)xml_result_.encoding<<"\n";

      pugi::xml_node tdmdocu = xml_doc_.child("usi:tdm").child("usi:documentation");
      std::cout<<tdmdocu.child_value("usi:exporter")<<"\n"
               <<tdmdocu.child_value("usi:exporterVersion")<<"\n";

      pugi::xml_node tdmmodel = xml_doc_.child("usi:tdm").child("usi:model");
      std::cout<<tdmmodel.attribute("modelName").value()<<"\n";
      std::cout<<tdmmodel.child("usi:include").attribute("nsUri").value()
               <<tdmmodel.child("usi:include").attribute("modelVersion").value()<<"\n";
    }

  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("failed to load tdm file: ") + e.what());
  }

  // process elements of XML
  this->process_include(showlog);

}

void tdm_reaper::process_include(bool showlog)
{
  // get XML node
  pugi::xml_node tdmincl = xml_doc_.child("usi:tdm").child("usi:include");

  // check endianess
  std::string endianness(tdmincl.child("file").attribute("byteOrder").value());
  endianness_ = endianness.compare("littleEndian") == 0 ? true : false;

  // obtain machine's endianess
  int num = 1;
  machine_endianness_ = ( *(char*)&num == 1 );
  if ( machine_endianness_ != endianness_ ) throw std::runtime_error("endianess mismatch");

  if ( showlog )
  {
    std::cout<<"\n";
    std::cout<<"endianess:          "<<(endianness_?"little":"big")<<"\n"
             <<"machine endianness: "<<(machine_endianness_?"little":"big")<<"\n\n";
  }

  // check for existence of attributes before using
  // pugi::xml_attribute attr;

  // list block of massdata
  for (pugi::xml_node anode: tdmincl.child("file").children())
  {
    // declare new block
    block tdxblock;

    if ( anode.attribute("id") )
    {
      tdxblock.id_ = anode.attribute("id").value();
    }
    if ( anode.attribute("byteOffset") )
    {
      tdxblock.byte_offset_ = std::stoul(anode.attribute("byteOffset").value());
    }
    if ( anode.attribute("length") )
    {
      tdxblock.length_ = std::stoul(anode.attribute("length").value());
    }
    if ( anode.attribute("blockOffset") )
    {
      tdxblock.block_offset_ = std::stoul(anode.attribute("blockOffset").value());
    }
    if ( anode.attribute("blockSize") )
    {
      tdxblock.block_size_ = std::stoul(anode.attribute("blockSize").value());
    }
    if ( anode.attribute("valueType") )
    {
      tdxblock.value_type_ = anode.attribute("valueType").value();
    }

    // add block to map
    tdx_blocks_.insert(std::pair<std::string,block>(tdxblock.id_,tdxblock));

    if ( showlog ) std::cout<<tdxblock.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of blocks: "<<tdx_blocks_.size()<<"\n";
}


  // pugi::xml_node xmlusiincl = xml_doc_.child("usi:tdm").child("usi:include");
  // pugi::xml_node xmlusidata = xml_doc_.child("usi:tdm").child("usi:data");
  // pugi::xml_node xmltdmroot = xml_doc_.child("usi:tdm").child("usi:data").child("tdm_root");

void tdm_reaper::print_channel(int idx, char const* name, int width)
{

}

void tdm_reaper::list_groups(std::ostream& out, int g, int c)
{

}

void tdm_reaper::list_channels(std::ostream& out, int g, int c)
{

}

// -------------------------------------------------------------------------- //
