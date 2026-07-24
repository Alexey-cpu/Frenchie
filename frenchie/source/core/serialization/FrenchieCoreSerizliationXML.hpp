#pragma once

#include <FrenchieCoreSerizliation.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            namespace XML
            {
                // Parser
                class Parser
                {
                public:
                    static bool read_string(const ElementObj& _Object, const char* _Begin, const char* _End);
                };

                // Writer
                template<bool Pretty = true>
                class Writer
                {
                public:

                    // API
                    static bool save_file(const ElementObj& _Object, const std::string& _Path)
                    {
                        FileStreamer streamer(_Path);
                        return Writer::write(_Object, streamer);
                    }

                    static std::string write_string(const ElementObj& _Object)
                    {
                        StringStreamer streamer;
                        return Writer::write(_Object, streamer) ? streamer.get_stream_string() : std::string();
                    }
                
                protected:

                    // service methods
                    template<typename Streamer>
                    static bool write(const ElementObj& _Object, Streamer& _Streamer)
                    {
                        if(_Object.is_null() || !_Streamer.begin())
                        {
                            _Streamer.end();
                            return false;
                        }

                        _Object.traverse(
                            [&_Streamer](const ElementObj& _Node, const int& _Depth)
                            {
                                if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)
                                {
                                    // write prolog
                                    if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog)
                                    {
                                        _Streamer.write("<?", 2);
                                        _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                        
                                        if(Pretty)
                                            _Streamer.write("?>\n", 3);
                                        else
                                            _Streamer.write("?>", 2);
                                    }
                                    // write comment
                                    else if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment)
                                    {
                                        _Streamer.write("<!--", 4);
                                        _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                        
                                        if(Pretty)
                                            _Streamer.write("-->\n", 4);
                                        else
                                            _Streamer.write("-->", 3);
                                    }
                                    // write default element
                                    else
                                    {
                                        if(Pretty)
                                        {
                                            for (int i = 0; i < _Depth - 1; i++)
                                                _Streamer.write("\t", 1);
                                        }

                                        _Streamer.write("<", 1);
                                        _Streamer.write(_Node.get_name().data(), (int)_Node.get_name().size());

                                        // write attributres
                                        for(auto& child : _Node)
                                        {
                                            if(child.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)
                                            {
                                                _Streamer.write(" ", 1);
                                                _Streamer.write(child.get_name().data(), (int)child.get_name().size());
                                                _Streamer.write("=\"", 2);
                                                _Streamer.write(child.get_value().data(), (int)child.get_value().size());
                                                _Streamer.write("\"", 1);
                                            }
                                        }

                                        _Streamer.write(">", 1);
                                        
                                        // write value
                                        if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeCDATA)
                                        {
                                            _Streamer.write("<![CDATA[", 9);
                                            _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                            _Streamer.write("]]>", 3);
                                        }
                                        else
                                        {
                                            _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                        }
                                        
                                        if(Pretty)
                                            _Streamer.write("\n", 1);
                                    }
                                }
                            },
                            [&_Streamer](const ElementObj& _Node, const int& _Depth)
                            {
                                if(
                                    (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)      &&
                                    !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)   &&
                                    !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog) &&
                                    !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment))
                                {
                                    if(Pretty)
                                    {
                                        for (int i = 0; i < _Depth - 1; i++)
                                            _Streamer.write("\t", 1);
                                    }
                                    
                                    _Streamer.write("</", 2);
                                    _Streamer.write(_Node.get_name().data(), (int)_Node.get_name().size());
                                    
                                    if(Pretty)
                                        _Streamer.write(">\n", 2);
                                    else
                                        _Streamer.write(">", 1);
                                }
                            }
                        );

                        _Streamer.end();

                        return true;
                    }
                };
            
                typedef Writer<true>  PrettyWriter;
                typedef Writer<false> CompactWriter;
            }
        }
    }
}