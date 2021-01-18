
#include "tdm_ripper.hpp"

tdm_ripper::tdm_ripper(std::string tdmfile, std::string tdxfile,
                       bool suppress_status, bool neglect_empty_groups):
  tdmfile_(tdmfile), tdxfile_(tdxfile), suppress_status_(suppress_status),
  neglect_empty_groups_(neglect_empty_groups), num_empty_groups_(0),
  num_channels_(0), num_groups_(0), channel_id_(0), inc_id_(0), units_(0),
  channel_name_(0), group_id_(0), group_name_(0),
  num_channels_group_(0), channels_group_(0), channel_ext_(0), minmax_(0),
  byteoffset_(0), length_(0), type_(0), external_id_(0)
{
  datatypes_ = {
    {"eInt8Usi",8},
    {"eInt16Usi",16},
    {"eInt32Usi",32},
    {"eInt64Usi",64},
    {"eUInt8Usi",8},
    {"eUInt16Usi",16},
    {"eUInt32Usi",32},
    {"eUInt64Usi",64},
    {"eFloat32Usi",32},
    {"eFloat64Usi",64}
  };

  // make sure the provided file is a .tdm file
  assert( tdmfile_.compare("") != 0 && "please provide a valid .tdm file" );
  std::string::size_type idx;
  idx = tdmfile_.find_last_of(".");
  assert( idx != std::string::npos && "there's no file extension at all - .tdm is required" );
  assert( tdmfile_.substr(tdmfile_.find_last_of(".")+1).compare("tdm") == 0 && "it's not a .tdm file" );

  // setup of xml-parser
  xml_result_ = xml_doc_.load_file(tdmfile_.c_str());
  if ( !suppress_status_ )
  {
    std::cout<<"\nloading and parsing file: "<<xml_result_.description()<<"\n";
    std::cout<<"\nencoding: "<<(pugi::xml_encoding)xml_result_.encoding<<"\n\n";
  }

  pugi::xml_node subtreeincl = xml_doc_.child("usi:tdm").child("usi:include");

  if ( !suppress_status_ )
  {
    std::cout<<"file modified: "<<xml_doc_.child("usi:tdm").child("usi:data")
                          .child("tdm_root").child_value("datetime")<<"\n\n";
  }

  // obtain corresponding .tdx filename given in .tdm file
  if ( tdxfile_.compare("") == 0 )
  {
    tdxfile_ = tdmfile_.substr(0, tdmfile_.find_last_of("\\/"))
                  +std::string("/")+subtreeincl.child("file").attribute("url").value();
  }

  // obtain endianness specified in .tdm file
  std::string endianness(subtreeincl.child("file").attribute("byteOrder").value());
  endianness_ = endianness.compare("littleEndian") == 0 ? true : false;

  // obtain machine's endianess
  int num = 1;
  machine_endianness_ = ( *(char*)&num == 1 );
  assert( machine_endianness_ == endianness_ );

  if ( !suppress_status_ )
  {
    std::cout<<"required .tdx-file is '"<<tdxfile_<<"'\n\n";
  }

  parse_structure();

  // open .tdx and stream all binary data into vector
  std::ifstream fin(tdxfile_.c_str(),std::ifstream::binary);
  assert( fin.good() && "failed to open .tdx-file" );
  if ( !fin.good() ) std::cout<<"failed to open .tdx-file\n\n";
  // assert( errno == 0 );
  // std::cout<<"error code "<<strerror(errno)<<"\n\n";
  std::vector<unsigned char> tdxbuf((std::istreambuf_iterator<char>(fin)),
                                    (std::istreambuf_iterator<char>()));
  tdxbuf_ = tdxbuf;

  if ( !suppress_status_ )
  {
    std::cout<<"number of bytes in binary file: "<<tdxbuf_.size()<<"\n\n";
  }
}

