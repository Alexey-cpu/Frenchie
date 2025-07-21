#pragma once

#include <memory>
#include <string>
#include <filesystem>

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
    {
        namespace Helpers
        {
            inline std::string get_file_extention(const std::filesystem::path& _Path)
            {
                if(std::filesystem::is_directory(_Path)) 
                    return std::string();

                auto        filePath  = std::filesystem::path(_Path).make_preferred().stem();
                std::string extention = std::filesystem::path(_Path).make_preferred().extension().string();

                while (!filePath.extension().empty())
                {
                    extention = filePath.extension().string().append(extention);
                    filePath  = std::filesystem::path(filePath.stem().wstring());
                }

                return extention;
            }

            // from string conversion
            template<typename __type>
            inline __type from_string(const std::string& _Input)
            {
                return __type()
            }

            template<> inline float from_string<float>(const std::string& _Input)
            {
                try
                {
                    return std::stof(_Input);
                }
                catch(...)
                {
                    return 0.0f;
                }
            }

            template <typename __type>
            std::string to_string(const __type& _Input)
            {
                return std::string();
            }

            template<> inline double from_string<double>(const std::string& _Input)
            {
                try
                {
                    return std::stod(_Input);
                }
                catch(...)
                {
                    return 0.0;
                }
            }

            template<> inline long double from_string<long double>(const std::string& _Input)
            {
                try
                {
                    return std::stold(_Input);
                }
                catch(...)
                {
                    return 0.0;
                }
            }

            template<> inline short from_string<short>(const std::string& _Input)
            {
                try
                {
                    return std::stoi(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline int from_string<int>(const std::string& _Input)
            {
                try
                {
                    return std::stoi(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline long from_string<long>(const std::string& _Input)
            {
                try
                {
                    return std::stol(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline long long from_string<long long>(const std::string& _Input)
            {
                try
                {
                    return std::stoll(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline unsigned short from_string<unsigned short>(const std::string& _Input)
            {
                try
                {
                    return (unsigned short)std::stoul(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline unsigned int from_string<unsigned int>(const std::string& _Input)
            {
                try
                {
                    return std::stoul(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline unsigned long from_string<unsigned long>(const std::string& _Input)
            {
                try
                {
                    return std::stoul(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline unsigned long long from_string<unsigned long long>(const std::string& _Input)
            {
                try
                {
                    return std::stoull(_Input);
                }
                catch(...)
                {
                    return 0;
                }
            }

            template<> inline bool from_string<bool>(const std::string& _Input)
            {
                try
                {
                    return _Input == "true" || std::stoi( _Input ) ? true : false;
                }
                catch(...)
                {
                    return false;
                }
            }

            template<> inline std::string from_string< std::string >(const std::string& _Input)
            {
                return _Input;
            }

            template<> inline char from_string<char>(const std::string& _Input)
            {
                return _Input.empty() ? ' ' : _Input[0];
            }

            // to string conversion
            template<typename __type>
            inline std::string to_string(const __type& _Input);

            template<> inline std::string to_string<bool>(const bool& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<float>(const float& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<double>(const double& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<long double>(const long double& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<short>(const short& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<int>(const int& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<long>(const long& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<long long>(const long long& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<unsigned short>(const unsigned short& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<unsigned int>(const unsigned int& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<unsigned long>(const unsigned long& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<unsigned long long>(const unsigned long long& _Input)
            {
                return std::to_string(_Input);
            }

            template<> inline std::string to_string<std::string>(const std::string& _Input)
            {
                return _Input;
            }

            inline std::chrono::high_resolution_clock::time_point tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            template< typename __type = std::chrono::nanoseconds >
            double elapsed( std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now )
            {
                return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
            }
        }    
    }
}