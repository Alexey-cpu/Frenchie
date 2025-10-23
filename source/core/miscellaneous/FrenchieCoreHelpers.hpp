#pragma once

#include <memory>
#include <string>
#include <bitset>
#include <codecvt>
#include <sstream>
#include <filesystem>
#include <functional>

#define STRINGIFY(_VALUE) #_VALUE

#if defined(unix) || defined(__unix) || defined(__unix__)
#define IS_LINUX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define IS_MACOS
#endif

#if defined(_WIN32) || defined(_WIN64)
#define IS_WINDOWS
#endif

#if defined(IS_LINUX) || defined(IS_MACOS)
#define IS_UNIX
#endif

namespace Frenchie
{
    namespace Core
    {
        namespace FileSystem
        {
            std::filesystem::path get_exe_absolute_path();

            std::filesystem::path get_exe_absolute_directory();

            std::string get_file_extention(const std::filesystem::path& _Path);

            FILE* open_file(const std::string& _Path, std::string _Mode);

            void create_directory(
                const std::filesystem::path& _Path, 
                const std::function<void()>& _OnSuccess, 
                const std::function<void(const std::exception&)>& _OnFail);
        }

#if defined(_STDINT_H) || defined(_STDINT_H_) || defined(_STDINT)

        namespace Bits
        {
            template<typename __type>
            void __set_bit__(__type& _M, uint_fast8_t _N)
            {
                if( _N < sizeof (_M) * 8UL )
                {
                    _M |= 1UL << _N;
                }
            }

            template<typename __type>
            void __reset_bit__(__type& _M, uint_fast8_t _N )
            {
                if( _N < sizeof( _M ) * 8UL )
                {
                    _M &= ~( 1UL << _N );
                }
            }

            template<typename __type>
            uint_fast32_t __get_bit__( const __type& _M, uint_fast8_t _N )
            {
                return _N < sizeof (_M) * 8UL ? (uint_fast32_t)(( _M >> _N ) & 1) : 0;
            }

            template<typename __type>
            __type __bit_reverse__(__type M, uint_fast8_t MSB)
            {
                __type result = 0;
                for ( uint_fast32_t i = 0 ; i < MSB ; ++i)
                {
                    if ( M & ( 1 << i ) ) // look for the set bits
                    {
                        result |= 1 << ( MSB - 1 - i ); // set the bits at the end of the resulting bit mask
                    }
                }

                return result;
            }

            template< typename __type >
            uint_fast32_t __get_msb__( const __type& _M )
            {
                uint_fast32_t MSB = 0;

                while ( ( 1 << MSB ) < _M )
                {
                    MSB++;
                }

                return MSB;
            }

            template<typename T1, typename T2>
            T2 __retrieve_bits__( const T1& _Input, uint_fast8_t _S, uint_fast8_t _E )
            {
                // check
                if( _E > sizeof(T1) * 8 )
                    return 0;

                // main code
                T2 output = 0;

                for( uint_fast32_t i = _S, j = 0 ; i < _E ; i++, j++ )
                {
                    T1 bit = Bits::__get_bit__<T1>( _Input, i );

                    if( bit > 0 )
                    {
                        Bits::__set_bit__( output, j );
                    }
                }

                return output;
            }

            inline uint_fast64_t __lfsr64__ ( uint_fast64_t _INPUT )
            {
                static uint_fast64_t S = _INPUT;
                S = ((((S >> 63) ^ (S >> 62) ^ (S >> 61) ^ (S >> 59) ^ (S >> 57) ^ S ) & (uint64_t)1 ) << 63 ) | (S >> 1);
                return S;
            }

        #if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD) || defined(_IOSFWD_)

            template< typename __type >
            std::string __to_hex_string__( const __type& _M, uint_fast8_t _N = 0 )
            {
                // initialize stream
                std::stringstream stream;
                stream << std::hex;

                // generate hex string
                uint_fast32_t N = _N < 4 ? sizeof(_M) * 8 : (uint_fast8_t)_N;

                for( uint_fast8_t i = 0 ; i < N ; i += 4 )
                {
                    stream << Bits::__retrieve_bits__< __type, uint_fast64_t >( _M, i, i + 4 );
                }

                return stream.str();
            }

        #endif
        }

#if defined(_GLIBCXX_BITSET) || defined(_LIBCPP_BITSET) || defined(_BITSET_)

