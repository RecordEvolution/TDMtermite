// -------------------------------------------------------------------------- //

#include "tdm_reaper.hpp"

// -------------------------------------------------------------------------- //

tdm_reaper::tdm_reaper()
{

}

tdm_reaper::tdm_reaper(std::string tdmfile, std::string tdxfile, bool showlog):
  tdmfile_(tdmfile), tdxfile_(tdxfile)
{
  // start processing tdm data model
  this->process_tdm(showlog);
}

void tdm_reaper::submit_files(std::string tdmfile, std::string tdxfile, bool showlog)
{
  // save files
  tdmfile_ = tdmfile;
  tdxfile_ = tdxfile;

  // start processing tdm data model
  this->process_tdm(showlog);
}

void tdm_reaper::process_tdm(bool showlog)
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
    // load XML document from stream
    std::ifstream fin(tdmfile_.c_str());
    xml_result_ = xml_doc_.load(fin);
    fin.close();
    // xml_result_ = xml_doc_.load_file(tdmfile_.c_str());

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
  this->process_root(showlog);
  this->process_channelgroups(showlog);
  this->process_channels(showlog);
  this->process_submatrices(showlog);
  this->process_localcolumns(showlog);
}

void tdm_reaper::process_include(bool showlog)
{
  // get XML node
  pugi::xml_node tdmincl = xml_doc_.child("usi:tdm").child("usi:include");

  // check endianness
  std::string endianness(tdmincl.child("file").attribute("byteOrder").value());
  endianness_ = endianness.compare("littleEndian") == 0 ? true : false;

  // check referenced .tdx file
  std::string urltdx(tdmincl.child("file").attribute("url").value());

  // obtain machine's endianness
  int num = 1;
  machine_endianness_ = ( *(char*)&num == 1 );
  if ( machine_endianness_ != endianness_ ) throw std::runtime_error("endianness mismatch");

  if ( showlog )
  {
    std::cout<<"\n";
    std::cout<<"endianness:         "<<(endianness_?"little":"big")<<"\n"
             <<"machine endianness: "<<(machine_endianness_?"little":"big")<<"\n"
             <<"url:                "<<urltdx<<"\n\n";
  }

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

  if ( showlog ) std::cout<<"number of blocks: "<<tdx_blocks_.size()<<"\n\n";
}

void tdm_reaper::process_root(bool showlog)
{
  // get XML node
  pugi::xml_node tdmdataroot = xml_doc_.child("usi:tdm").child("usi:data")
                                                        .child("tdm_root");

  // extract properties
  tdmroot_.id_ = tdmdataroot.attribute("id").value();
  tdmroot_.name_ = tdmdataroot.child_value("name");
  tdmroot_.description_ = tdmdataroot.child_value("description");
  tdmroot_.title_ = tdmdataroot.child_value("title");
  tdmroot_.author_ = tdmdataroot.child_value("author");
  tdmroot_.timestamp_ = tdmdataroot.child_value("datetime");

  // collect channelgroup identifiers associated to root
  tdmroot_.channelgroups_ = this->extract_ids(tdmdataroot.child_value("channelgroups"));

  if ( showlog ) std::cout<<tdmroot_.get_info()<<"\n";
}

void tdm_reaper::process_channelgroups(bool showlog)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc_.child("usi:tdm").child("usi:data");

  // find all its <tdm_channelgroup> elements
  for ( pugi::xml_node group = tdmdata.child("tdm_channelgroup"); group;
                       group = group.next_sibling("tdm_channelgroup") )
  {
    // declare new group
    tdm_channelgroup tdmchannelgroup;

    // extract properties
    tdmchannelgroup.id_ = group.attribute("id").value();
    tdmchannelgroup.name_ = group.child_value("name");
    tdmchannelgroup.description_ = group.child_value("description");
    std::vector<std::string> gr = this->extract_ids(group.child_value("root"));
    if ( gr.size() == 1 )
    {
      tdmchannelgroup.root_ = gr.at(0);
    }
    else
    {
      throw std::runtime_error("tdm_channelgroup with out/multiple root id(s)");
    }
    tdmchannelgroup.channels_ = this->extract_ids(group.child_value("channels"));
    tdmchannelgroup.submatrices_ = this->extract_ids(group.child_value("submatrices"));

    // add channelgroup to map
    tdmchannelgroups_.insert( std::pair<std::string,tdm_channelgroup>(
                                      tdmchannelgroup.id_,tdmchannelgroup) );

    if ( showlog ) std::cout<<tdmchannelgroup.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of channelgroups: "<<tdmchannelgroups_.size()<<"\n\n";
}