void tdm_ripper::parse_structure()
{
  // get node with channel and endianess information
  pugi::xml_node subtreefile = xml_doc_.child("usi:tdm").child("usi:include").child("file");

  for (pugi::xml_node anode: subtreefile.children())
  {
    // count overall number of channels
    num_channels_++;

    // get byteoffset of channel
    byteoffset_.push_back(atoi(anode.attribute("byteOffset").value()));

    // get length of channel
    length_.push_back(atoi(anode.attribute("length").value()));

    // find datatype of channel
    type_.push_back(anode.attribute("valueType").value());

    // external id of channel
    external_id_.push_back(anode.attribute("id").value());
  }

  // get node with channels and groups
  pugi::xml_node subtreedata = xml_doc_.child("usi:tdm").child("usi:data");

  // extract basic information about available groups
  int groupcount = 0;
  for (pugi::xml_node anode: subtreedata.children())
  {
    // get meta-info contained in tdm_root element
    if ( std::string(anode.name()).compare("tdm_root") == 0 )
    {
      // preliminiary: extract some hard-coded information tags only
      root_info_.insert(std::pair<std::string,std::string>("name",anode.child_value("name")));
      root_info_.insert(std::pair<std::string,std::string>("description",anode.child_value("description")));
      root_info_.insert(std::pair<std::string,std::string>("title",anode.child_value("title")));
      root_info_.insert(std::pair<std::string,std::string>("author",anode.child_value("author")));
    }

    if ( std::string(anode.name()).compare("tdm_channelgroup") == 0 )
    {
      groupcount++;

      for ( pugi::xml_node mnode: anode.child("instance_attributes").children() )
      {
        // preliminiary fix for Conti-TDM files since values are one arbitrary tree level above
        bool pretdmfix = ( std::string(mnode.child_value()).compare("") == 0 ) ? false : true;

        if ( pretdmfix )
        {
          meta_info_.insert(std::pair<std::string,std::string>(mnode.attribute("name").value(),mnode.child_value()));
        }
        else
        {
          meta_info_.insert(std::pair<std::string,std::string>(mnode.attribute("name").value(),mnode.child_value("s")));
        }
      }

      int numchann = count_occ_string(anode.child_value("channels"),"id");
      if ( numchann > 0 || !neglect_empty_groups_ )
      {
        num_groups_++;
        group_id_.push_back(anode.attribute("id").value());
        group_name_.push_back(anode.child_value("name"));
        num_channels_group_.push_back(numchann);

        // get time-stamp
        pugi::xml_node insatt = anode.child("instance_attributes");
        std::pair<std::string,std::string> startstop;
        for ( pugi::xml_node bnode: insatt.children() )
        {
          assert( std::string(bnode.name()).compare("double_attribute") == 0 );
          if ( std::string(bnode.attribute("name").value()).compare("Starttime") == 0 )
          {
            startstop.first = bnode.child_value();
          }
          else if ( std::string(bnode.attribute("name").value()).compare("Stoptime") == 0 )
          {
            startstop.second = bnode.child_value();
          }
          else
          {
            startstop.first = "";
            startstop.second = "";
          }
        }
        group_timestamp_.push_back(startstop);
      }
      if ( numchann == 0 ) num_empty_groups_++;
    }
  }

  // obtain list of xpointers and ids to assign channels
  for (pugi::xml_node anode: subtreedata.children())
  {
    if ( std::string(anode.name()).compare("tdm_channel") == 0 )
    {
      std::string id(anode.attribute("id").value());
      std::string val = get_str_between(anode.child_value("local_columns"),"\"","\"");
      xml_local_columns_.insert(std::pair<std::string,std::string>(id,val));
    }

    if ( std::string(anode.name()).compare("localcolumn") == 0 )
    {
      std::string id(anode.attribute("id").value());
      std::string val = get_str_between(anode.child_value("values"),"\"","\"");
      xml_values_.insert(std::pair<std::string,std::string>(id,val));
    }

    if ( std::string(anode.name()).compare("double_sequence") == 0
      || std::string(anode.name()).compare("long_sequence") == 0 )
    {
      std::string id(anode.attribute("id").value());
      std::string val = anode.child("values").attribute("external").value();
      xml_double_sequence_.insert(std::pair<std::string,std::string>(id,val));
    }
  }

  if ( !suppress_status_ )
  {
    std::cout<<"number of pairs in\n";
    std::cout<<std::setw(25)<<std::left<<"xml_local_columns_:"<<xml_local_columns_.size()<<"\n";
    std::cout<<std::setw(25)<<std::left<<"xml_values_:"<<xml_values_.size()<<"\n";
    std::cout<<std::setw(25)<<std::left<<"xml_double_sequence_:"<<xml_double_sequence_.size()<<"\n";
    std::cout<<std::right<<"\n\n";

    std::cout<<"meta-info snippets "<<meta_info_.size()<<"\n\n";
  }

  // extract basic information about available channels
  // int prog = 0;
  for (pugi::xml_node anode: subtreedata.children())
  {
    if ( std::string(anode.name()).compare("tdm_channel") == 0 )
    {
      // prog++;
      // std::cout<<"processing channel "<<prog<<"\n";

      channel_id_.push_back(anode.attribute("id").value());
      channel_name_.push_back(anode.child_value("name"));
      std::string groupid(anode.child_value("group"));
      for ( int g = 0; g < num_groups_; g++ )
      {
        if ( groupid.find(group_id_[g]) != std::string::npos ) channels_group_.push_back(g);
      }

      // obtain measurement unit of channel
      units_.push_back(anode.child_value("unit_string"));
      if ( (*(units_.end()-1)).compare("°C") == 0 ) (*(units_.end()-1)) = "deg. Celsius";

      // obtain minimum/maximum of channel
      std::pair<double,double> minmaxchan(atof(anode.child_value("minimum")),
                                          atof(anode.child_value("maximum")));
      minmax_.push_back(minmaxchan);

      // get correct assignment of channels to byteoffset, length and datatype
      std::string locolvalext;
      locolvalext = xml_double_sequence_[xml_values_[xml_local_columns_[anode.attribute("id").value()]]];

      // save external id of channel and get corresponding channel index
      inc_id_.push_back(locolvalext);
      int extid = 1;
      for ( int i = 0; i < (int)external_id_.size(); i++ )
      {
        if ( external_id_[i].compare(locolvalext) == 0 ) extid = i;
      }
      channel_ext_.push_back(extid);
    }
  }

  // std::string keyinit("usi23258");
  // std::cout<<"xml test "<<xml_double_sequence_[xml_values_[xml_local_columns_[keyinit]]]<<"\n\n";

  // for ( auto el: minmax_ ) std::cout<<el.first<<"  "<<el.second<<"\n";
  // std::cout<<"\n\n";

  // check consistency of number of channel-groups
  int numgroups = count_occ_string(subtreedata.child("tdm_root").child_value("channelgroups"),"id");
  assert( (neglect_empty_groups_ && numgroups == num_groups_+num_empty_groups_)
       || (!neglect_empty_groups_ && numgroups == num_groups_) );

  // check consistency of number of channels
  assert( num_channels_ == (int)channel_id_.size()
       && num_channels_ == (int)channel_name_.size()
       && num_channels_ == (int)channels_group_.size() );
}

