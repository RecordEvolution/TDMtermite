// -------------------------------------------------------------------------- //

#include "tdm_termite.hpp"

// -------------------------------------------------------------------------- //

tdm_termite::tdm_termite()
{
  tdx_ifstream_ = new std::ifstream;
}

tdm_termite::tdm_termite(std::string tdmfile, std::string tdxfile, bool showlog):
  tdmfile_(tdmfile), tdxfile_(tdxfile)
{
  tdx_ifstream_ = new std::ifstream;

  // start processing tdm data model
  this->process_tdm(showlog);
}

tdm_termite::~tdm_termite()
{
  // close tdx-file stream and free memory
  if ( tdx_ifstream_->is_open() ) tdx_ifstream_->close();
  delete tdx_ifstream_;
}

tdm_termite::tdm_termite(const tdm_termite& other):
  tdmfile_(other.tdmfile_), tdxfile_(other.tdxfile_), csvfile_(other.csvfile_),
  endianness_(other.endianness_), machine_endianness_(other.machine_endianness_),
  meta_data_(other.meta_data_), tdmdt_name_(other.tdmdt_name_),
  tdmdt_chan_(other.tdmdt_chan_),
  tdx_blocks_(other.tdx_blocks_), tdmroot_(other.tdmroot_),
  tdmchannelgroups_(other.tdmchannelgroups_), tdmchannels_(other.tdmchannels_),
  tdmchannels_data_(other.tdmchannels_data_), submatrices_(other.submatrices_),
  localcolumns_(other.localcolumns_), tdxbuffer_(other.tdxbuffer_)
{
  tdx_ifstream_ = new std::ifstream;
  if ( other.tdx_ifstream_->is_open() )
  {
    tdx_ifstream_->open(tdxfile_.c_str(),std::ifstream::binary);
    tdx_ifstream_->seekg(other.tdx_ifstream_->tellg());
  }
}

tdm_termite& tdm_termite::operator=(const tdm_termite& other)
{
  if ( this != &other )
  {
    tdmfile_ = other.tdmfile_;
    tdxfile_ = other.tdxfile_;
    csvfile_ = other.csvfile_;
    endianness_ = other.endianness_;
    machine_endianness_ = other.machine_endianness_;
    meta_data_ = other.meta_data_;
    tdmdt_name_ = other.tdmdt_name_;
    tdmdt_chan_= other.tdmdt_chan_;
    tdx_blocks_ = other.tdx_blocks_;
    tdmroot_ = other.tdmroot_;
    tdmchannelgroups_ = other.tdmchannelgroups_;
    tdmchannels_ = other.tdmchannels_;
    tdmchannels_data_ = other.tdmchannels_data_;
    submatrices_ = other.submatrices_;
    localcolumns_ = other.localcolumns_;
    tdxbuffer_ = other.tdxbuffer_;
    if ( other.tdx_ifstream_->is_open() )
    {
      tdx_ifstream_->open(tdxfile_.c_str(),std::ifstream::binary);
      tdx_ifstream_->seekg(other.tdx_ifstream_->tellg());
    }
  }

  return *this;
}

void tdm_termite::submit_files(std::string tdmfile, std::string tdxfile, bool showlog)
{
  // save files
  tdmfile_ = tdmfile;
  tdxfile_ = tdxfile;

  // reset and clear all members
  tdx_blocks_.clear();
  tdmchannelgroups_.clear();
  tdmchannels_.clear();
  submatrices_.clear();
  localcolumns_.clear();
  tdxbuffer_.clear();

  // start processing tdm data model
  this->process_tdm(showlog);
}

