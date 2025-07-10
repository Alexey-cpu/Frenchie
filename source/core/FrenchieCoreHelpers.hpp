#pragma once

#include <memory>
#include <filesystem>

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
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
    }
}