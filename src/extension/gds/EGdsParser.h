#ifndef ECAD_EXT_GDS_PARSER_EGDSPARSER_H
#define ECAD_EXT_GDS_PARSER_EGDSPARSER_H
#include "EGdsObjects.h"
#include "EGdsRecords.h"
#include <istream>

#define ECAD_EXT_GDS_NO_SPACES_TO_INDENT 2
namespace ecad {
namespace ext {
namespace gds {

class EGdsReader;
class ECAD_API EGdsParser
{
public:
    EGdsParser(EGdsReader & reader);
    virtual ~EGdsParser();

    bool operator() (const std::string & filename);
    bool operator() (std::istream & fp);

protected:
    const char * Parse(std::istream & fp, int & noRead, size_t n);
    void FindRecordType(int numeric, EGdsRecords::EnumType & recordType, int & expectedDataType);
    void FindDataType(int numeric, EGdsData::EnumType & dataType);

protected:
    EGdsReader & m_reader;
    char * m_buffer;
    char * m_bptr;
    size_t m_bcap;//buffer capacity
    size_t m_blen;//current buffer size, from m_bptr to m_buffer + m_bcap
};

}//namespace gds   
}//namespace ext
}//namespace ecad

#ifdef ECAD_HEADER_ONLY
#include "EGdsParser.cpp"
#endif

#endif//ECAD_EXT_GDS_PARSER_EGDSPARSER_H