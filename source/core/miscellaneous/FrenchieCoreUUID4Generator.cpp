#include <FrenchieCoreUUID4Generator.hpp>

#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core;

// UUID4
UUID4::UUID4(const uint_fast32_t& time_low,
        const uint_fast16_t& time_mid,
        const uint_fast16_t& time_hi_and_version,
        const uint_fast8_t&  clock_seq_hi_and_reserved,
        const uint_fast8_t&  clock_seq_low,
        const uint_fast64_t& node)
{
    BitSet::__read_bits_from_uint__(m_Key, time_low, 0, 32);
    BitSet::__read_bits_from_uint__(m_Key, time_mid, 32, 48);
    BitSet::__read_bits_from_uint__(m_Key, time_hi_and_version, 48, 64);
    BitSet::__read_bits_from_uint__(m_Key, clock_seq_hi_and_reserved, 64, 72);
    BitSet::__read_bits_from_uint__(m_Key, clock_seq_low, 72, 80);
    BitSet::__read_bits_from_uint__(m_Key, node, 80, 128);
}

UUID4::UUID4(const std::string& _GUID)
{
    this->from_string(_GUID);
}

UUID4::~UUID4(){}

std::string UUID4::to_string(bool _Delimeted) const
{
    // read GUID attributes
    std::bitset<32> time_low                  = BitSet::__extract_bits__<128, 32>(m_Key, 0, 32);
    std::bitset<16> time_mid                  = BitSet::__extract_bits__<128, 16>(m_Key, 32, 48);
    std::bitset<16> time_hi_and_version       = BitSet::__extract_bits__<128, 16>(m_Key, 48, 64);
    std::bitset<8>  clock_seq_hi_and_reserved = BitSet::__extract_bits__<128, 8>(m_Key, 64, 72);
    std::bitset<8>  clock_seq_low             = BitSet::__extract_bits__<128, 8>(m_Key, 72, 80);
    std::bitset<48> node                      = BitSet::__extract_bits__<128, 48>(m_Key, 80, 128);

    // convert to string
    if(_Delimeted)
    {
        return BitSet::__write_to_hex_string__<32>(time_low) + "-" +
                BitSet::__write_to_hex_string__<16>(time_mid) + "-" +
                BitSet::__write_to_hex_string__<16>(time_hi_and_version) + "-" +
                BitSet::__write_to_hex_string__<8>(clock_seq_hi_and_reserved) +
                BitSet::__write_to_hex_string__<8>(clock_seq_low) + "-" +
                BitSet::__write_to_hex_string__<48>(node);
    }
    else
    {
        return BitSet::__write_to_hex_string__<32>(time_low) +
                BitSet::__write_to_hex_string__<16>(time_mid) +
                BitSet::__write_to_hex_string__<16>(time_hi_and_version) +
                BitSet::__write_to_hex_string__<8>(clock_seq_hi_and_reserved) +
                BitSet::__write_to_hex_string__<8>(clock_seq_low) +
                BitSet::__write_to_hex_string__<48>(node);
    }
}

UUID4::operator std::bitset<128>() const
{
    return m_Key;
}

bool UUID4::operator == (const UUID4& _UUID) const
{
    return BitSet::__big_unsigned_numbers_equal__(m_Key, _UUID.m_Key);
}

bool UUID4::operator != (const UUID4& _UUID) const
{
    return !BitSet::__big_unsigned_numbers_equal__(m_Key, _UUID.m_Key);
}

bool UUID4::operator < (const UUID4& _UUID) const
{
    return BitSet::__big_unsigned_number_is_less__(m_Key, _UUID.m_Key);
}

bool UUID4::operator > (const UUID4& _UUID) const
{
    return BitSet::__big_unsigned_number_is_greater__(m_Key, _UUID.m_Key);
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
    std::vector<std::string> input = Frenchie::Core::String::split(_GUID, "-");

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
                m_Key[l++] = Bits::__get_bit__(num , k);
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
            m_Key[k++] = Bits::__get_bit__(num , j);
        }
    }
}

void UUID4::from_string(const std::string& _GUID)
{
    if(Frenchie::Core::String::contains_symbol(_GUID, '-' ))
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
    uint_fast32_t time_low                  = Bits::__retrieve_bits__<uint_fast64_t, uint_fast32_t>(m, 0, 32);
    uint_fast16_t time_mid                  = Bits::__retrieve_bits__<uint_fast64_t, uint_fast16_t>(m, 32, 48);
    uint_fast16_t time_hi_and_version       = Bits::__retrieve_bits__<uint_fast64_t, uint_fast16_t>(m, 48, 64);
    uint_fast8_t  clock_seq_hi_and_reserved = Bits::__retrieve_bits__<uint_fast64_t, uint_fast8_t>(n, 0, 8);
    uint_fast8_t  clock_seq_low             = Bits::__retrieve_bits__<uint_fast64_t, uint_fast8_t>(n, 8, 16);
    uint_fast64_t node                      = Bits::__retrieve_bits__<uint_fast64_t, uint_fast64_t>(n, 16, 64);

    // 1 Set the two most significant bits (bits 6 and 7) of the
    // clock_seq_hi_and_reserved to zero and one, respectively.
    Bits::__reset_bit__<uint_fast8_t>(clock_seq_hi_and_reserved, 0);
    Bits::__set_bit__<uint_fast8_t>(clock_seq_hi_and_reserved, 1);

    // 2 Set the four most significant bits (bits 12 through 15) of the
    // time_hi_and_version field to the 4-bit version number from RFC4122
    Bits::__reset_bit__<uint_fast16_t>(time_hi_and_version, 0);
    Bits::__reset_bit__<uint_fast16_t>(time_hi_and_version, 1);
    Bits::__set_bit__<uint_fast16_t>(time_hi_and_version, 2);
    Bits::__reset_bit__<uint_fast16_t>(time_hi_and_version, 3);

    return UUID4(
        time_low,
        time_mid,
        time_hi_and_version,
        clock_seq_hi_and_reserved,
        clock_seq_low, node
        );
}