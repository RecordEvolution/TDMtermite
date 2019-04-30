

#include "lib/tdm_ripper.hpp"

int main(int argc, char* argv[])
{
  // path of filename provided ?
  assert( argc > 1 && "please provide a filename and path" );

  // declare and initialize tdm_ripper
  tdm_ripper ripper(argv[1]);

  ripper.list_datatypes();

  // int sn = -76476;
  // std::vector<unsigned char> bych = ripper.convert_int(sn);
  // std::cout<<"length of vector "<<bych.size()<<"\n\n";
  // for ( auto c: bych) std::cout<<(int)c<<"  ";
  // std::cout<<"\n\n";
  //
  // std::cout<<ripper.convert_int(bych)<<"\n\n";

  ripper.list_channels();
  std::ofstream fout("data/list_of_channels.dat");
  ripper.list_channels(fout);
  fout.close();
  // ripper.show_structure();

  std::cout<<"number of channels "<<ripper.num_channels()<<"\n\n";
  std::cout<<"number of groups "<<ripper.num_groups()<<"\n\n";

  // std::vector<double> channA = ripper.get_channel(1);
  // for ( auto el: channA ) std::cout<<el<<"\n";
  // std::cout<<"\n\n";

  for ( int i = 0; i < ripper.num_channels(); i++ )
  {
    ripper.print_channel(i+1,("data/channel_"+std::to_string(i+1)+"_"
                                        +ripper.channel_name(i+1)+".dat").c_str());
  }

  return 0;
}
