#pragma once

#include <memory>
#include <filesystem>

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
    {
        template<typename Type, typename ... __arguments>
        std::shared_ptr<Type> create_shared_pointer(__arguments... _Parameters)
        {
            return std::make_shared<Type>(_Parameters ...);
        }

        template<typename Type, typename ... __arguments>
        std::unique_ptr<Type> create_unique_pointer(__arguments... _Parameters)
        {
            return std::make_unique<Type>(_Parameters ...);
        }

        template<typename Type, typename ... __arguments>
        Type* create_raw_pointer(__arguments... _Parameters)
        {
            return new Type(_Parameters ...);
        }

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