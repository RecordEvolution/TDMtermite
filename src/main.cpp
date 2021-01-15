// ------------------------------------------------------------------------- //

#include "tdm_ripper.hpp"

// ------------------------------------------------------------------------- //

const std::string tagfull("tagfullstring");
const std::string version("versionstring");

void show_usage()
{
  std::cout<<"\n"
           <<"tdmripper ["<<tagfull<<"] (Copyright © 2021 Record Evolution GmbH)"
           <<"\n\n"
           <<"Decode TDM/TDX files and dump data as *.csv"
           <<"\n\n"
           <<"Usage:\n\n"
           <<" tdmripper [options] <tdm-file> <tdx-file>"
           <<"\n\n"
           <<"Options:"
           <<"\n\n"
           <<" -d, --outputdir  output directory (must already exist!) for .csv files (default: current working directory)\n"
           <<" -s, --csvsep     separator character used in .csv files (default is comma ',')\n"
           <<" -h, --help       show this help message \n"
           <<" -v, --version    display version\n"
           <<"\n";
}

// ------------------------------------------------------------------------- //

// define type of key-value map object
typedef std::map<std::string,std::string> optkeys;

optkeys parse_args(int argc, char* argv[], bool showargs = false)
{
  if ( showargs )
  {
    std::cout<<"number of CLI-arguments: "<<argc<<"\n";
    for ( int i = 0; i < argc; i++ ) std::cout<<i<<":   "<<argv[i]<<"\n";
  }

  // declare empty key-value object
  optkeys prsdkeys;

  // at least both tdm and tdx file must be provided
  if ( argc > 1 )
  {
    if ( std::string(argv[1]) == std::string("--help") )
    {
      show_usage();
    }
    else if ( std::string(argv[1]) == std::string("--version") )
    {
      std::cout<<"tdmripper "<<version<<"\n";
    }
    else
    {
      for ( int i = 1; i < argc; i++ )
      {

      }
    }
  //   {
  //     if ( std::string(argv[i]) == std::string("--image") )
  //     {
  //       if ( i+1 == argc || std::string(argv[i+1]).substr(0,2) == std::string("--") )
  //       {
  //         throw std::runtime_error("invalid or missing --image argument");
  //       }
  //       else
  //       {
  //         prsdkeys.insert(std::pair<std::string,std::string>("image",argv[i+1]));
  //       }
  //       i++;
  //     }
  //     else if ( std::string(argv[i]) == std::string("--drive") )
  //     {
  //       if ( i+1 == argc || std::string(argv[i+1]).substr(0,2) == std::string("--") )
  //       {
  //         throw std::runtime_error("invalid or missing --drive argument");
  //       }
  //       else
  //       {
  //         prsdkeys.insert(std::pair<std::string,std::string>("drive",argv[i+1]));
  //       }
  //       i++;
  //     }
  //     else if ( std::string(argv[i]) == std::string("--config") )
  //     {
  //       if ( i+1 == argc || std::string(argv[i+1]).substr(0,2) == std::string("--") )
  //       {
  //         throw std::runtime_error("invalid or missing --config argument");
  //       }
  //       else
  //       {
  //         prsdkeys.insert(std::pair<std::string,std::string>("config",argv[i+1]));
  //       }
  //       i++;
  //     }
  //     else if ( std::string(argv[i]) == std::string("--writeblocksize") )
  //     {
  //       if ( i+1 == argc || std::string(argv[i+1]).substr(0,2) == std::string("--") )
  //       {
  //         throw std::runtime_error("invalid or missing --writeblocksize argument");
  //       }
  //       else
  //       {
  //         prsdkeys.insert(std::pair<std::string,std::string>("writeblock",argv[i+1]));
  //       }
  //       i++;
  //     }
  //     else if ( std::string(argv[i]) == std::string("--readblocksize") )
  //     {
  //       if ( i+1 == argc || std::string(argv[i+1]).substr(0,2) == std::string("--") )
  //       {
  //         throw std::runtime_error("invalid or missing --readblocksize argument");
  //       }
  //       else
  //       {
  //         prsdkeys.insert(std::pair<std::string,std::string>("readblock",argv[i+1]));
  //       }
  //       i++;
  //     }
  //     else if ( std::string(argv[i]) == std::string("--novalidate") )
  //     {
  //       prsdkeys.insert(std::pair<std::string,std::string>("novalidate","no"));
  //     }
  //     else if ( std::string(argv[i]) == std::string("--listdrives") )
  //     {
  //       prsdkeys.insert(std::pair<std::string,std::string>("listdrives","yes"));
  //     }
  //     else if ( std::string(argv[i]) == std::string("--help") )
  //     {
  //       show_usage();
  //     }
  //     else
  //     {
  //       throw std::runtime_error(std::string("invalid argument ") + std::string(argv[i]));
  //     }
  //   }
  }
  else
  {
    // show_usage();
    throw std::runtime_error(".tdm/.tdx files are missing! see $ ./tdmripper --help for usage");
  }

  return prsdkeys;
}

