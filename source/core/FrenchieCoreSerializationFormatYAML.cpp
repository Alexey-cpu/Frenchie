#include <FrenchieCoreSerializationFormatYAML.hpp>

// ryml can be used as a single header, or as a simple library:
#if defined(RYML_SINGLE_HEADER) // using the single header directly in the executable
    #define RYML_SINGLE_HDR_DEFINE_NOW
    #include <ryml_all.hpp>
#elif defined(RYML_SINGLE_HEADER_LIB) // using the single header from a library
    #include <ryml_all.hpp>
#else
    #include <ryml.hpp>
    // <ryml_std.hpp> is needed if interop with std containers is
    // desired; ryml itself does not use any STL container.
    // For this sample, we will be using std interop, so...
    #include <ryml_std.hpp> // optional header, provided for std:: interop
    #include <c4/format.hpp> // needed for the examples below
#endif

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class YAML final
            {
            public:

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path)) 
                        return false;

                    return true;
                }

                template<bool Compact>
                static bool write(Document* _Document, const std::filesystem::path& _Path)
                {
                    //return true;

                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path.parent_path())) 
                        return false;

                    return true;
                }
            };
        }
    }
}

using namespace Frenchie::Core;
using namespace Frenchie::Core::Helpers;
using namespace Frenchie::Core::Serialization;

bool YAMLReader::read(Document* _Document, const std::filesystem::path& _Path)
{
    return YAML::read(_Document, _Path);
}

bool YAMLCompactWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return YAML::write<true>(_Document, _Path);
}

bool YAMLBeautifulWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return YAML::write<false>(_Document, _Path);
}