void tdm_ripper::list_channels(std::ostream& gout, int width, int maxshow)
{
  gout<<std::setw(width)<<"index";
  gout<<std::setw(width)<<"id";
  gout<<std::setw(width)<<"inc_id";
  gout<<std::setw(2*width)<<"name";
  gout<<std::setw(width)<<"offset";
  gout<<std::setw(width)<<"length";
  gout<<std::setw(width)<<"datatype";
  gout<<std::setw(width)<<"unit";
  gout<<std::setw(width)<<"minimum";
  gout<<std::setw(width)<<"maximum";
  gout<<std::setw(width)<<"group";
  gout<<std::setw(width)<<"group id";
  gout<<std::setw(width)<<"group name";
  gout<<std::setw(width)<<"num channels";
  gout<<"\n";
  gout<<std::setfill('-')<<std::setw(15*width+1)<<"\n";
  gout<<std::setfill(' ');

  for ( int i = 0; i < num_channels_ && i < maxshow; i++ )
  {
    gout<<std::setw(width)<<i+1;
    gout<<std::setw(width)<<channel_id_[i];
    gout<<std::setw(width)<<inc_id_[i];
    gout<<std::setw(2*width)<<channel_name_[i];
    gout<<std::setw(width)<<byteoffset_[channel_ext_[i]];
    gout<<std::setw(width)<<length_[channel_ext_[i]];
    gout<<std::setw(width)<<type_[channel_ext_[i]];
    gout<<std::setw(width)<<units_[i];
    gout<<std::setw(width)<<minmax_[i].first;
    gout<<std::setw(width)<<minmax_[i].second;
    gout<<std::setw(width)<<channels_group_[i];
    gout<<std::setw(width)<<group_id_[channels_group_[i]];
    gout<<std::setw(width)<<group_name_[channels_group_[i]];
    gout<<std::setw(width)<<num_channels_group_[channels_group_[i]];
    gout<<"\n";
  }
  gout<<"\n\n";

  if ( num_channels_ > 3*maxshow )
  {
    for ( int i = num_channels_-maxshow; i < num_channels_; i++ )
    {
      gout<<std::setw(width)<<i+1;
      gout<<std::setw(width)<<channel_id_[i];
      gout<<std::setw(width)<<inc_id_[i];
      gout<<std::setw(2*width)<<channel_name_[i];
      gout<<std::setw(width)<<byteoffset_[channel_ext_[i]];
      gout<<std::setw(width)<<length_[channel_ext_[i]];
      gout<<std::setw(width)<<type_[channel_ext_[i]];
      gout<<std::setw(width)<<units_[i];
      gout<<std::setw(width)<<minmax_[i].first;
      gout<<std::setw(width)<<minmax_[i].second;
      gout<<std::setw(width)<<channels_group_[i];
      gout<<std::setw(width)<<group_id_[channels_group_[i]];
      gout<<std::setw(width)<<group_name_[channels_group_[i]];
      gout<<std::setw(width)<<num_channels_group_[channels_group_[i]];
      gout<<"\n";
    }
    gout<<"\n\n";
  }
}