void tdm_termite::process_tdm(bool showlog)
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
  pugi::xml_document xml_doc;
  pugi::xml_parse_result xml_result;
  try {
    // load XML document from stream
    std::ifstream fin(tdmfile_.c_str());
    xml_result = xml_doc.load(fin);
    fin.close();
    // xml_result = xml_doc_.load_file(tdmfile_.c_str());

    if ( showlog )
    {
      std::cout<<"\nloading "<<tdmfile_<<": "<<xml_result.description()<<"\n";
      std::cout<<"encoding: "<<(pugi::xml_encoding)xml_result.encoding<<"\n\n";
    }

    // check XML parse result
    if ( xml_result.status != 0 )
    {
      throw std::runtime_error( std::string("failed to parse XML tree: " )
                              + xml_result.description() );
    }
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("failed to load tdm file: ") + e.what());
  }

    // collect meta-data
    pugi::xml_node tdmdocu = xml_doc.child("usi:tdm").child("usi:documentation");
    meta_data_.docu_expo_ = tdmdocu.child_value("usi:exporter");
    meta_data_.docu_expover_ = tdmdocu.child_value("usi:exporterVersion");
    pugi::xml_node tdmmodel = xml_doc.child("usi:tdm").child("usi:model");
    meta_data_.model_name_ = tdmmodel.attribute("modelName").value();
    meta_data_.model_version_ = tdmmodel.attribute("modelVersion").value();
    meta_data_.model_include_uri_ = tdmmodel.child("usi:include").attribute("nsUri").value();
    //
    pugi::xml_node tdmincl = xml_doc.child("usi:tdm").child("usi:include");
    meta_data_.byte_order_ = tdmincl.child("file").attribute("byteOrder").value();
    meta_data_.file_url_ = tdmincl.child("file").attribute("url").value();

    if ( showlog ) std::cout<<meta_data_.get_info()<<"\n";

  // check datatype consistency, i.e. "local" representation of datatypes
  // and build map(s) for "tdm_datatypes"
  this->check_datatype_consistency();
  for ( tdm_datatype el: tdm_datatypes )
  {
    tdmdt_name_.insert(std::pair<std::string,tdm_datatype>(el.name_,el));
    tdmdt_chan_.insert(std::pair<std::string,tdm_datatype>(el.channel_datatype_,el));
  }

  // process elements of XML
  this->process_include(showlog,xml_doc);
  this->process_root(showlog,xml_doc);
  this->process_channelgroups(showlog,xml_doc);
  this->process_channels(showlog,xml_doc);
  this->process_submatrices(showlog,xml_doc);
  this->process_localcolumns(showlog,xml_doc);

  // open .tdx and stream all binary data into buffer
  // try {
  //   std::ifstream fin(tdxfile_.c_str(),std::ifstream::binary);
  //   // if ( !fin.good() ) std::cerr<<"failed to open .tdx-file\n";
  //
  //   std::vector<unsigned char> tdxbuf((std::istreambuf_iterator<char>(fin)),
  //                                     (std::istreambuf_iterator<char>()));
  //   tdxbuffer_ = tdxbuf;
  //
  //   if ( showlog ) std::cout<<"size of .tdx buffer (bytes): "<<tdxbuffer_.size()<<"\n\n";
  //
  //   // close .tdx file
  //   fin.close();
  // } catch (const std::exception& e ) {
  //   throw std::runtime_error( std::string("failed to open .tdx and stream data to buffer: ")
  //                             + e.what() );
  // }
  try {
    tdx_ifstream_->open(tdxfile_.c_str(),std::ifstream::binary);
  } catch (const std::exception& e) {
    throw std::runtime_error( std::string("failed to open .tdx file in ifstream: ")
                              + e.what() );
  }
}

void tdm_termite::process_include(bool showlog, pugi::xml_document& xml_doc)
{
  // get XML node
  pugi::xml_node tdmincl = xml_doc.child("usi:tdm").child("usi:include");

  // check endianness
  std::string endianness(tdmincl.child("file").attribute("byteOrder").value());
  endianness_ = endianness.compare("littleEndian") == 0 ? true : false;

  // obtain machine's endianness
  int num = 1;
  machine_endianness_ = ( *(char*)&num == 1 );
  if ( machine_endianness_ != endianness_ ) throw std::runtime_error("endianness mismatch");

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

void tdm_termite::process_root(bool showlog, pugi::xml_document& xml_doc)
{
  // get XML node
  pugi::xml_node tdmdataroot = xml_doc.child("usi:tdm").child("usi:data")
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

void tdm_termite::process_channelgroups(bool showlog, pugi::xml_document& xml_doc)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc.child("usi:tdm").child("usi:data");

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

void tdm_termite::process_channels(bool showlog, pugi::xml_document& xml_doc)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc.child("usi:tdm").child("usi:data");

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
      throw std::logic_error("tdm_channel with out/multiple group id(s)");
    }
    tdmchannel.local_columns_ = this->extract_ids(channel.child_value("local_columns"));

    // add channel to map
    tdmchannels_.insert( std::pair<std::string,tdm_channel>(tdmchannel.id_,tdmchannel) );

    if ( showlog ) std::cout<<tdmchannel.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of channels: "<<tdmchannels_.size()<<"\n\n";
}

