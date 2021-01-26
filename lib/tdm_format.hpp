// -------------------------------------------------------------------------- //

#ifndef TDMFORMAT
#define TDMFORMAT

// format output of info strings for terminal and file table
class format
{

private:

  unsigned int width_;
  bool tabular_;
  bool header_;
  char sep_;
  std::vector<std::pair<std::string,std::string>> columns_;

public:

  format(int width = 25, bool tabular = false, bool header = false, char sep = ' '):
    width_(width), tabular_(tabular), header_(header), sep_(sep)
  {

  }

  void set_width(int width)
  {
    width_ = width;
  }

  void set_tabular(bool tabular)
  {
    tabular_ = tabular;
  }

  void set_header(bool header)
  {
    header_ = header;
  }

  void set_sep(char sep)
  {
    sep_ = sep;
  }

  void set_columns(std::vector<std::pair<std::string,std::string>> columns)
  {
    columns_ = columns;
  }

  std::string get_info()
  {
    std::stringstream ss;

    for ( std::vector<std::pair<std::string,std::string>>::iterator it = columns_.begin();
                                                                    it != columns_.end(); ++it )
    {
      if ( tabular_ )
      {
        // header or body of table
        std::string entry = header_? it->first : it->second;

        // make broad aligned columns for human reader
        if ( sep_ == ' ' )
        {
          entry = entry.size() > width_-2 ? entry.substr(0,width_-2) : entry;
          // if ( it == columns_.begin() && !header_ ) ss<<"  ";
          ss<<std::setw(width_)<<std::left<<entry;
        }
        // make compressed csv like columns
        else
        {
          ss<<entry;
          if ( std::next(it,1) != columns_.end() ) ss<<sep_;
        }
      }
      else
      {
        ss<<std::setw(width_)<<std::left<<(it->first+std::string(":"))<<it->second<<"\n";
      }
    }

    return ss.str();
  }
};

// define default formatter
static format defformat(25,false,false,',');

// join a list of strings
static std::string join_strings(std::vector<std::string> &thestring, const char* sep = " ")
{
  std::string joined;
  for ( std::vector<std::string>::iterator it = thestring.begin();
                                           it != thestring.end(); ++it )
  {
    joined += std::next(it,1) != thestring.end() ? ( *it + std::string(sep) ) : *it;
  }

  return joined;
}

// join a list of numbers
template<class numtype>
static std::string join(std::vector<numtype> &thevec, const char* sep = " ")
{
  std::string joined;
  for ( unsigned int i = 0; i < thevec.size(); i++ )
  {
    joined += std::to_string(thevec.at(i));
    if ( i+1 < thevec.size() ) joined += std::string(sep);
  }

  return joined;
}

#endif

// -------------------------------------------------------------------------- //
