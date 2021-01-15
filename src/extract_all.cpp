

#include "lib/tdm_ripper.hpp"

int main(int argc, char* argv[])
{
  // list of arguments: a) tdm file b) tdx file c) optional argument: output directory, default is ./
  assert( argc == 3 || argc == 4 );

  // declare and initialize tdm_ripper with two arguments specifying the files
  tdm_ripper ripper(argv[1],argv[2],true);

  // use given or default directory for output
  std::string outdir;
  outdir = ( argc == 4 ) ? argv[3] : "./" ;

  ripper.print_hash_local((outdir+"hash_table_xml_local.dat").c_str());
  ripper.print_hash_values((outdir+"hash_table_xml_value.dat").c_str());
  ripper.print_hash_double((outdir+"hash_table_xml_double.dat").c_str());
  ripper.print_extid((outdir+"channel_ext_id.dat").c_str());

  std::ofstream gout((outdir+"list_of_groups.dat").c_str());
  ripper.list_groups(gout);
  gout.close();

  std::ofstream fout((outdir+"list_of_channels.dat").c_str());
  ripper.list_channels(fout);
  fout.close();

  // print all meta information
  ripper.print_meta((outdir+"meta_info.csv").c_str());

  for ( int i = 0; i < ripper.num_channels(); i++ )
  {
    ripper.print_channel(i,(outdir+"channel_"+std::to_string(i+1)+"_"
                                  +ripper.channel_name(i)+".dat").c_str());
  }

  return 0;
}
