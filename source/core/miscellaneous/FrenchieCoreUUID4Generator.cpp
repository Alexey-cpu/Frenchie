#include <FrenchieCoreUUID4Generator.hpp>

using namespace Frenchie::Core;

#include <sstream>

namespace Frenchie
{
    namespace Core
    {
        namespace Helpers
        {
            std::vector<std::string> split(const std::string& _Input, const std::string& _Delimeter)
            {
                if( _Input.empty() )
                    return std::vector<std::string>();

                int start  = 0;
                int end    = 0;
                int size   = (int)_Delimeter.size();
                std::vector<std::string> output;

                while(end >= 0)
                {
                    end = (int)_Input.find(_Delimeter, start);
                    output.push_back( _Input.substr(start, end-start) );
                    start = end + size;
                }

                return output;
            }

            bool contains_symbol(const std::string& _Input, char _Symbol)
            {
                for(size_t i = 0; i < _Input.size(); i++)
                {
                    if( _Input[i] == _Symbol )
                        return true;
                }

                return false;
            }

            template<typename __type>
            void set_bit(__type& _M, const uint_fast8_t& _N)
            {
                if(_N < sizeof (_M) * 8UL)
                    _M |= 1UL << _N;
            }

            template<typename __type>
            void reset_bit(__type& _M, const uint_fast8_t& _N)
            {
                if(_N < sizeof(_M) * 8UL)
                    _M &= ~(1UL << _N);
            }

            template<typename __type>
            uint_fast32_t get_bit(const __type& _M, const uint_fast8_t& _N)
            {
                return _N < sizeof (_M) * 8UL ? (uint_fast32_t)(( _M >> _N) & 1) : 0;
            }

            template<typename T1, typename T2>
            T2 retrieve_bits(const T1& _Input, const uint_fast8_t& _S, const uint_fast8_t& _E)
            {
                // check
                if(_E > sizeof(T1) * 8)
                    return 0;

                // main code
                T2 output = 0;

                for(uint_fast32_t i = _S, j = 0 ; i < _E ; i++, j++)
                {
                    T1 bit = get_bit<T1>(_Input, i);

                    if(bit > 0)
                        set_bit(output, j);
                }

                return output;
            }

            template< typename __type >
            std::string to_hex_string(const __type& _M, const uint_fast8_t& _N = 0)
            {
                // initialize stream
                std::stringstream stream;
                stream << std::hex;

                // generate hex string
                uint_fast32_t N = _N < 4 ? sizeof(_M) * 8 : (uint_fast8_t)_N;

                for(uint_fast8_t i = 0 ; i < N ; i += 4)
                    stream << retrieve_bits<__type, uint_fast64_t>(_M, i, i + 4);

                return stream.str();
            }

            template<std::size_t N>
            void read_bits_from_uint(std::bitset<N>& _Target, const uint_fast64_t& _Source, const uint_fast8_t& _Start, const uint_fast8_t& _End)
            {
                // check
                if(N < _Start || N < _End)
                    return;

                // main code
                for(uint_fast8_t i = _Start, j = 0 ; i < _End ; i++, j++)
                    _Target[i] = get_bit(_Source, j);
            }

            template<std::size_t N1, std::size_t N2>
            std::bitset<N2> extract_bits(const std::bitset<N1>& _Source, const uint_fast8_t& _Start, const uint_fast8_t& _End)
            {
                // generate output
                std::bitset<N2> output;

                // check
                if(N1 < _Start || N1 < _End)
                    return output;

                // fill output
                for(uint_fast64_t i = _Start, j = 0 ; i < _End ; i++, j++)
                    output[j] = _Source[i];

                return output;
            }

            template< std::size_t N >
            bool big_unsigned_numbers_equal(std::bitset<N> _A, std::bitset<N> _B)
            {
                return !(_A ^ _B).any();
            }

            template< std::size_t N >
            bool big_unsigned_number_is_greater(std::bitset<N> _A, std::bitset<N> _B)
            {
                std::bitset<N> reference;
                reference[N-1] = 1;

                if(big_unsigned_numbers_equal(_A, _B))
                    return false;

                while (!((_A ^ _B) & reference).any())
                {
                    _A <<= 1;
                    _B <<= 1;
                }

                return (_A & reference).any();
            }

            template< std::size_t N >
            bool big_unsigned_number_is_less(std::bitset<N> _A, std::bitset<N> _B)
            {
                std::bitset<N> reference;
                reference[N-1] = 1;

                if(big_unsigned_numbers_equal( _A, _B ))
                    return false;

                while (!((_A ^ _B) & reference).any())
                {
                    _A <<= 1;
                    _B <<= 1;
                }

                return !(_A & reference).any();
            }

