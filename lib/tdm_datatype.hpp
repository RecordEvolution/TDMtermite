// -------------------------------------------------------------------------- //

#ifndef TDMDATATYPE
#define TDMDATATYPE

// https://zone.ni.com/reference/de-XX/help/370858P-0113/tdmdatamodel/tdmdatamodel/tdm_header_tdx_data/

// !!!! define mapping of locally supported datatypes to tdm datatypes
//  !!!! this is where the magic happens !!!
typedef short int eInt16Usi;
typedef int eInt32Usi;
typedef unsigned char eUInt8Usi;
typedef unsigned short int eUInt16Usi;
typedef unsigned int eUInt32Usi;
typedef float eFloat32Usi;
typedef double eFloat64Usi;
typedef char eStringUsi;

class tdmdatatype
{

protected:
  eInt16Usi sint16_;    // 0
  eInt32Usi sint32_;    // 1
  eUInt8Usi uint8_;     // 2
  eUInt16Usi uint16_;   // 3
  eUInt32Usi uint32_;   // 4
  eFloat32Usi float32_; // 5
  eFloat64Usi float64_; // 6
  eStringUsi string_;   // 7
  short int dtidx_;     // \in \{0,...,7\}

public:
  tdmdatatype(): sint16_(0), sint32_(0),
                 uint8_(0), uint16_(0), uint32_(0),
                 float32_(0.0), float64_(0.0), string_(0),
                 dtidx_(0) { };
  // every supported datatype gets its own constructor
  tdmdatatype(eInt16Usi num): sint16_(num), dtidx_(0) {};
  tdmdatatype(eInt32Usi num): sint32_(num), dtidx_(1) {};
  tdmdatatype(eUInt8Usi num): uint8_(num), dtidx_(2) {};
  tdmdatatype(eUInt16Usi num): uint16_(num), dtidx_(3) {};
  tdmdatatype(eUInt32Usi num): uint32_(num), dtidx_(4) {};
  tdmdatatype(eFloat32Usi num): float32_(num), dtidx_(5) {};
  tdmdatatype(eFloat64Usi num): float64_(num), dtidx_(6) {};
  tdmdatatype(eStringUsi num): string_(num), dtidx_(7) {};

  // identify type
  short int& dtype() { return dtidx_; }

  // copy constructor
  tdmdatatype(const tdmdatatype &num)
  {
    this->sint16_ = num.sint16_;
    this->sint32_ = num.sint32_;
    this->uint8_ = num.uint8_;
    this->uint16_ = num.uint16_;
    this->uint32_ = num.uint32_;
    this->float32_ = num.float32_;
    this->float64_ = num.float64_;
    this->string_ = num.string_;
    this->dtidx_ = num.dtidx_;
  }

  // overall assignment operator
  tdmdatatype& operator=(const tdmdatatype &num)
  {
    if ( this != &num )
    {
      this->sint16_ = num.sint16_;
      this->sint32_ = num.sint32_;
      this->uint8_ = num.uint8_;
      this->uint16_ = num.uint16_;
      this->uint32_ = num.uint32_;
      this->float32_ = num.float32_;
      this->float64_ = num.float64_;
      this->string_ = num.string_;
      this->dtidx_ = num.dtidx_;
    }

    return *this;
  }

  // implement assignment operator for individual datatypes
  tdmdatatype& operator=(const eInt16Usi &num)
  {
    this->sint16_ = num;
    this->dtidx_ = 0;
    return *this;
  }
  tdmdatatype& operator=(const eInt32Usi &num)
  {
    this->sint32_ = num;
    this->dtidx_ = 1;
    return *this;
  }
  tdmdatatype& operator=(const eUInt8Usi &num)
  {
    this->uint8_ = num;
    this->dtidx_ = 2;
    return *this;
  }
  tdmdatatype& operator=(const eUInt16Usi &num)
  {
    this->uint16_ = num;
    this->dtidx_ = 3;
    return *this;
  }
  tdmdatatype& operator=(const eUInt32Usi &num)
  {
    this->uint32_ = num;
    this->dtidx_ = 4;
    return *this;
  }
  tdmdatatype& operator=(const eFloat32Usi &num)
  {
    this->float32_ = num;
    this->dtidx_ = 5;
    return *this;
  }
  tdmdatatype& operator=(const eFloat64Usi &num)
  {
    this->float64_ = num;
    this->dtidx_ = 6;
    return *this;
  }
  tdmdatatype& operator=(const eStringUsi &num)
  {
    this->string_ = num;
    this->dtidx_ = 7;
    return *this;
  }

  // obtain number as double
  double as_double()
  {
    double num = 0.0;
    if ( dtidx_ == 0 ) num = (double)sint16_;
    else if ( dtidx_ == 1 ) num = (double)sint32_;
    else if ( dtidx_ == 2 ) num = (double)uint8_;
    else if ( dtidx_ == 3 ) num = (double)uint16_;
    else if ( dtidx_ == 4 ) num = (double)uint32_;
    else if ( dtidx_ == 5 ) num = (double)float32_;
    else if ( dtidx_ == 6 ) num = (double)float64_;
    else if ( dtidx_ == 7 ) num = (double)(int)string_;
    return num;
  }

