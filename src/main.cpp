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
           <<" tdmreaper <tdm-file> <tdx-file> [options]"
           <<"\n\n"
           <<"Options:"
           <<"\n\n"
           <<" -m, --showmeta          show meta information about tdm dataset\n"
           <<" -r, --showroot          show root data of dataset\n"
           <<" -g, --listgroups        list channelgroups\n"
           <<" -c, --listchannels      list channels\n"
           <<" -b, --listblocks        list TDX blocks\n"
           <<" -u, --listsubmatrices   list submatrices\n"
           <<" -l, --listlocalcolumns  list localcolumns\n"
           // <<"options with arguments must be provided as '--option arg' or '-o arg'\n"
           <<" -d, --output            (existing) output directory (default: no default)\n"
           <<" -f, --filenames         If an output directory is provided, all channels\n"
           <<"                         of the dataset will be written to file(s).\n"
           <<"                         The filenaming rule using %C (channel index), %c (channel name),\n"
           <<"                                                   %G (group index), %g (group name) \n"
           <<"                         determines whether the channels are collected in files according\n"
           <<"                         to their channelgroups (as long as the filenaming rule includes\n"
           <<"                         %G or %g) or written in separate files. For instance, to obtain\n"
           <<"                         files with only one channel each use '--f channel_%C_%c.csv'.\n"
           <<"                         (default: '--filenames channelgroup_%G.csv' )\n"
           <<" -s, --csvsep            separator character used in .csv files (default is comma '--csvsep ,')\n"
           <<" -i, --includemeta       include channel(-group) meta-data into files\n"
           <<" -h, --help              show this help message \n"
           <<" -v, --version           display version\n"
           <<"\n";
}

// ------------------------------------------------------------------------- //

// define type of key-value map object
typedef std::map<std::string,std::string> optkeys;