            template<std::size_t N>
            std::string write_to_hex_string(const std::bitset<N>& _Input)
            {
                // initialize stream
                std::stringstream stream;
                stream << std::hex;

                // generate hex string
                for(uint_fast64_t i = 0 ; i < N ; i += 4)
                {
                    uint_fast64_t hexNumber = 0;

                    for(uint_fast8_t j = 0 ; j < 4 ; j++)
                    {
                        if(_Input[i + j])
                            set_bit<uint_fast64_t>(hexNumber, j);
                    }

                    stream << hexNumber;
                }

                return stream.str();
            }
        }
    }
}

// UUID4
UUID4::UUID4(const uint_fast32_t& time_low,
        const uint_fast16_t& time_mid,
        const uint_fast16_t& time_hi_and_version,
        const uint_fast8_t&  clock_seq_hi_and_reserved,
        const uint_fast8_t&  clock_seq_low,
        const uint_fast64_t& node)
{
    Helpers::read_bits_from_uint(m_Key, time_low, 0, 32);
    Helpers::read_bits_from_uint(m_Key, time_mid, 32, 48);
    Helpers::read_bits_from_uint(m_Key, time_hi_and_version, 48, 64);
    Helpers::read_bits_from_uint(m_Key, clock_seq_hi_and_reserved, 64, 72);
    Helpers::read_bits_from_uint(m_Key, clock_seq_low, 72, 80);
    Helpers::read_bits_from_uint(m_Key, node, 80, 128);
}

UUID4::UUID4(const std::string& _GUID)
{
    this->from_string(_GUID);
}

UUID4::~UUID4(){}

std::string UUID4::to_string(bool _Delimeted) const
{
    // read GUID attributes
    std::bitset<32> time_low                  = Helpers::extract_bits<128, 32>(m_Key, 0, 32);
    std::bitset<16> time_mid                  = Helpers::extract_bits<128, 16>(m_Key, 32, 48);
    std::bitset<16> time_hi_and_version       = Helpers::extract_bits<128, 16>(m_Key, 48, 64);
    std::bitset<8>  clock_seq_hi_and_reserved = Helpers::extract_bits<128, 8>(m_Key, 64, 72);
    std::bitset<8>  clock_seq_low             = Helpers::extract_bits<128, 8>(m_Key, 72, 80);
    std::bitset<48> node                      = Helpers::extract_bits<128, 48>(m_Key, 80, 128);

    // convert to string
    if(_Delimeted)
    {
        return Helpers::write_to_hex_string<32>(time_low) + "-" +
                Helpers::write_to_hex_string<16>(time_mid) + "-" +
                Helpers::write_to_hex_string<16>(time_hi_and_version) + "-" +
                Helpers::write_to_hex_string<8>(clock_seq_hi_and_reserved) +
                Helpers::write_to_hex_string<8>(clock_seq_low) + "-" +
                Helpers::write_to_hex_string<48>(node);
    }
    else
    {
        return Helpers::write_to_hex_string<32>(time_low) +
                Helpers::write_to_hex_string<16>(time_mid) +
                Helpers::write_to_hex_string<16>(time_hi_and_version) +
                Helpers::write_to_hex_string<8>(clock_seq_hi_and_reserved) +
                Helpers::write_to_hex_string<8>(clock_seq_low) +
                Helpers::write_to_hex_string<48>(node);
    }
}

UUID4::operator std::bitset<128>() const
{
    return m_Key;
}

bool UUID4::operator == (const UUID4& _UUID) const
{
    return Helpers::big_unsigned_numbers_equal(m_Key, _UUID.m_Key);
}

bool UUID4::operator != (const UUID4& _UUID) const
{
    return !Helpers::big_unsigned_numbers_equal(m_Key, _UUID.m_Key);
}

bool UUID4::operator < (const UUID4& _UUID) const
{
    return Helpers::big_unsigned_number_is_less(m_Key, _UUID.m_Key);
}

bool UUID4::operator > (const UUID4& _UUID) const
{
    return Helpers::big_unsigned_number_is_greater(m_Key, _UUID.m_Key);
}

uint_fast64_t UUID4::convert_from_hex(const char& _Symbol)
{
    if(_Symbol == '0')
        return 0;

    if(_Symbol == '1')
        return 1;

    if(_Symbol == '2')
        return 2;

    if(_Symbol == '3')
        return 3;

    if(_Symbol == '4')
        return 4;

    if(_Symbol == '5')
        return 5;

    if(_Symbol == '6')
        return 6;

    if(_Symbol == '7')
        return 7;

    if(_Symbol == '8')
        return 8;

    if(_Symbol == '9')
        return 9;

    if(_Symbol == 'a')
        return 10;

    if(_Symbol == 'b')
        return 11;

    if(_Symbol == 'c')
        return 12;

    if(_Symbol == 'd')
        return 13;

    if(_Symbol == 'e')
        return 14;

    if(_Symbol == 'f')
        return 15;

    return 16;
}

