
#include "tdm_ripper.hpp"

tdm_ripper::tdm_ripper():
  tdmfile_(""), tdxfile_(""), filesprovided_(false),
  num_channels_(0), num_groups_(0),
  channel_id_(0), group_id_(0), channel_name_(0), group_name_(0),
  num_channels_group_(0), channels_group_(0),
  byteoffset_(0), length_(0), type_(0)
{

}

tdm_ripper::tdm_ripper(std::string tdmfile, std::string tdxfile):
  tdmfile_(tdmfile), tdxfile_(tdxfile), filesprovided_(true),
  num_channels_(0), num_groups_(0),
  channel_id_(0), group_id_(0), channel_name_(0), group_name_(0),
  num_channels_group_(0), channels_group_(0),
  byteoffset_(0), length_(0), type_(0)
{
  setup();
}

void tdm_ripper::setfiles(std::string tdmfile, std::string tdxfile)
{
  tdmfile_ = tdmfile;
  tdxfile_ = tdxfile;
  filesprovided_ = true;
  setup();
}

void tdm_ripper::setup()
{
  assert( filesprovided_ );

  // TODO directly provide the C datatype to be used!!
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
  std::cout<<"\nloading and parsing file: "<<xml_result_.description()<<"\n";
  std::cout<<"\nencoding: "<<(pugi::xml_encoding)xml_result_.encoding<<"\n\n";

  pugi::xml_node subtreeincl = xml_doc_.child("usi:tdm").child("usi:include");

  std::cout<<"file modified: "<<xml_doc_.child("usi:tdm").child("usi:data")
                        .child("tdm_root").child_value("datetime")<<"\n\n";

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

  std::cout<<"required .tdx-file is '"<<tdxfile_<<"'\n\n";

  parse_structure();

  // open .tdx and stream all binary data into vector
  std::ifstream fin(tdxfile_.c_str(),std::ifstream::binary);
  std::vector<unsigned char> tdxbuf((std::istreambuf_iterator<char>(fin)),
                                    (std::istreambuf_iterator<char>()));
  tdxbuf_ = tdxbuf;

  std::cout<<"number of bytes in binary file: "<<tdxbuf_.size()<<"\n\n";
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
  }

  // get node with channels and groups
  pugi::xml_node subtreedata = xml_doc_.child("usi:tdm").child("usi:data");

  // extract basic information about available groups
  for (pugi::xml_node anode: subtreedata.children())
  {
    if ( std::string(anode.name()).compare("tdm_channelgroup") == 0 )
    {
      num_groups_++;
      group_id_.push_back(anode.attribute("id").value());
      group_name_.push_back(anode.child_value("name"));
      int numchann = count_occ_string(anode.child_value("channels"),"id");
      num_channels_group_.push_back(numchann);
    }
  }

  // extract basic information about available channels
  for (pugi::xml_node anode: subtreedata.children())
  {
    if ( std::string(anode.name()).compare("tdm_channel") == 0 )
    {
      channel_id_.push_back(anode.attribute("id").value());
      channel_name_.push_back(anode.child_value("name"));
      std::string groupid(anode.child_value("group"));
      for ( int g = 0; g < num_groups_; g++ )
      {
        if ( groupid.find(group_id_[g]) != std::string::npos ) channels_group_.push_back(g+1);
      }
    }
  }

  // check consistency of number of channelgroups
  int numgroups = count_occ_string(subtreedata.child("tdm_root").child_value("channelgroups"),"id");
  assert( numgroups == num_groups_ );

  // check consistency of number of channels
  assert( num_channels_ == (int)channel_id_.size()
       && num_channels_ == (int)channel_name_.size()
       && num_channels_ == (int)channels_group_.size() );
}

void tdm_ripper::show_channels(int width, int maxshow)
{
  std::cout<<std::setw(width)<<"index";
  std::cout<<std::setw(width)<<"id";
  std::cout<<std::setw(2*width)<<"name";
  std::cout<<std::setw(width)<<"offset";
  std::cout<<std::setw(width)<<"length";
  std::cout<<std::setw(width)<<"datatype";
  std::cout<<std::setw(width)<<"group";
  std::cout<<std::setw(width)<<"group id";
  std::cout<<std::setw(width)<<"group name";
  std::cout<<std::setw(width)<<"num channels";
  std::cout<<"\n";
  std::cout<<std::setfill('-')<<std::setw(11*width+1)<<"\n";
  std::cout<<std::setfill(' ');

  for ( int i = 0; i < num_channels_ && i < maxshow; i++ )
  {
    std::cout<<std::setw(width)<<i+1;
    std::cout<<std::setw(width)<<channel_id_[i];
    std::cout<<std::setw(2*width)<<channel_name_[i];
    std::cout<<std::setw(width)<<byteoffset_[i];
    std::cout<<std::setw(width)<<length_[i];
    std::cout<<std::setw(width)<<type_[i];
    std::cout<<std::setw(width)<<channels_group_[i];
    std::cout<<std::setw(width)<<group_id_[channels_group_[i]-1];
    std::cout<<std::setw(width)<<group_name_[channels_group_[i]-1];
    std::cout<<std::setw(width)<<num_channels_group_[channels_group_[i]-1];
    std::cout<<"\n";
  }
  std::cout<<"\n\n";

  if ( num_channels_ > maxshow )
  {
    for ( int i = num_channels_-maxshow; i < num_channels_; i++ )
    {
      std::cout<<std::setw(width)<<i+1;
      std::cout<<std::setw(width)<<channel_id_[i];
      std::cout<<std::setw(2*width)<<channel_name_[i];
      std::cout<<std::setw(width)<<byteoffset_[i];
      std::cout<<std::setw(width)<<length_[i];
      std::cout<<std::setw(width)<<type_[i];
      std::cout<<std::setw(width)<<channels_group_[i];
      std::cout<<std::setw(width)<<group_id_[channels_group_[i]-1];
      std::cout<<std::setw(width)<<group_name_[channels_group_[i]-1];
      std::cout<<std::setw(width)<<num_channels_group_[channels_group_[i]-1];
      std::cout<<"\n";
    }
    std::cout<<"\n\n";
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

// convert array of chars to floating point double
double tdm_ripper::convert_float(std::vector<unsigned char> bych)
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

std::vector<double> tdm_ripper::convert_channel(int byteoffset, int length, int typesize)
{
  std::vector<double> chann(length);

  for ( int i = 0; i < length; i++ )
  {
    std::vector<unsigned char> cseg(tdxbuf_.begin()+byteoffset+i*typesize,
                                    tdxbuf_.begin()+byteoffset+(i+1)*typesize);
    chann[i] = convert_float(cseg);
  }

  return chann;
}

std::vector<double> tdm_ripper::get_channel(int channelid)
{
  assert( channelid > 0 && channelid <= num_channels_ && "please provide valid channel id" );

  return convert_channel(byteoffset_[channelid-1],length_[channelid-1],
                              datatypes_[type_[channelid-1]]/CHAR_BIT);
}

void tdm_ripper::print_channel(int channelid, const char* filename, int width)
{
  std::ofstream fout(filename);

  std::vector<double> channdat = get_channel(channelid);
  for ( auto el: channdat ) fout<<std::setw(width)<<el<<"\n";
  fout.close();
}