void tdm_ripper::list_groups(std::ostream& gout, int width, int maxshow)
{
  // if present, show timestamps
  bool showts = ( group_timestamp_[0].first.compare("") != 0 );

  gout<<std::setw(width)<<"group";
  gout<<std::setw(width)<<"group id";
  gout<<std::setw(width)<<"group name";
  gout<<std::setw(width)<<"num channels";
  if ( showts ) gout<<std::setw(2*width)<<"start time";
  if ( showts ) gout<<std::setw(2*width)<<"stop time";
  gout<<"\n";
  gout<<std::setfill('-')<<(showts?std::setw(8*width+1):std::setw(4*width+1))<<"\n";
  gout<<std::setfill(' ');

  for ( int i = 0; i < num_groups_ && i < maxshow; i++ )
  {
    gout<<std::setw(width)<<i+1;
    gout<<std::setw(width)<<group_id_[i];
    gout<<std::setw(width)<<group_name_[i];
    gout<<std::setw(width)<<num_channels_group_[i];
    if ( showts ) gout<<std::setw(2*width)<<time_stamp(i,true);
    if ( showts ) gout<<std::setw(2*width)<<time_stamp(i,false);
    gout<<"\n";
  }
  gout<<"\n\n";

  if ( num_groups_ > 3*maxshow )
  {
    for ( int i = num_groups_-maxshow; i < num_groups_; i++ )
    {
      gout<<std::setw(width)<<i+1;
      gout<<std::setw(width)<<group_id_[i];
      gout<<std::setw(width)<<group_name_[i];
      gout<<std::setw(width)<<num_channels_group_[i];
      if ( showts ) gout<<std::setw(2*width)<<time_stamp(i,true);
      if ( showts ) gout<<std::setw(2*width)<<time_stamp(i,false);
      gout<<"\n";
    }
    gout<<"\n\n";
  }
}

void tdm_ripper::show_structure()
{
  int width = 25;

  std::cout<<"second level tree elements:\n";
  for ( pugi::xml_node child: xml_doc_.child("usi:tdm").children())
  {
    std::cout<<child.name()<<"\n";
  }
  std::cout<<"\n\n";

  pugi::xml_node subtreeincl = xml_doc_.child("usi:tdm").child("usi:include");

  // most important information in .tdm file
  // - byteOffset provides the starting position of particular channel
  // - length     is the number of e.g. double (=8byte) value in that channel
  std::cout<<"file properties:\n\n";
  for (pugi::xml_node anode: subtreeincl.children("file"))
  {
    for (pugi::xml_attribute attr: anode.attributes())
    {
      std::cout<<" "<<attr.name()<<" = "<<attr.value()<<"   ";
    }
    std::cout<<"\n\n";

    int iter = 0;
    for (pugi::xml_node child: anode.children())
    {
      if ( iter < 100 )
      {
        std::cout<<std::right;
        std::cout<<std::setw(width)<<iter;
        std::cout<<std::setw(width)<<child.name();
        std::cout<<std::setw(width)<<child.value();

        for (pugi::xml_attribute attr: child.attributes())
        {
          std::cout<<std::right<<attr.name()<<" = "<<std::setw(width)<<std::left<<attr.value()<<"   ";

          if ( std::string(attr.name()).compare("valueType") == 0 )
          {
            std::cout<<"number of bytes = "<<datatypes_[attr.value()]/CHAR_BIT;
          }
        }
        std::cout<<"\n";
      }

      iter++;
    }
  }
  std::cout<<"\n\n";
}

