// ------------------------------------------------------------------------- //

#include "tdm_ripper.hpp"

#include <filesystem>
#include <regex>

// ------------------------------------------------------------------------- //

const std::string gittag("TAGSTRING");
const std::string githash("HASHSTRING");

void show_usage()
{
  std::cout<<"\n"
           <<"tdmripper ["<<gittag<<"-g"<<githash<<"] (Copyright © 2021 Record Evolution GmbH)"
           <<"\n\n"
           <<"Decode TDM/TDX files and dump data as *.csv"
           <<"\n\n"
           <<"Usage:\n\n"
           <<" tdmripper [options] <tdm-file> <tdx-file>"
           <<"\n\n"
           <<"Options:"
           <<"\n\n"
           <<" -d, --output        (existing!) output directory (default: current working directory)\n"
           <<" -s, --csvsep        separator character used in .csv files (default is comma ',')\n"
           <<" -p, --prefix        name prefix for .csv files (default is none)\n"
           <<" -g, --listgroups    list groups in data\n"
           <<" -c, --listchannels  list channels in data\n"
           <<" -h, --help          show this help message \n"
           <<" -v, --version       display version\n"
           <<"\n";
}

// ------------------------------------------------------------------------- //

// define type of key-value map object
typedef std::map<std::string,std::string> optkeys;

const std::string argmsg = std::string("both .tdm and .tdx file (and maybe any valid option) must be provided!");
const std::string arguse = std::string("see $ tdmripper --help for usage");

optkeys parse_args(int argc, char* argv[], bool showargs = false)
{
  if ( showargs )
  {
    std::cout<<"number of CLI-arguments: "<<argc<<"\n";
    for ( int i = 0; i < argc; i++ ) std::cout<<i<<":   "<<argv[i]<<"\n";
  }

  // declare empty key-value object
  optkeys prsdkeys;

  if ( argc == 2 )
  {
    if ( std::string(argv[1]) == std::string("--help")
      || std::string(argv[1]) == std::string("-h") )
    {
      show_usage();
    }
    else if ( std::string(argv[1]) == std::string("--version")
           || std::string(argv[1]) == std::string("-v") )
    {
      std::cout<<"tdmripper "<<gittag<<"\n";
    }
    else
    {
      std::cerr<<argmsg + std::string("\n") + arguse + std::string("\n");
      prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[1]));
      return prsdkeys;
    }
  }
  else if ( argc > 2 ) // && argc%2 == 1 )
  {
    // tdm file
    if ( std::string(argv[argc-2]).find(std::string(".tdm")) != std::string::npos )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("tdm",argv[argc-2]));
    }
    else
    {
      std::string tdmerr = std::string(argv[argc-2])
                         + std::string(" does not look like a .tdm file")
                         + std::string(", evtl. add file extension *.tdm")
                         + std::string("\n") + arguse;
      std::cerr<<tdmerr + std::string("\n");
      prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[argc-2]));
      return prsdkeys;
    }
    // tdx file
    if ( std::string(argv[argc-1]).find(std::string(".tdx")) != std::string::npos )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("tdx",argv[argc-1]));
    }
    else
    {
      std::string tdxerr = std::string(argv[argc-1])
                         + std::string(" does not look like a .tdx file")
                         + std::string(", evtl. add file extension *.tdx")
                         + std::string("\n") + arguse;
      std::cerr<<tdxerr + std::string("\n");
      prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[argc-1]));
      return prsdkeys;
    }

    // options (in any order)
    // for ( int i = 1; i < argc-2; i+=2 )
    for ( int i = 1; i < argc-2; i++ )
    {
      if ( std::string(argv[i]) == std::string("--output")
        || std::string(argv[i]) == std::string("-d") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("output",argv[i+1]));
        i += 1;
      }
      else if ( std::string(argv[i]) == std::string("--csvsep")
        || std::string(argv[i]) == std::string("-s") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("csvsep",argv[i+1]));
        i += 1;
      }
      else if ( std::string(argv[i]) == std::string("--prefix")
             || std::string(argv[i]) == std::string("-p") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("prefix",argv[i+1]));
        i += 1;
      }
      else if ( std::string(argv[i]) == std::string("--listgroups")
             || std::string(argv[i]) == std::string("-g") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listgroups",argv[i+1]));
      }
      else if ( std::string(argv[i]) == std::string("--listchannels")
             || std::string(argv[i]) == std::string("-c") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listchannels",argv[i+1]));
      }
      else
      {
        std::string argerr = std::string("unkown option '") + argv[i] + std::string("'");
        std::cerr<<argerr + std::string("\n") + arguse + std::string("\n");
        prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[i]));
        return prsdkeys;
      }
    }
  }
  else
  {
    std::cerr<<argmsg + std::string("\n") + arguse + std::string("\n");
  }

  return prsdkeys;
}

// ------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
  // parse CLI arguments
  optkeys cfgopts = parse_args(argc,argv);

  if ( cfgopts.count("tdm") == 1 && cfgopts.count("tdx") == 1
    && cfgopts.count("invalidoption") == 0 )
  {
    // set required option values
    std::string output = cfgopts.count("output") == 1 ? cfgopts.at("output")
                                                      : std::string("./");
    std::string csvsep = cfgopts.count("csvsep") == 1 ? cfgopts.at("csvsep")
                                                      : std::string(",");
    std::string prefix = cfgopts.count("prefix") == 1 ? cfgopts.at("prefix")
                                                      : std::string("");
    bool listgroups = cfgopts.count("listgroups") == 1 ? true : false;
    bool listchannels = cfgopts.count("listchannels") == 1 ? true : false;

    // declare and initialize tdm_ripper instance
    tdm_ripper jack(cfgopts.at("tdm"),cfgopts.at("tdx"));

    // print list of groups or channels to stdout
    if ( listgroups ) jack.list_groups();
    if ( listchannels ) jack.list_channels();

    // write all groups/channels to filesystem
    if ( !listgroups && !listchannels )
    {
      std::filesystem::path pd = output;
      if ( std::filesystem::is_directory(pd) )
      {
        for ( int i = 0; i < jack.num_channels(); i++ )
        {
          // sanitize channel name
          std::regex reg("([^A-Za-z0-9])");
          std::string chname = std::regex_replace(jack.channel_name(i),reg,"");

          // concat path and construct file name
          std::filesystem::path outfile = pd / ( std::string("channel_")
                                               + std::to_string(i+1) + std::string("_")
                                               + chname + std::string(".csv") );

          // write channel to filesystem
          jack.print_channel(i,outfile.c_str());
        }
      }
      else
      {
        std::cerr<<std::string("directory '") + output 
                 + std::string("' does not exist") + std::string("\n");
      }
    }
  }

  return 0;
}

// ------------------------------------------------------------------------- //
