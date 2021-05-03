// ------------------------------------------------------------------------- //

#include "tdm_termite.hpp"

#include <filesystem>
#include <regex>
#include <thread>
#include <chrono>

// ------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
  std::string tdmfile(argv[1]);
  std::string tdxfile(argv[2]);

  std::cout<<tdmfile<<"\n"<<tdxfile<<"\n";

  pugi::xml_document xml_doc;
  pugi::xml_parse_result xml_result;

  // load XML document from stream
  std::ifstream fin(tdmfile.c_str());
  xml_result = xml_doc.load(fin);
  fin.close();

  std::cout<<"\nloading "<<tdmfile<<": "<<xml_result.description()<<"\n";
  std::cout<<"encoding: "<<(pugi::xml_encoding)xml_result.encoding<<"\n\n";

  std::this_thread::sleep_for(std::chrono::milliseconds(4000));

  return 0;
}