void tdm_reaper::process_channels(bool showlog)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc_.child("usi:tdm").child("usi:data");

  // find all its <tdm_channel> elements
  for ( pugi::xml_node channel = tdmdata.child("tdm_channel"); channel;
                       channel = channel.next_sibling("tdm_channel") )
  {
    // declare new channel
    tdm_channel tdmchannel;

    // extract properties
    tdmchannel.id_ = channel.attribute("id").value();
    tdmchannel.name_ = channel.child_value("name");
    tdmchannel.description_ = channel.child_value("description");
    tdmchannel.unit_string_ = channel.child_value("unit_string");
    tdmchannel.datatype_ = channel.child_value("datatype");
    std::string chmin = channel.child_value("minimum");
    chmin = chmin.empty() ? std::string("0.0") : chmin;
    tdmchannel.minimum_ = std::stod(chmin);
    std::string chmax = channel.child_value("maximum");
    chmax = chmax.empty() ? std::string("0.0") : chmax;
    tdmchannel.maximum_ = std::stod(chmax);
    std::vector<std::string> cg = this->extract_ids(channel.child_value("group"));
    if ( cg.size() == 1 )
    {
      tdmchannel.group_ = cg.at(0);
    }
    else
    {
      throw std::runtime_error("tdm_channel with out/multiple group id(s)");
    }
    tdmchannel.local_columns_ = this->extract_ids(channel.child_value("local_columns"));

    // add channel to map
    tdmchannels_.insert( std::pair<std::string,tdm_channel>(tdmchannel.id_,tdmchannel) );

    if ( showlog ) std::cout<<tdmchannel.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of channels: "<<tdmchannels_.size()<<"\n\n";
}

void tdm_reaper::process_submatrices(bool showlog)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc_.child("usi:tdm").child("usi:data");

  // find all its <submatrix> elements
  for ( pugi::xml_node subm = tdmdata.child("submatrix"); subm;
                       subm = subm.next_sibling("submatrix") )
  {
    // declare new submatrix
    submatrix submat;

    // extract properties
    submat.id_ = subm.attribute("id").value();
    submat.name_ = subm.child_value("name");
    submat.description_ = subm.child_value("description");
    std::vector<std::string> mid = this->extract_ids(subm.child_value("measurement"));
    if ( mid.size() == 1 )
    {
      submat.measurement_ = mid.at(0);
    }
    else
    {
      throw std::runtime_error("submatrix with out/multiple measurement id(s)");
    }
    submat.local_columns_ = this->extract_ids(subm.child_value("local_columns"));
    std::string numrows = subm.child_value("number_of_rows");
    numrows = numrows.empty() ? std::string("0") : numrows; 
    submat.number_of_rows_ = std::stoul(numrows);

    // add submatrix to map
    submatrices_.insert( std::pair<std::string,submatrix>(submat.id_,submat) );

    if ( showlog ) std::cout<<submat.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of submatrices: "<<submatrices_.size()<<"\n\n";
}

void tdm_reaper::process_localcolumns(bool showlog)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc_.child("usi:tdm").child("usi:data");

  // find all its <localcolumn> elements
  for ( pugi::xml_node loccol = tdmdata.child("localcolumn"); loccol;
                       loccol = loccol.next_sibling("localcolumn") )
  {
    // declare new localcolumn
    localcolumn locc;

    // extract properties
    locc.id_ = loccol.attribute("id").value();
    locc.name_ = loccol.child_value("name");
    locc.description_ = loccol.child_value("description");
    std::vector<std::string> mq = this->extract_ids(loccol.child_value("measurement_quantity"));
    if ( mq.size() == 1 )
    {
      locc.measurement_quantity_ = mq.at(0);
    }
    else
    {
      throw std::runtime_error("localcolumn with out/multiple measurement quantity id(s)");
    }
    std::vector<std::string> sm = this->extract_ids(loccol.child_value("submatrix"));
    if ( sm.size() == 1 )
    {
      locc.submatrix_ = sm.at(0);
    }
    else
    {
      throw std::runtime_error("localcolumn with out/multiple submatrix id(s)");
    }
    std::string lcmin = loccol.child_value("minimum");
    lcmin = lcmin.empty() ? std::string("0.0") : lcmin;
    locc.minimum_ = std::stod(lcmin);
    std::string lcmax = loccol.child_value("maximum");
    lcmax = lcmax.empty() ? std::string("0.0") : lcmax;
    locc.maximum_ = std::stod(lcmax);
    locc.sequence_representation_ = loccol.child_value("sequence_representation");
    // TODO
    // .... loccal.child_value("generation_parameters");

    std::vector<std::string> vl = this->extract_ids(loccol.child_value("values"));
    if ( vl.size() == 1 )
    {
      locc.values_ = vl.at(0);
    }
    else
    {
      throw std::runtime_error("localcolumn with out/multiple values id(s)");
    }

    // add localcolumn to map
    localcolumns_.insert( std::pair<std::string,localcolumn>(locc.id_,locc) );

    if ( showlog ) std::cout<<locc.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of localcolumns: "<<localcolumns_.size()<<"\n\n";
}


// -------------------------------------------------------------------------- //