  // define custom stream operator to print the correct type
  friend std::ostream& operator<<(std::ostream& out, const tdmdatatype& num)
  {
    if ( num.dtidx_ == 0 ) out<<num.sint16_;
    else if ( num.dtidx_ == 1 ) out<<num.sint32_;
    else if ( num.dtidx_ == 2 ) out<<num.uint8_;
    else if ( num.dtidx_ == 3 ) out<<num.uint16_;
    else if ( num.dtidx_ == 4 ) out<<num.uint32_;
    else if ( num.dtidx_ == 5 ) out<<num.float32_;
    else if ( num.dtidx_ == 6 ) out<<num.float64_;
    else if ( num.dtidx_ == 7 ) out<<num.string_;
    return out;
  }

};

// // base class for all tdm datatypes
// class tdmdatatype
// {
// protected:
//   short int sint16_;
//   int sint32_;
//   unsigned char uint8_;
//   unsigned short int uint16_;
//   unsigned int uint32_;
//   float float32_;
//   double float64_;
// public:
//   tdmdatatype(): sint16_(0), sint32_(0),
//                  uint8_(0), uint16_(0), uint32_(0),
//                  float32_(0.0), float64_(0.0) {};
//   virtual ~tdmdatatype() = default;
//   friend std::ostream& operator<<(std::ostream& out, const tdmdatatype& num)
//   {
//     return num.print(out);
//   }
//   virtual std::ostream& print(std::ostream& out) const
//   {
//     out<<"tdmdatatype";
//     return out;
//   }
// };

// class eInt16Usi: public tdmdatatype
// {
// public:
//   eInt16Usi() { }
//   eInt16Usi(short int num) { sint16_ = num; }
//   // eInt16Usi& operator=(const eInt16Usi &num)
//   // {
//   //   // self-assignment check
//   //   if ( this != &num)
//   //   {
//   //     this->sint16_ = num.sint16_;
//   //   }
// 	// 	return *this;
//   // }
//   friend std::ostream& operator<<(std::ostream& out, const eInt16Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<sint16_;
//     return out;
//   }
// };
//
// class eInt32Usi: public tdmdatatype
// {
// public:
//   eInt32Usi() { }
//   eInt32Usi(int num) { sint32_ = num; }
//   friend std::ostream& operator<<(std::ostream& out, const eInt32Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<sint32_;
//     return out;
//   }
// };
//
// class eUInt8Usi: public tdmdatatype
// {
// public:
//   eUInt8Usi() { }
//   eUInt8Usi(int num) { uint8_ = num; }
//   friend std::ostream& operator<<(std::ostream& out, const eUInt8Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<uint8_;
//     return out;
//   }
// };
//
// class eUInt16Usi: public tdmdatatype
// {
// public:
//   eUInt16Usi() { }
//   eUInt16Usi(int num) { uint16_ = num; }
//   friend std::ostream& operator<<(std::ostream& out, const eUInt16Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<uint16_;
//     return out;
//   }
// };
//
// class eUInt32Usi: public tdmdatatype
// {
// public:
//   eUInt32Usi() { }
//   eUInt32Usi(int num) { uint32_ = num; }
//   friend std::ostream& operator<<(std::ostream& out, const eUInt32Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<uint32_;
//     return out;
//   }
// };
//
// class eFloat32Usi: public tdmdatatype
// {
// public:
//   eFloat32Usi() { }
//   eFloat32Usi(int num) { float32_ = num; }
//   friend std::ostream& operator<<(std::ostream& out, const eFloat32Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<float32_;
//     return out;
//   }
// };
//
// class eFloat64Usi: public tdmdatatype
// {
// public:
//   eFloat64Usi() { }
//   eFloat64Usi(int num) { float64_ = num; }
//   friend std::ostream& operator<<(std::ostream& out, const eFloat64Usi& num)
//   {
//     return num.print(out);
//   }
//   std::ostream& print(std::ostream& out) const override
//   {
//     out<<float64_;
//     return out;
//   }
// };

struct tdm_datatype {

  std::string name_;
  std::string channel_datatype_;
  int numeric_;
  std::string value_sequence_;
  unsigned int size_;
  std::string description_;

  const std::string get_info() { return get_info(defformat); }
  const std::string get_info(format& formatter)
  {
    formatter.set_columns( { std::make_pair("name",name_),
                             std::make_pair("channel_datatype",channel_datatype_),
                             std::make_pair("name",name_),
                             std::make_pair("value_sequence",value_sequence_),
                             std::make_pair("size",std::to_string(size_)),
                             std::make_pair("description",description_) } );

    return formatter.get_info();
  }
};

const std::vector<tdm_datatype> tdm_datatypes = {

  {"eInt16Usi","DT_SHORT",2,"short_sequence",2,"signed 16 bit integer"},
  {"eInt32Usi","DT_LONG",6,"long_sequence",4,"signed 32 bit integer"},

  {"eUInt8Usi","DT_BYTE",5,"byte_sequence",1,"unsigned 8 bit integer"},
  {"eUInt16Usi","DT_SHORT",2,"short_sequence",2,"unsigned 16 bit integer"},
  {"eUInt32Usi","DT_LONG",6,"long_sequence",4,"unsigned 32 bit integer"},

  {"eFloat32Usi","DT_FLOAT",3,"float_sequence",4,"32 bit float"},
  {"eFloat64Usi","DT_DOUBLE",7,"double_sequence",8,"64 bit double"},

  {"eStringUsi","DT_STRING",1,"string_sequence",1,"text"}

};

#endif

// -------------------------------------------------------------------------- //
