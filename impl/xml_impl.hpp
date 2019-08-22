#ifndef VTU11_XML_IMPL_HPP
#define VTU11_XML_IMPL_HPP

namespace vtu11
{

inline ScopedXmlTag::ScopedXmlTag( std::ostream& output,
                                   const std::string& name,
                                   const StringStringMap& attributes ) :
   closeTag( [ &output, name ]( ){ output << "</" << name << ">\n"; } )
{
  output << "<" << name;
  
  for( const auto& attribute : attributes )
  {
    output << " " << attribute.first << "=\"" << attribute.second << "\"";
  }
  
  output << ">\n";
}   


inline ScopedXmlTag::~ScopedXmlTag( )
{
  closeTag( );
}


} // namespace vtu11

#endif // VTU11_XML_HPP
