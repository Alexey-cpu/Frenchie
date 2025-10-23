#pragma once

#include <cstdint>
#include <bitset>
#include <random>

namespace Frenchie
{
    namespace Core
    {
        // UUID4
        class UUID4 final
        {
        public:

            // nested types
            struct TransparentComparator
            {
                using is_transparent = UUID4;

            public:

                bool operator()(UUID4 _A, UUID4 _B) const
                {
                    return  _A < _B;
                }
            };

            UUID4(const uint_fast32_t& time_low,
                  const uint_fast16_t& time_mid,
                  const uint_fast16_t& time_hi_and_version,
                  const uint_fast8_t&  clock_seq_hi_and_reserved,
                  const uint_fast8_t&  clock_seq_low,
                  const uint_fast64_t& node);

            UUID4(const std::string& _GUID);
            ~UUID4();

            // public methods
            std::string to_string(bool _Delimeted = false) const;

            // operators overload
            operator std::bitset<128>() const;
            bool operator == (const UUID4&) const;
            bool operator != (const UUID4&) const;
            bool operator < (const UUID4&) const;
            bool operator > (const UUID4&) const;

        private:

            // info
            std::bitset<128> m_Key;

            // define hex format table
            static uint_fast64_t convert_from_hex(const char& _Symbol);

            // service methods
            void read_with_delimeter(const std::string&);
            void read_without_delimeter(const std::string&);
            void from_string(const std::string&);
        };

        // UUID4Generator
        class UUID4Generator final
        {
        public:
            UUID4Generator();
            ~UUID4Generator();

            // API
            UUID4 guid();

        protected:

            // info
            std::mt19937_64 m_PseudoRandomNumberGenerator;
        };
    }
};