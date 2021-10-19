
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

int main(int argc, char* argv[])
{
  for ( int i = 0; i < argc; i++ )
  {
    std::cout<<argv[i]<<"\n";
  }
  if ( argc < 3 )
  {
    std::cout<<"missing file argument\n";
    return 1;
  }

  std::ifstream fin(argv[1],std::ifstream::binary);

  std::vector<unsigned char> tdxbuf((std::istreambuf_iterator<char>(fin)),
                                    (std::istreambuf_iterator<char>()));

  fin.close();

  std::cout<<"length of buffer: "<<tdxbuf.size()<<"\n";
                                      

  unsigned long int dtsize = 8;

  if ( tdxbuf.size()%dtsize != 0 )
  {
    std::cout<<"mismatch between datatype size and length of buffer\n";
    return 1;
  }
  

  unsigned long int nums = tdxbuf.size()/dtsize;
  
  std::cout<<"number of entities: "<<nums<<"\n";

  std::vector<unsigned char> tdxbufrev(tdxbuf.size());

  for ( unsigned long int i = 0; i < nums; i++ )
  {
    for ( unsigned long int j = 0; j < dtsize; j++ )
    {
      tdxbufrev[i*dtsize+j] = tdxbuf[(i+1)*dtsize-(j+1)];
    }
  }

  std::ofstream fou(argv[2],std::ifstream::binary);

  for ( unsigned char ch: tdxbufrev)
  {
    fou<<ch;
  }

  fou.close();

  return 0;
}