void tdm_termite::process_submatrices(bool showlog, pugi::xml_document& xml_doc)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc.child("usi:tdm").child("usi:data");

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
      throw std::logic_error("submatrix with out/multiple measurement id(s)");
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

void tdm_termite::process_localcolumns(bool showlog, pugi::xml_document& xml_doc)
{
  // get XML node <usi:data>
  pugi::xml_node tdmdata = xml_doc.child("usi:tdm").child("usi:data");

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
      throw std::logic_error("localcolumn with out/multiple measurement quantity id(s)");
    }
    std::vector<std::string> sm = this->extract_ids(loccol.child_value("submatrix"));
    if ( sm.size() == 1 )
    {
      locc.submatrix_ = sm.at(0);
    }
    else
    {
      throw std::logic_error("localcolumn with out/multiple submatrix id(s)");
    }
    std::string lcmin = loccol.child_value("minimum");
    lcmin = lcmin.empty() ? std::string("0.0") : lcmin;
    locc.minimum_ = std::stod(lcmin);
    std::string lcmax = loccol.child_value("maximum");
    lcmax = lcmax.empty() ? std::string("0.0") : lcmax;
    locc.maximum_ = std::stod(lcmax);
    locc.sequence_representation_ = loccol.child_value("sequence_representation");
    std::string genpar = loccol.child_value("generation_parameters");
    // check for any given generation parameters (applies to 'implicit_linear' channels only)
    if ( !genpar.empty() )
    {
      // check for two floats
      std::vector<std::string> params = this->split(genpar,std::string(" "));
      if ( params.size() == 2 )
      {
        // remove default elements and insert new numbers
        locc.generation_parameters_.clear();
        for ( std::string el: params )
        {
          locc.generation_parameters_.push_back(std::stod(el));
        }
      }
    }

    std::vector<std::string> vl = this->extract_ids(loccol.child_value("values"));
    if ( vl.size() == 1 )
    {
      locc.values_ = vl.at(0);
    }
    else if ( vl.size() == 0 )
    {
      //std::cerr<<"localcolumn ("<<locc.id_<<","<<locc.name_<<") misses any value-ids"<<"\n";
      locc.values_ = "none";
    }
    else
    {
      throw std::logic_error("localcolumn with multiple values id(s)");
    }

    // add external id referring to block in <usi:include>
    {
      // relying on fully initialized "tdmchannels_" !!)
      if ( tdmchannels_.size() == 0 ) throw std::logic_error("tdmchannels_ not initialized");

      // determine "channel_datatype_" and map it to its sequence type
      if ( tdmchannels_.count(locc.measurement_quantity_) != 1 )
      {
        throw std::runtime_error(std::string("measurement_quantity: ")
                                 + locc.measurement_quantity_
                                 + std::string(" is missing/ambiguous") );
      }
      std::string dt = tdmchannels_.at(locc.measurement_quantity_).datatype_;
      std::string sequence_type;
      if ( tdmdt_chan_.count(dt) != 1 )
      {
        throw std::runtime_error(std::string("datatype: ") + dt
                                 + std::string(" is unknown/invalid") );
      }
      sequence_type = tdmdt_chan_.at(dt).value_sequence_;

      for ( pugi::xml_node seq = tdmdata.child(sequence_type.c_str()); seq;
                           seq = seq.next_sibling(sequence_type.c_str()) )
      {
        if ( seq.attribute("id").value() == locc.values_ )
        {
          locc.external_id_ = seq.child("values").attribute("external").value();
        }
      }

      if ( locc.external_id_.empty() )
      {
        //throw std::logic_error( std::string("no external id found for ")
        //                      + sequence_type + std::string(" with ") + locc.values_ );
        //std::cerr<<"no external id found for "<<sequence_type<<" with "<<locc.values_<<"\n";
        locc.external_id_ = "none";
      }
    }

    // add localcolumn to map
    localcolumns_.insert( std::pair<std::string,localcolumn>(locc.id_,locc) );

    if ( showlog ) std::cout<<locc.get_info()<<"\n";
  }

  if ( showlog ) std::cout<<"number of localcolumns: "<<localcolumns_.size()<<"\n\n";
}

