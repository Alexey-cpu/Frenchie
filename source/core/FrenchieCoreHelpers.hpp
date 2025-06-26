#pragma once

#include <memory>
#include <filesystem>

namespace Frenchie
{
    namespace Core
    {
        template<typename __type, typename ... __arguments>
        std::shared_ptr<__type> create_shared_pointer(__arguments... _Parameters)
        {
            return std::make_shared<__type>(_Parameters ...);
        }

        template<typename __type, typename ... __arguments>
        std::unique_ptr<__type> create_unique_pointer(__arguments... _Parameters)
        {
            return std::make_unique<__type>(_Parameters ...);
        }

        template<typename __type, typename ... __arguments>
        __type* create_raw_pointer(__arguments... _Parameters)
        {
            return new __type(_Parameters ...);
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