const std::string argmsg = std::string("both .tdm and .tdx file must be provided!");
const std::string arguse = std::string("see $ tdmreaper --help for usage");

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
      std::cout<<"tdmreaper "<<gittag<<"-g"<<githash<<"\n";
    }
    else
    {
      std::cerr<<argmsg + std::string("\n") + arguse + std::string("\n");
      prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[1]));
      return prsdkeys;
    }
  }
  else if ( argc > 2 )
  {
    // tdm file
    if ( std::string(argv[1]).find(std::string(".tdm")) != std::string::npos )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("tdm",argv[1]));
    }
    else
    {
      std::string tdmerr = std::string(argv[1])
                         + std::string(" does not look like a .tdm file")
                         + std::string(", evtl. add file extension *.tdm")
                         + std::string("\n") + arguse;
      std::cerr<<tdmerr + std::string("\n");
      prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[1]));
      return prsdkeys;
    }
    // tdx file
    if ( std::string(argv[2]).find(std::string(".tdx")) != std::string::npos )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("tdx",argv[2]));
    }
    else
    {
      std::string tdxerr = std::string(argv[2])
                         + std::string(" does not look like a .tdx file")
                         + std::string(", evtl. add file extension *.tdx")
                         + std::string("\n") + arguse;
      std::cerr<<tdxerr + std::string("\n");
      prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[2]));
      return prsdkeys;
    }

    // options (in any order)
    for ( int i = 3; i < argc; i++ )
    {
      if ( std::string(argv[i]) == std::string("--showmeta")
        || std::string(argv[i]) == std::string("-m") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("showmeta","showmeta"));
      }
      else if ( std::string(argv[i]) == std::string("--showroot")
             || std::string(argv[i]) == std::string("-r") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("showroot","showroot"));
      }
      else if ( std::string(argv[i]) == std::string("--listgroups")
             || std::string(argv[i]) == std::string("-g") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listgroups","listgroups"));
      }
      else if ( std::string(argv[i]) == std::string("--listchannels")
             || std::string(argv[i]) == std::string("-c") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listchannels","listchannels"));
      }
      else if ( std::string(argv[i]) == std::string("--listblocks")
             || std::string(argv[i]) == std::string("-b") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listblocks","listblocks"));
      }
      else if ( std::string(argv[i]) == std::string("--listsubmatrices")
             || std::string(argv[i]) == std::string("-u") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listsubmatrices","listsubmatrices"));
      }
      else if ( std::string(argv[i]) == std::string("--listlocalcolumns")
             || std::string(argv[i]) == std::string("-l") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("listlocalcolumns","listlocalcolumns"));
      }
      //
      else if ( std::string(argv[i]) == std::string("--output")
             || std::string(argv[i]) == std::string("-d") )
      {
        if ( argc > i+1 )
        {
          prsdkeys.insert(std::pair<std::string,std::string>("output",argv[i+1]));
          i += 1;
        }
        else
        {
          std::cerr<<"missing argument for "<<argv[i]<<"\n";
          prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",""));
        }
      }
      else if ( std::string(argv[i]) == std::string("--filenames")
             || std::string(argv[i]) == std::string("-f") )
      {
        if ( argc > i+1 )
        {
          prsdkeys.insert(std::pair<std::string,std::string>("filenames",argv[i+1]));
          i += 1;
        }
        else
        {
          std::cerr<<"missing argument for "<<argv[i]<<"\n";
          prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",""));
        }
      }
      else if ( std::string(argv[i]) == std::string("--csvsep")
             || std::string(argv[i]) == std::string("-s") )
      {
        if ( argc > i+1 )
        {
          if ( std::string(argv[i+1]).size() == 1 )
          {
            prsdkeys.insert(std::pair<std::string,std::string>("csvsep",argv[i+1]));
            i += 1;
          }
          else
          {
            std::cerr<<"invalid argument of --csvsep\n";
            prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",argv[i+1]));
          }
        }
        else
        {
          std::cerr<<"missing argument for "<<argv[i]<<"\n";
          prsdkeys.insert(std::pair<std::string,std::string>("invalidoption",""));
        }
      }
      //
      else if ( std::string(argv[i]) == std::string("--includemeta")
             || std::string(argv[i]) == std::string("-i") )
      {
        prsdkeys.insert(std::pair<std::string,std::string>("includemeta","includemeta"));
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

  // show all CLI arguments
  if ( false )
  {
    for ( optkeys::iterator it=cfgopts.begin(); it!=cfgopts.end(); ++it )
    {
      std::cout<<std::setw(20)<<std::left<<it->first<<it->second<<"\n";
    }
  }

  // exit on any invalid option
  if ( cfgopts.count("tdm") == 1 && cfgopts.count("tdx") == 1
    && cfgopts.count("invalidoption") == 0 )
  {
    // get options
    bool showmeta = cfgopts.count("showmeta") == 1 ? true : false;
    bool showroot = cfgopts.count("showroot") == 1 ? true : false;
    bool listgroups = cfgopts.count("listgroups") == 1 ? true : false;
    bool listchannels = cfgopts.count("listchannels") == 1 ? true : false;
    bool listblocks = cfgopts.count("listblocks") == 1 ? true : false;
    bool listsubmatrices = cfgopts.count("listsubmatrices") == 1 ? true : false;
    bool listlocalcolumns = cfgopts.count("listlocalcolumns") == 1 ? true : false;
    bool includemeta = cfgopts.count("includemeta") == 1 ? true : false;

    // set required option values
    std::string output = cfgopts.count("output") == 1 ? cfgopts.at("output")
                                                      : std::string("");
    std::string files = cfgopts.count("filenames") == 1 ? cfgopts.at("filenames")
                                                      : std::string("channelgroup_%G.csv");
    std::string csvsep = cfgopts.count("csvsep") == 1 ? cfgopts.at("csvsep")
                                                      : std::string(",");

    // declare and initialize tdm_reaper instance
    tdm_reaper jack;
    try {
      jack.submit_files(cfgopts.at("tdm"),cfgopts.at("tdx"),false);
    } catch (const std::exception& e) {
      throw std::runtime_error( std::string("failed to load/parse tdm/tdx files: ")
                              + e.what() );
    }

    // show some meta data of the dataset
    if ( showmeta ) std::cout<<"\n"<<jack.get_meta().get_info()<<"\n";
    if ( showroot ) std::cout<<"\n"<<jack.get_root().get_info()<<"\n";

    // get complete channel(-group) overview
    format grpformatter(26,false,false,' ');
    if (listgroups) std::cout<<"\n"<<jack.get_overview<tdm_channelgroup>(grpformatter)<<"\n";
    format chformatter(14,false,false,' ');
    if (listchannels) std::cout<<"\n"<<jack.get_channel_overview(chformatter)<<"\n";

    // get complete submatrix/localcolumns overview
    format formatter(18,false,false,' ');
    if (listblocks) std::cout<<jack.get_overview<block>(formatter)<<"\n";
    if (listsubmatrices) std::cout<<jack.get_overview<submatrix>(formatter)<<"\n";
    if (listlocalcolumns) std::cout<<jack.get_overview<localcolumn>(formatter)<<"\n";

    // print data to files
    if ( !output.empty() )
    {
      // declare filesystem path
      std::filesystem::path pd = output;

      // check for given directory
      if ( std::filesystem::is_directory(pd) )
      {
        // obtain channelgroup ids
        std::vector<std::string> chgrids = jack.get_channelgroup_ids();

        // check for any existing group-id in filenames rule
        std::string slctgrp("");
        for ( auto id: chgrids ) if ( files.find(id) != std::string::npos ) slctgrp = id;

        // obtain list of channel ids
        std::vector<std::string> chids = jack.get_channel_ids();

        // check for any existing channel-id in filenames rule
        std::string slctchn("");
        for ( auto id: chids ) if ( files.find(id) != std::string::npos ) slctchn = id;

        // write channels in files grouped by channelgroups
        if ( files.find("%G") != std::string::npos || files.find("%g") != std::string::npos
          || !slctgrp.empty() )
        {
          // iterate through channelgroup ids
          for ( auto id: chgrids )
          {
            // write all channelgroups or single chosen one
            if ( slctgrp.empty() || slctgrp == id )
            {
              // get and sanitize group name
              tdm_channelgroup tdmgrp = jack.channelgroup(id);
              std::string grpnm = tdmgrp.name_;
              std::regex regg("([^A-Za-z0-9])");
              std::string grpname = std::regex_replace(grpnm,regg,"");

              // construct file name according to filenaming rule
              std::string filenm = files;
              filenm = std::regex_replace(filenm,std::regex("\\%G"),tdmgrp.id_);
              filenm = std::regex_replace(filenm,std::regex("\\%g"),grpname);

              // concat paths
              std::filesystem::path outfile = pd / filenm;

              // write entire channelgroup to file
              jack.print_group(id,outfile.c_str(),includemeta,csvsep.at(0));
            }
          }
        }
        // ...or write channels separately
        else
        {
          // iterate through channel ids
          for ( auto id: chids )
          {
            // write all channelgroups or single chosen one
            if ( slctchn.empty() || slctchn == id )
            {
              // get and sanitize channel name
              tdm_channel tdmchn = jack.channel(id);
              std::string chnnm = tdmchn.name_;
              std::regex regg("([^A-Za-z0-9])");
              std::string chnname = std::regex_replace(chnnm,regg,"");

              // construct file name according to filenaming rule
              std::string filenm = files;
              filenm = std::regex_replace(filenm,std::regex("\\%C"),tdmchn.id_);
              filenm = std::regex_replace(filenm,std::regex("\\%c"),chnname);

              // concat paths
              std::filesystem::path outfile = pd / filenm;

              // write entire channelgroup to file
              jack.print_channel(id,outfile.c_str(),includemeta);
            }
          }
        }
      }
      else
      {
        std::cerr<<std::string("directory '") + output
                 + std::string("' does not exist") + std::string("\n");
      }
    }
    else
    {
      if ( cfgopts.count("showmeta") == 0 && cfgopts.count("showroot") == 0
        && cfgopts.count("listgroups") == 0 && cfgopts.count("listchannels") == 0
        && cfgopts.count("listblocks") == 0 && cfgopts.count("listsubmatrices") == 0
        && cfgopts.count("listlocalcolumns") == 0 ) std::cerr<<"no output directory given\n";
    }
  }

  //std::this_thread::sleep_for(std::chrono::milliseconds(10000));

  return 0;
}

// ------------------------------------------------------------------------- //