// -------------------------------------------------------------------------- //

std::string tdm_termite::get_channel_overview(format chformatter)
{
  // summarize all output in single string
  std::string channels_summary;

  // set tabular mode of formatter
  chformatter.set_tabular(true);

  // compose header
  chformatter.set_header(true);
  //tdm_channelgroup grp;
  //channels_summary += grp.get_info(chformatter);
  tdm_channel chn;
  channels_summary += chn.get_info(chformatter);
  std::string rule; // = std::string("#");
  for ( unsigned long int i = 0; i < channels_summary.size(); i++ )
  {
    rule += std::string("-");
  }
  // rule += std::string("#");
  channels_summary = // std::string("# ") +
                channels_summary + std::string("\n") + rule + std::string("\n");

  chformatter.set_header(false);
  for (std::map<std::string,tdm_channel>::iterator it=tdmchannels_.begin();
                                                   it!=tdmchannels_.end(); ++it)
  {
    // get corresponding group
    // tdm_channelgroup grp = tdmchannelgroups_.at(it->second.group_);
    // channels_summary += grp.get_info(chformatter);

    // ...and actual channel
    channels_summary += it->second.get_info(chformatter);
    channels_summary += std::string("\n");
  }

  return channels_summary;
}

template<typename tdmelement>
std::string tdm_termite::get_overview(format formatter)
{
  // summarize all output in single string
  std::string summary;

  // set tabular mode of formatter
  formatter.set_tabular(true);

  // compose header
  formatter.set_header(true);
  tdmelement tdmel;
  summary += tdmel.get_info(formatter);
  std::string rule;
  for ( unsigned long int i = 0; i < summary.size(); i++ )
  {
    rule += std::string("-");
  }
  summary += std::string("\n") + rule + std::string("\n");

  // write body of summary with data
  formatter.set_header(false);
  this->summarize_member(tdmel,summary,formatter);

  return summary;
}

template std::string tdm_termite::get_overview<tdm_channelgroup>(format formatter);
template std::string tdm_termite::get_overview<submatrix>(format formatter);
template std::string tdm_termite::get_overview<localcolumn>(format formatter);
template std::string tdm_termite::get_overview<block>(format formatter);

void tdm_termite::summarize_member(tdm_channelgroup &chp, std::string& summary, format& formatter)
{
  for ( std::map<std::string,tdm_channelgroup>::iterator it=this->tdmchannelgroups_.begin();
                                                         it!=this->tdmchannelgroups_.end(); ++it)
  {
    summary += it->second.get_info(formatter);
    summary += std::string("\n");
  }
}

void tdm_termite::summarize_member(submatrix &sbm, std::string& summary, format& formatter)
{
  for ( std::map<std::string,submatrix>::iterator it=this->submatrices_.begin();
                                                  it!=this->submatrices_.end(); ++it)
  {
    summary += it->second.get_info(formatter);
    summary += std::string("\n");
  }
}

void tdm_termite::summarize_member(localcolumn &lcc, std::string& summary, format& formatter)
{
  for ( std::map<std::string,localcolumn>::iterator it=this->localcolumns_.begin();
                                                    it!=this->localcolumns_.end(); ++it)
  {
    summary += it->second.get_info(formatter);
    summary += std::string("\n");
  }
}

