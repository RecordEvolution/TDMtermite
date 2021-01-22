// ------------------------------------------------------------------------- //

#include "tdm_reaper.hpp"

#include <filesystem>
#include <regex>
#include <thread>
#include <chrono>

// ------------------------------------------------------------------------- //

const std::string gittag("TAGSTRING");
const std::string githash("HASHSTRING");

void show_usage()
{
  std::cout<<"\n"
           <<"tdmreaper ["<<gittag<<"-g"<<githash<<"] (github.com/RecordEvolution/tdm_ripper.git)"
           <<"\n\n"
           <<"Decode TDM/TDX files and dump data as *.csv"
           <<"\n\n"
           <<"Usage:\n\n"
           <<" tdmripper [options] <tdm-file> <tdx-file>"
           <<"\n\n"
           <<"Options:"
           <<"\n\n"
           <<" -d, --output        (existing!) output directory (default: current working directory)\n"
           <<" -f, --filenames     filenaming rule using %C (channel index), %c (channel name),\n"
           <<"                                           %G (group index), %g (group name) \n"
           <<"                                           (default: --filenames=channel_%G_%C.csv )\n"
           <<" -s, --csvsep        separator character used in .csv files (default is comma ',')\n"
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
      else if ( std::string(argv[i]) == std::string("--filenames")
             || std::string(argv[i]) == std::string("-f") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("filenames",argv[i+1]));
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
    std::string files = cfgopts.count("filenames") == 1 ? cfgopts.at("filenames")
                                                      : std::string("channel_%G_%C.csv");
    // bool listgroups = cfgopts.count("listgroups") == 1 ? true : false;
    // bool listchannels = cfgopts.count("listchannels") == 1 ? true : false;

    // declare and initialize tdm_ripper instance
    tdm_reaper jack;
    try {
      jack.submit_files(cfgopts.at("tdm"),cfgopts.at("tdx"),false);
    } catch (const std::exception& e) {
      throw std::runtime_error( std::string("failed to load/parse tdm/tdx files: ")
                              + e.what() );
    }

    // check available datatypes on machine
    // jack.check_local_datatypes();

    // show some meta data of the dataset
    std::cout<<"\n"<<jack.get_root().get_info()<<"\n\n";

    // get complete channel overview
    format chformatter(14,false,false,' ');
    std::cout<<jack.get_channel_overview(chformatter)<<"\n";

    // get complete submatrix/localcolumns overview
    format formatter(18,false,false,' ');
    std::cout<<jack.get_submatrix_overview(formatter)<<"\n";
    std::cout<<jack.get_localcolumn_overview(formatter)<<"\n";
    std::cout<<jack.get_block_overview(formatter)<<"\n";

    std::string chid("usi14");
    std::vector<tdmdatatype> chdata = jack.get_channel(chid);

    std::cout<<"channel size: "<<chdata.size()<<"\n";
    for ( tdmdatatype el: chdata )
    {
      std::cout<<el<<":"<<el.dtype()<<"\n";
    }

    // std::vector<std::string> chgrids = jack.get_channelgroup_ids();
    // for ( auto el: chgrids ) std::cout<<el<<",";
    // std::cout<<"\n";
    // std::vector<std::string> chids = jack.get_channel_ids();
    // for ( auto el: chids ) std::cout<<el<<",";
    // std::cout<<"\n\n";


    // for ( auto el: chids )
    // {
    //   std::string chfile = std::string("channel_") +el +std::string(".csv");
    //   jack.print_channel(el,chfile.c_str());
    // }

    // print list of groups or channels to stdout
    // if ( listgroups ) jack.list_groups();
    // if ( listchannels ) jack.list_channels();

    // // write data to filesystem
    // if ( !listgroups && !listchannels )
    // {
    //   // declare filesystem path
    //   std::filesystem::path pd = output;
    //
    //   // check for given directory
    //   if ( std::filesystem::is_directory(pd) )
    //   {
    //     // print (group,channel) data
    //     for ( int g = 0; g < jack.num_groups(); g++ )
    //     {
    //       // get and sanitize group name
    //       std::string grpnm = jack.group_name(g);
    //       std::regex regg("([^A-Za-z0-9])");
    //       std::string grpname = std::regex_replace(grpnm,regg,"");
    //
    //       for ( int c = 0; c < jack.no_channels(g); c++ )
    //       {
    //         // get overall channel index/id
    //         int chidx = jack.obtain_channel_id(g,c);
    //
    //         // get and sanitize channel name
    //         std::string chnm = jack.channel_name(g,c);
    //         std::regex regc("([^A-Za-z0-9])");
    //         std::string chname = std::regex_replace(chnm,regc,"");
    //
    //         // construct file name according to filenaming rule
    //         std::string filenm = files;
    //         filenm = std::regex_replace(files,std::regex("\\%C"),std::to_string(c+1));
    //         filenm = std::regex_replace(filenm,std::regex("\\%c"),chname);
    //         filenm = std::regex_replace(filenm,std::regex("\\%G"),std::to_string(g+1));
    //         filenm = std::regex_replace(filenm,std::regex("\\%g"),grpname);
    //
    //         // concat paths
    //         std::filesystem::path outfile = pd / filenm;
    //
    //         // write channel to filesystem
    //         jack.print_channel(chidx,outfile.c_str());
    //       }
    //     }
    //
    //     // print meta data
    //     jack.print_meta((pd / "meta-data.log").c_str());
    //   }
    //   else
    //   {
    //     std::cerr<<std::string("directory '") + output
    //              + std::string("' does not exist") + std::string("\n");
    //   }
    // }
  }

  //std::this_thread::sleep_for(std::chrono::milliseconds(10000));

  return 0;
}

// ------------------------------------------------------------------------- //
