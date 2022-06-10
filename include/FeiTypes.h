/*所用的类型*/
#ifndef FEITYPES_H
#define FEITYPES_H
#include <string>
#include <cstdint>
#include <memory>
namespace feipu {
    using std::string;
    /*from https://stackoverflow.com/a/26221725*/
    /*自建的string format*/
    template<typename ... Args>
    string string_format( const string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }
}
#endif