void UUID4::read_with_delimeter(const std::string& _GUID)
{
    // split input into sections
    std::vector<std::string> input = Frenchie::Core::Helpers::split(_GUID, "-");

    // check input size
    if(input.empty() || input.size() < 5)
    {
        // show error here !!!
        return;
    }

    // check input sections size
    if(
        input[0].size() < 8 ||
        input[1].size() < 4 ||
        input[2].size() < 4 ||
        input[3].size() < 4 ||
        input[4].size() < 12 )
    {
        // show error here
        return;
    }

    // main code
    uint_fast8_t l = 0;
    for(uint_fast8_t i = 0 ; i < input.size() ; i++)
    {
        for(uint_fast8_t j = 0 ; j < input[i].size() ; j++)
        {
            uint_fast64_t num = convert_from_hex(input[i][j]);

            // check
            if(num >= 16)
            {
                m_Key.reset();
                return;
            }

            for(uint_fast8_t k = 0 ; k < 4 ; k++)
            {
                m_Key[l++] = Helpers::get_bit(num , k);
            }
        }
    }
}

void UUID4::read_without_delimeter(const std::string& _GUID)
{
    // check input size
    if( _GUID.size() < 32 )
        return;

    // main code
    uint_fast8_t k = 0;
    for(uint_fast8_t i = 0 ; i < _GUID.size() ; i++)
    {
        uint_fast64_t num = convert_from_hex(_GUID[i]);

        // check parsed hex number
        if( num >= 16 )
        {
            m_Key.reset();
            return;
        }

        for(uint_fast8_t j = 0 ; j < 4 ; j++)
        {
            m_Key[k++] = Helpers::get_bit(num , j);
        }
    }
}

void UUID4::from_string(const std::string& _GUID)
{
    if(Frenchie::Core::Helpers::contains_symbol(_GUID, '-' ))
        read_with_delimeter(_GUID);
    else 
        read_without_delimeter(_GUID);
}

// UUID4Generator
UUID4Generator::UUID4Generator()
{
    // setup the seed
    m_PseudoRandomNumberGenerator.seed( (uint_fast64_t)this );

    // warmup pseudo random number generator
    for(uint_fast64_t i = 0; i < 1e2; i++)
        (void)m_PseudoRandomNumberGenerator();
}

UUID4Generator::~UUID4Generator(){}

UUID4 UUID4Generator::guid()
{
    // generate 128 random bits
    uint_fast64_t m = m_PseudoRandomNumberGenerator();
    uint_fast64_t n = m_PseudoRandomNumberGenerator();

    // generate UUID attributes
    uint_fast32_t time_low                  = Helpers::retrieve_bits<uint_fast64_t, uint_fast32_t>(m, 0, 32);
    uint_fast16_t time_mid                  = Helpers::retrieve_bits<uint_fast64_t, uint_fast16_t>(m, 32, 48);
    uint_fast16_t time_hi_and_version       = Helpers::retrieve_bits<uint_fast64_t, uint_fast16_t>(m, 48, 64);
    uint_fast8_t  clock_seq_hi_and_reserved = Helpers::retrieve_bits<uint_fast64_t, uint_fast8_t>(n, 0, 8);
    uint_fast8_t  clock_seq_low             = Helpers::retrieve_bits<uint_fast64_t, uint_fast8_t>(n, 8, 16);
    uint_fast64_t node                      = Helpers::retrieve_bits<uint_fast64_t, uint_fast64_t>(n, 16, 64);

    // 1 Set the two most significant bits (bits 6 and 7) of the
    // clock_seq_hi_and_reserved to zero and one, respectively.
    Helpers::reset_bit<uint_fast8_t>(clock_seq_hi_and_reserved, 0);
    Helpers::set_bit<uint_fast8_t>(clock_seq_hi_and_reserved, 1);

    // 2 Set the four most significant bits (bits 12 through 15) of the
    // time_hi_and_version field to the 4-bit version number from RFC4122
    Helpers::reset_bit<uint_fast16_t>(time_hi_and_version, 0);
    Helpers::reset_bit<uint_fast16_t>(time_hi_and_version, 1);
    Helpers::set_bit<uint_fast16_t>(time_hi_and_version, 2);
    Helpers::reset_bit<uint_fast16_t>(time_hi_and_version, 3);

    return UUID4(
        time_low,
        time_mid,
        time_hi_and_version,
        clock_seq_hi_and_reserved,
        clock_seq_low, node
        );
}