// ------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
  // parse CLI arguments
  optkeys cfgopts = parse_args(argc,argv);

  // // path of filename provided ?
  // assert( argc > 1 && "please provide a filename and path" );
  //
  // std::cout<<"number of CLI-arguments: "<<argc<<"\n";
  // for ( int i = 0; i < argc; i++ ) std::cout<<std::setw(5)<<i<<":   "<<argv[i]<<"\n";

  // // declare and initialize tdm_ripper
  // assert( argc == 3 );
  // tdm_ripper ripper(argv[1],argv[2],false); //,"samples/SineData.tdx",false);
  //
  // // ripper.list_datatypes();
  // // ripper.show_structure();
  //
  // ripper.print_hash_local("data/hash_table_xml_local.dat");
  // ripper.print_hash_values("data/hash_table_xml_value.dat");
  // ripper.print_hash_double("data/hash_table_xml_double.dat");
  // ripper.print_extid("data/channel_ext_id.dat");
  //
  // ripper.list_groups();
  // std::ofstream gout("data/list_of_groups.dat");
  // ripper.list_groups(gout);
  // gout.close();
  //
  // ripper.list_channels();
  // std::ofstream fout("data/list_of_channels.dat");
  // ripper.list_channels(fout);
  // fout.close();
  //
  // // long int nsa = 6.3636349745e10; // expected result: 22.07.2016, 19:49:05
  // // long int nsb = 6.3636350456e10;
  // // std::string ts = std::to_string(nsa);
  // // std::cout<<ripper.unix_timestamp(ts)<<"\n";
  //
  // std::cout<<"number of channels "<<ripper.num_channels()<<"\n";
  // std::cout<<"number of groups "<<ripper.num_groups()<<"\n\n";
  //
  // // obtain some specific meta information tags
  // std::cout<<"\n"<<ripper.get_meta("SMP_Name")<<"\n";
  // std::cout<<ripper.get_meta("SMP_Aufbau_Nr")<<"\n";
  // std::cout<<ripper.get_meta("SMP_Type")<<"\n";
  // std::cout<<ripper.get_meta("Location")<<"\n\n";
  //
  // // print all meta information
  // ripper.print_meta("data/meta_info.csv");
  //
  // // for ( int i = 0; i < ripper.num_groups(); i++ )
  // // {
  // //   std::cout<<std::setw(10)<<i+1<<std::setw(10)<<ripper.no_channels(i+1)<<"\n";
  // //   for ( int j = 0; j < ripper.no_channels(i+1); j++ )
  // //   {
  // //     std::cout<<std::setw(10)<<i+1<<std::setw(10)<<j+1<<std::setw(30)<<ripper.channel_name(i+1,j+1)<<"\n";
  // //   }
  // // }
  //
  // // for ( int i = 3; i < 10; i++ )
  // for ( int i = 0; i < ripper.num_channels(); i++ )
  // // for ( int i = 11880; i < ripper.num_channels(); i++ )
  // {
  //   ripper.print_channel(i,("data/channel_"+std::to_string(i+1)+"_"
  //                                       +ripper.channel_name(i)+".dat").c_str());
  // }

  return 0;
}

// ------------------------------------------------------------------------- //