        namespace BitSet
        {
            template<std::size_t N>
            void __read_bits_from_uint__(std::bitset<N>& _Target, const uint_fast64_t& _Source, uint_fast8_t _Start, uint_fast8_t _End)
            {
                // check
                if( N < _Start || N < _End )
                    return;

                // main code
                for( uint_fast8_t i = _Start, j = 0 ; i < _End ; i++, j++ )
                {
                    _Target[i] = Bits::__get_bit__( _Source, j );
                }
            }

            template< std::size_t N1, std::size_t N2 >
            std::bitset<N2> __extract_bits__( const std::bitset<N1>& _Source, uint_fast8_t _Start, uint_fast8_t _End )
            {
                // generate output
                std::bitset<N2> output;

                // check
                if( N1 < _Start || N1 < _End )
                    return output;

                // fill output
                for( uint_fast64_t i = _Start, j = 0 ; i < _End ; i++, j++ )
                {
                    output[j] = _Source[i];
                }

                return output;
            }

            template< std::size_t N >
            uint_fast8_t __get_msb__( std::bitset<N>& _Source )
            {
                uint_fast8_t MSB = 0;

                for( uint_fast8_t i = 0 ; i < N ; i++ )
                {
                    if( _Source[i] > 0 )
                    {
                        MSB = i;
                    }
                }

                return MSB;
            }

            template< std::size_t N >
            bool __big_unsigned_numbers_equal__( std::bitset<N> _A, std::bitset<N> _B )
            {
                return !( _A ^ _B ).any();
            }

            template< std::size_t N >
            bool __big_unsigned_number_is_greater__( std::bitset<N> _A, std::bitset<N> _B )
            {
                std::bitset<N> reference;
                reference[N-1] = 1;

                if( __big_unsigned_numbers_equal__( _A, _B ) )
                    return false;

                while ( !( (_A ^ _B) & reference).any() )
                {
                    _A <<= 1;
                    _B <<= 1;
                }

                return (_A & reference).any();
            }

            template< std::size_t N >
            bool __big_unsigned_number_is_less__( std::bitset<N> _A, std::bitset<N> _B )
            {
                std::bitset<N> reference;
                reference[N-1] = 1;

                if( __big_unsigned_numbers_equal__( _A, _B ) )
                    return false;

                while ( !( (_A ^ _B) & reference).any() )
                {
                    _A <<= 1;
                    _B <<= 1;
                }

                return !(_A & reference).any();
            }

            template< std::size_t N >
            struct BitSetsComparator
            {
                using is_transparent = std::bitset<N>;

            public:

                bool operator()( const std::bitset<N>& _A, const std::bitset<N>& _B ) const
                {
                    return __big_unsigned_number_is_less__( _A, _B );
                }
            };

        #if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD) || defined(_IOSFWD_)

            template< std::size_t N >
            std::string __write_to_hex_string__( const std::bitset<N>& _Set )
            {
                // initialize stream
                std::stringstream stream;
                stream << std::hex;

                // generate hex string
                for( uint_fast64_t i = 0 ; i < N ; i += 4 )
                {
                    uint_fast64_t hexNumber = 0;

                    for( uint_fast64_t j = 0 ; j < 4 ; j++ )
                    {
                        if( _Set[ i + j ] )
                        {
                            Bits::__set_bit__<uint_fast64_t>( hexNumber, j );
                        }
                    }

                    stream << hexNumber;
                }

                return stream.str();
            }

        #endif
        }

#endif

#endif

        namespace String
        {
            std::vector<std::string> split(const std::string& _Input, const std::string& _Delimeter = " ");
            std::string to_upper(std::string);
            std::string to_lower(std::string);

            std::string replace_symbol(std::string& _Input, const char& _From, const char& _To);
            std::string remove_symbol(std::string& _Input, const char& _What);
            std::string replace_substring(const std::string& _String, const std::string& _Substring, const std::string& _NewSubstring);
            bool contains_substring(const std::string& _String, const std::string& _Substring);
            bool contains_symbol(const std::string& _Input, char _Symbol = ' ');

            // from string conversion
            template<typename __type> 
            __type from_string(const std::string& _Input);

            // to string conversion
            template <typename __type>
            std::string to_string(const __type& _Input);

            std::string as_utf8(const std::wstring&);
            std::wstring as_wide(const std::string&);
        }

        namespace Time
        {
            inline std::chrono::high_resolution_clock::time_point tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            template< typename __type = std::chrono::nanoseconds >
            double elapsed(std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now)
            {
                return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
            }
        }  
    }
}