void tdm_ripper::list_datatypes()
{
  // show datatype size on machine
  int width = 30;

  std::cout<<std::setw(width)<<"size of short int: "<<sizeof(short int)<<"\n";
  std::cout<<std::setw(width)<<"size of int: "<<sizeof(int)<<"\n";
  std::cout<<std::setw(width)<<"size of long int: "<<sizeof(long int)<<"\n";
  std::cout<<std::setw(width)<<"size of unsigned short int: "<<sizeof(unsigned short int)<<"\n";
  std::cout<<std::setw(width)<<"size of unsigned int: "<<sizeof(unsigned int)<<"\n";
  std::cout<<std::setw(width)<<"size of unsigned long int: "<<sizeof(unsigned long int)<<"\n\n";

  std::cout<<std::setw(width)<<"size of float: "<<sizeof(float)<<"\n";
  std::cout<<std::setw(width)<<"size of double: "<<sizeof(double)<<"\n";
  std::cout<<std::setw(width)<<"size of long double: "<<sizeof(long double)<<"\n\n";
}

// convert array of chars to integer
int tdm_ripper::convert_int(std::vector<unsigned char> bych)
{
  assert( bych.size() == sizeof(int) );
  assert( endianness_ );

  int df = 0.0;

  uint8_t *dfcast = reinterpret_cast<uint8_t*>(&df);

  for ( int i = 0; i < (int)sizeof(int); i++ )
  {
    dfcast[i] = (int)bych[i];
  }

  return df;
}

// disassemble single integer into array of chars
std::vector<unsigned char> tdm_ripper::convert_int(int df)
{
  assert( endianness_ );

  std::vector<unsigned char> bych((int)sizeof(int));

  uint8_t *dfcast = reinterpret_cast<uint8_t*>(&df);

  for ( int i = 0; i < (int)sizeof(int); i++ )
  {
    bych[i] = (int)dfcast[i];
  }

  return bych;
}

// convert array of chars to floating point double
double tdm_ripper::convert_double(std::vector<unsigned char> bych)
{
  assert( bych.size() == sizeof(double) );
  assert( endianness_ );

  // check for IEEE754 floating point standard
  assert( std::numeric_limits<double>::is_iec559 );

  double df = 0.0;

  uint8_t *dfcast = reinterpret_cast<uint8_t*>(&df);

  for ( int i = 0; i < (int)sizeof(double); i++ )
  {
    dfcast[i] = (int)bych[i];
  }

  return df;
}

std::vector<double> tdm_ripper::convert_channel(int channelid)
{
  // obtain offset, length of channel and size of datatype
  int byteoffset = byteoffset_[channelid];
  int length = length_[channelid];
  int typesize = datatypes_[type_[channelid]]/CHAR_BIT;

  // declare resulting array
  std::vector<double> chann(length);

  for ( int i = 0; i < length; i++ )
  {
    std::vector<unsigned char> cseg(tdxbuf_.begin()+byteoffset+i*typesize,
                                    tdxbuf_.begin()+byteoffset+(i+1)*typesize);

    if ( type_[channelid].compare("eInt32Usi") == 0 )
    {
      chann[i] = convert_int(cseg);
    }
    else if ( type_[channelid].compare("eFloat64Usi") == 0 )
    {
      chann[i] = convert_double(cseg);
    }
    else
    {
      assert( false && "datatype not supported!" );
    }
  }

  return chann;
}

std::vector<double> tdm_ripper::get_channel(int channelid)
{
  assert( channelid >= 0 && channelid < num_channels_ && "please provide valid channel id" );

  std::vector<double> chann = convert_channel(channel_ext_[channelid]);

  // check if converted value is within expected range
  for ( int i = 0; i < (int)chann.size(); i++ )
  {
    assert( chann[i] >= minmax_[channelid].first  - 1.0e-6
         && chann[i] <= minmax_[channelid].second + 1.0e-6 );
  }

  return chann;
}

void tdm_ripper::print_channel(int channelid, const char* filename, int width)
{
  assert( channelid >= 0 && channelid < num_channels_ && "please provide valid channel id" );

  std::ofstream fout(filename);

  std::vector<double> channdat = get_channel(channelid);
  for ( auto el: channdat ) fout<<std::setw(width)<<el<<"\n";
  fout.close();
}