void tdm_termite::summarize_member(block &blk, std::string& summary, format& formatter)
{
  for ( std::map<std::string,block>::iterator it=this->tdx_blocks_.begin();
                                              it!=this->tdx_blocks_.end(); ++it)
  {
    summary += it->second.get_info(formatter);
    summary += std::string("\n");
  }
}

// -------------------------------------------------------------------------- //

// extract channel by id
std::vector<tdmdatatype> tdm_termite::get_channel(std::string& id)
{
  // check for existence of required channel id (=key)
  if ( tdmchannels_.count(id) == 1 )
  {
    // retrieve full channel info
    tdm_channel chn = tdmchannels_.at(id);

    // extract (first) "localcolumn" for channel TODO there should only be a single!! local_column!!
    if ( chn.local_columns_.size() != 1 )
    {
      throw std::runtime_error(std::string("invalid local_columns_ of channel: ") + id);
    }
    localcolumn loccol = localcolumns_.at(chn.local_columns_[0]);

    // check sequence_representation
    if ( loccol.sequence_representation_ != "explicit"
      && loccol.sequence_representation_ != "implicit_linear"
      && loccol.sequence_representation_ != "raw_linear" )
    {
      throw std::runtime_error(std::string("unsupported sequence_representation: ")
                                + loccol.sequence_representation_ );
    }

    // use "values" id to map to external block
    if ( loccol.external_id_ == "none" )
    {
      //throw std::runtime_error(std::string("missing external_id in local_column ")+loccol.id_);
      //std::cerr<<"missing external_id in local_column "<<loccol.id_<<"\n";
      return std::vector<tdmdatatype>(0);
    }
    block blk = tdx_blocks_.at(loccol.external_id_);

    // find corresponding submatrix
    if ( submatrices_.count(loccol.submatrix_) != 1 )
    {
      throw std::runtime_error(std::string("no associated submatrix for localcolumn found: ") + loccol.id_);
    }
    submatrix subm = submatrices_.at(loccol.submatrix_);
    if ( subm.number_of_rows_ != blk.length_ )
    {
      std::stringstream ss;
      ss<<"number of rows in submatrix "<<subm.id_<<" ("<<subm.number_of_rows_<<") "
        <<" does not agree with length of associated block "<<blk.id_<<" ("<<blk.length_<<")";
      throw std::runtime_error(ss.str());
    }

    // declare vector of appropriate length
    std::vector<tdmdatatype> datavec(blk.length_);

    // retrieve corresponding TDM datatype
    tdm_datatype dtyp = this->tdmdt_name_.at(blk.value_type_);

    // declare buffer covering the required range of "tdxbuffer_"
    // (consider both channel-wise and block-wise ordering)
    unsigned long int strtidx = blk.block_offset_*blk.block_size_
                              + blk.byte_offset_;
                      // fnshidx = strtidx + blk.length_*dtyp.size_;
    // std::vector<unsigned char> tdxblk( tdxbuffer_.begin()+strtidx,
    //                                    tdxbuffer_.begin()+fnshidx );
    char* blkbuf = new char[blk.length_*dtyp.size_];
    try {
      tdx_ifstream_->seekg(strtidx);
      tdx_ifstream_->read(blkbuf,blk.length_*dtyp.size_);
    } catch (const std::exception& e) {
      throw std::runtime_error( std::string("failed to read block from tdx_ifstream_: ")
                                + e.what() );
    }
    std::vector<unsigned char> tdxblk(blkbuf,blkbuf+blk.length_*dtyp.size_);
    delete []blkbuf;

    // distinguish numeric datatypes included in "tdmdatatype"
    if ( blk.value_type_ == std::string("eInt16Usi") )
    {
      this->convert_data_to_type<eInt16Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eInt32Usi") )
    {
      this->convert_data_to_type<eInt32Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eUInt8Usi") )
    {
      this->convert_data_to_type<eUInt8Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eUInt16Usi") )
    {
      this->convert_data_to_type<eUInt16Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eUInt32Usi") )
    {
      this->convert_data_to_type<eUInt32Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eFloat32Usi") )
    {
      this->convert_data_to_type<eFloat32Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eFloat64Usi") )
    {
      this->convert_data_to_type<eFloat64Usi>(tdxblk,datavec);
    }
    else if ( blk.value_type_ == std::string("eStringUsi") )
    {
      this->convert_data_to_type<eStringUsi>(tdxblk,datavec);
    }
    else
    {
      throw std::runtime_error(std::string("unsupported/unknown datatype") + blk.value_type_);
    }

    // apply offset and factor for implicit_linear and raw_linear representation
    if ( loccol.sequence_representation_ == "implicit_linear"
      || loccol.sequence_representation_ == "raw_linear" )
    {
      // datatype has to be 'DT_DOUBLE' for these representations
      if ( chn.datatype_ != std::string("DT_DOUBLE") )
      {
        throw std::runtime_error( std::string("inconsistent sequence_representation and datatype: ")
                                  + chn.id_ + std::string(",") + loccol.sequence_representation_
                                  + std::string(",") + chn.datatype_ );
      }

      // scale and shift channel
      for ( auto &el: datavec )
      {
        el = loccol.generation_parameters_[0]
             + el.as_double()*loccol.generation_parameters_[1];
      }
    }

    return datavec;
  }
  else
  {
    throw std::invalid_argument(std::string("channel id does not exist: ") + id);
  }
}

// -------------------------------------------------------------------------- //

void tdm_termite::print_channel(std::string &id, const char* filename, bool include_meta)
{
  // check required path
  this->check_filename_path(filename);

  // check for channel id
  if ( this->tdmchannels_.count(id) != 1 )
  {
    throw std::invalid_argument(std::string("channel id does not exist: ") + id);
  }
  else
  {
    // declare file stream
    std::ofstream fou;
    try {
      fou.open(filename);
    } catch ( const std::exception& e) {
      throw std::runtime_error( std::string("failed to open file to dump channel")
                                + e.what() );
    }

    // get channel object
    tdm_channel chn = this->tdmchannels_.at(id);
    if ( include_meta )
    {
      int width = 20;
      fou<<std::setw(width)<<std::left<<"# channel-id:"<<chn.id_<<"\n";
      fou<<std::setw(width)<<std::left<<"# name:"<<chn.name_<<"\n";
      fou<<std::setw(width)<<std::left<<"# description:"<<chn.description_<<"\n";
      fou<<std::setw(width)<<std::left<<"# unit_string:"<<chn.unit_string_<<"\n";
      fou<<std::setw(width)<<std::left<<"# datatype:"<<chn.datatype_<<"\n";
      fou<<std::setw(width)<<std::left<<"# minimum:"<<chn.minimum_<<"\n";
      fou<<std::setw(width)<<std::left<<"# maximum:"<<chn.maximum_<<"\n";
      fou<<std::setw(width)<<std::left<<"# group:"<<chn.group_<<"\n";
    }

    // obtain channel data
    std::vector<tdmdatatype> chndata = this->get_channel(id);
    for ( auto el: chndata ) fou<<el<<"\n";

    // close file
    fou.close();
  }
}

void tdm_termite::print_group(std::string &id, const char* filename, bool include_meta,
                             char sep, std::string column_header)
{
  // check required path
  this->check_filename_path(filename);

  // check for group id
  if ( this->tdmchannelgroups_.count(id) != 1 )
  {
    throw std::invalid_argument(std::string("channelgroup id does not exist: ") + id);
  }
  else
  {
    // declare file stream
    std::ofstream fou;
    try {
      fou.open(filename);
    } catch ( const std::exception& e) {
      throw std::runtime_error( std::string("failed to open file to dump group")
                                + e.what() );
    }

    // get group object
    tdm_channelgroup chngrp = this->tdmchannelgroups_.at(id);

    int width = 25;

    // file header
    if ( include_meta )
    {
      // group meta data
      fou<<"# "<<std::setw(width)<<std::left<<"group-id:"<<chngrp.id_<<"\n";
      fou<<"# "<<std::setw(width)<<std::left<<"name:"<<chngrp.name_<<"\n";
      fou<<"# "<<std::setw(width)<<std::left<<"description:"<<chngrp.description_<<"\n";
      fou<<"# "<<std::setw(width)<<std::left<<"root:"<<chngrp.root_<<"\n";
      fou<<"# "<<std::setw(width)<<std::left<<"channels:"<<join_strings(chngrp.channels_)<<"\n";
      fou<<"# \n";

      // print channels's meta data
      std::vector<tdm_channel> grpschs;
      for ( std::string chn: chngrp.channels_ )
      {
        if ( this->tdmchannels_.count(chn) == 1 )
        {
          grpschs.push_back(this->tdmchannels_.at(chn));
        }
        else
        {
          throw std::runtime_error("channel not found");
        }
      }

      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("channel-id:  "+chn.id_);
      fou<<"\n";
      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("name:        "+chn.name_);
      fou<<"\n";
      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("description: "+chn.description_);
      fou<<"\n";
      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("unit_string: "+chn.unit_string_);
      fou<<"\n";
      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("minimum:     "+std::to_string(chn.minimum_));
      fou<<"\n";
      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("maximum:     "+std::to_string(chn.maximum_));
      fou<<"\n";
      fou<<"# ";
      for ( tdm_channel chn: grpschs ) fou<<std::setw(width)<<std::left<<std::string("group-id:    "+chn.group_);
      fou<<"\n";
    }

    // collect channel data
    std::vector<std::vector<tdmdatatype>> allchns;
    unsigned int maxrows = 0; // TODO use submatrix info to determine rows!!!
    for ( std::string chn: chngrp.channels_ )
    {
      std::vector<tdmdatatype> chndat = this->get_channel(chn);
      if ( chndat.size() > maxrows ) maxrows = (unsigned int)chndat.size();
      allchns.push_back(chndat);
    }

    // provide column header (with channel ids)
    if ( column_header.empty() )
    {
      for ( std::string chn: chngrp.channels_ )
      {
        // use given csv separator token
        if ( sep == ' ' )
        {
          fou<<std::setw(width)<<std::left<<chn;
        }
        else
        {
          fou<<chn;
        }
        if ( chn != chngrp.channels_.back() && sep != ' ' ) fou<<sep;
      }
    }
    else
    {
      fou<<column_header;
    }
    fou<<"\n";

    // write data
    for ( unsigned int row = 0; row < maxrows; row++ )
    {
      for ( unsigned int chi = 0; chi < chngrp.channels_.size(); chi++ )
      {
        if ( allchns.at(chi).size() > row )
        {
          // use given csv separator token
          if ( sep == ' ' )
          {
            fou<<std::setw(width)<<std::left<<allchns.at(chi).at(row);
          }
          else
          {
            fou<<allchns.at(chi).at(row);
          }
        }

        if ( chi+1 < chngrp.channels_.size() && sep != ' ' ) fou<<sep;
      }
      fou<<"\n";
    }

    // close file
    fou.close();
  }
}

void tdm_termite::check_filename_path(const char* filename)
{
  // declare filesystem path instance from filename
  std::filesystem::path pt(filename);

  // get pure directory path
  pt.remove_filename();

  if ( !std::filesystem::is_directory(pt) )
  {
    throw std::runtime_error( std::string("directory does not exist: ") + pt.u8string() );
  }
}

// -------------------------------------------------------------------------- //

void tdm_termite::check_local_datatypes()
{
  std::cout<<"\nmachine's C++ datatypes:\n";
  std::cout<<std::setw(25)<<std::left<<"char:"
           <<std::setw(5)<<std::left<<sizeof(char)<<"byte(s)\n"
           <<std::setw(25)<<std::left<<"uint8_t:"
           <<std::setw(5)<<std::left<<sizeof(uint8_t)<<"byte(s)\n"

           <<std::setw(25)<<std::left<<"short int:"
           <<std::setw(5)<<std::left<<sizeof(short int)<<"byte(s)\n"
           <<std::setw(25)<<std::left<<"unsigned short int:"
           <<std::setw(5)<<std::left<<sizeof(unsigned short int)<<"byte(s)\n"

           <<std::setw(25)<<std::left<<"int:"
           <<std::setw(5)<<std::left<<sizeof(int)<<"byte(s)\n"
           <<std::setw(25)<<std::left<<"unsigned int:"
           <<std::setw(5)<<std::left<<sizeof(unsigned int)<<"byte(s)\n"

           <<std::setw(25)<<std::left<<"long int:"
           <<std::setw(5)<<std::left<<sizeof(long int)<<"byte(s)\n"
           <<std::setw(25)<<std::left<<"unsigned long int:"
           <<std::setw(5)<<std::left<<sizeof(unsigned long int)<<"byte(s)\n"

           <<std::setw(25)<<std::left<<"float:"
           <<std::setw(5)<<std::left<<sizeof(float)<<"byte(s)\n"
           <<std::setw(25)<<std::left<<"double:"
           <<std::setw(5)<<std::left<<sizeof(double)<<"byte(s)\n"
           <<std::setw(25)<<std::left<<"long double:"
           <<std::setw(5)<<std::left<<sizeof(long double)<<"byte(s)\n\n";
}

void tdm_termite::check_datatype_consistency()
{
  // check datatype consistency, i.e. "local" representation of datatypes
  for ( tdm_datatype el: tdm_datatypes )
  {
    if ( el.name_ == "eInt16Usi" )
    {
      if ( el.size_ != sizeof(eInt16Usi) ) throw std::logic_error("invalid representation of eInt16Usi");
    }
    else if ( el.name_ == "eInt32Usi" )
    {
      if ( el.size_ != sizeof(eInt32Usi) ) throw std::logic_error("invalid representation of eInt32Usi");
    }
    else if ( el.name_ == "eUInt8Usi" )
    {
      if ( el.size_ != sizeof(eUInt8Usi) ) throw std::logic_error("invalid representation of eUInt8Usi");
    }
    else if ( el.name_ == "eUInt16Usi" )
    {
      if ( el.size_ != sizeof(eUInt16Usi) ) throw std::logic_error("invalid representation of eUInt16Usi");
    }
    else if ( el.name_ == "eUInt32Usi" )
    {
      if ( el.size_ != sizeof(eUInt32Usi) ) throw std::logic_error("invalid representation of eUInt32Usi");
    }
    else if ( el.name_ == "eFloat32Usi" )
    {
      if ( el.size_ != sizeof(eFloat32Usi) ) throw std::logic_error("invalid representation of eFloat32Usi");
    }
    else if ( el.name_ == "eFloat64Usi" )
    {
      if ( el.size_ != sizeof(eFloat64Usi) ) throw std::logic_error("invalid representation of eFloat64Usi");
    }
    else if ( el.name_ == "eStringUsi" )
    {
      if ( el.size_ != sizeof(eStringUsi) ) throw std::logic_error("invalid representation of eStringUsi");
    }
    else
    {
      throw std::logic_error("missing datatype validation");
    }
  }
}

// -------------------------------------------------------------------------- //

template<typename datatype>
void tdm_termite::convert_data_to_type(std::vector<unsigned char> &buffer,
                                      std::vector<tdmdatatype> &channel)
{
  // check number of elements of type "datatype" in buffer
  if ( buffer.size() != channel.size()*sizeof(datatype) )
  {
    throw std::runtime_error("size mismatch between buffer and datatype");
  }

  // extract every single number of type "datatype" from buffer
  for ( unsigned long int i = 0; i < channel.size(); i++ )
  {
    // declare number of required type and point it to first byte in buffer
    // representing the number
    datatype df;
    uint8_t* dfcast = reinterpret_cast<uint8_t*>(&df);

    for ( unsigned long int j = 0; j < sizeof(datatype); j++ )
    {
      dfcast[j] = (int)buffer[i*sizeof(datatype)+j];
    }

    // save number in channel
    channel[i] = df;
  }
}

// -------------------------------------------------------------------------- //
