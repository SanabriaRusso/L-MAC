#include <bitset>

std::vector< int > get_bits( unsigned long x ) {

    std::string chars( std::bitset< sizeof(long) * CHAR_BIT >( x ).to_string( char(0), char(1) ) );
    return std::vector< int >( chars.begin(), chars.end() );
}