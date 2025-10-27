#include <FrenchieCoreStringConvert.hpp>

using namespace Frenchie::Core;

#include <chrono>

template<> float Frenchie::Core::String::from_string<float>(const std::string& _Input)
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

template<> double Frenchie::Core::String::from_string<double>(const std::string& _Input)
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

template<> long double Frenchie::Core::String::from_string<long double>(const std::string& _Input)
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

template<> short Frenchie::Core::String::from_string<short>(const std::string& _Input)
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

template<> int Frenchie::Core::String::from_string<int>(const std::string& _Input)
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

template<> long Frenchie::Core::String::from_string<long>(const std::string& _Input)
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

template<> long long Frenchie::Core::String::from_string<long long>(const std::string& _Input)
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

template<> unsigned short Frenchie::Core::String::from_string<unsigned short>(const std::string& _Input)
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

template<> unsigned int Frenchie::Core::String::from_string<unsigned int>(const std::string& _Input)
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

template<> unsigned long Frenchie::Core::String::from_string<unsigned long>(const std::string& _Input)
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

template<> unsigned long long Frenchie::Core::String::from_string<unsigned long long>(const std::string& _Input)
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

template<> bool Frenchie::Core::String::from_string<bool>(const std::string& _Input)
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

template<> std::string Frenchie::Core::String::from_string< std::string >(const std::string& _Input)
{
    return _Input;
}

template<> char Frenchie::Core::String::from_string<char>(const std::string& _Input)
{
    return _Input.empty() ? ' ' : _Input[0];
}

template<> std::string Frenchie::Core::String::to_string<bool>(const bool& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<float>(const float& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<double>(const double& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<long double>(const long double& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<short>(const short& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<int>(const int& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<long>(const long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<long long>(const long long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned short>(const unsigned short& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned int>(const unsigned int& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned long>(const unsigned long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned long long>(const unsigned long long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<nullptr_t>(const nullptr_t& _Input)
{
    return "0";
}

template<> std::string Frenchie::Core::String::to_string<std::string>(const std::string& _Input)
{
    return _Input;
}

template<> std::string Frenchie::Core::String::to_string<std::chrono::system_clock::time_point>(const std::chrono::system_clock::time_point& _Input)
{
    std::time_t now = std::chrono::system_clock::to_time_t(_Input);
    std::string time = std::string(std::asctime(std::localtime(&now)));
    time.pop_back();
    return time;
}