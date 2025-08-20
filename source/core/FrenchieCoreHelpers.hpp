#pragma once

#include <memory>
#include <string>
#include <codecvt>
#include <filesystem>

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
    {
        namespace Helpers
        {
            //Returns the absolute path of the executable
            std::filesystem::path get_exe_absolute_path();

            std::filesystem::path get_exe_absolute_directory();

            std::string get_file_extention(const std::filesystem::path& _Path);

            FILE* open_file(std::string _Path, std::string _Mode);

            template<typename T, int S = 512>
            class Stack final
            {
            public:

                Stack()
                {
                    container.resize(S);
                }

                std::vector<T> container;

                void push(const T& _Value)
                {
                    current           = next;
                    container[next++] = _Value;

                    if(next >= container.size()) 
                    {
                        container.resize(growth * container.size());
                        growth *= 2;
                    }
                }
                
                void pop()
                {
                    next--;
                    current--;
                }

                T& top()
                {
                    return container[current];
                }

                bool empty() const
                {
                    return current < 0;
                }

                int current = -1;
                int next    = 0;
                int growth  = 2;
            };

            template<typename T, int S = 512>
            class Queue final
            {
            public:

                Queue()
                {
                    container.resize(S);
                }

                std::vector<T> container;

                void push(const T& _Value)
                {
                    container[tail++] = _Value;

                    if(tail >= container.size()) 
                    {
                        container.resize(growth * container.size());
                        growth *= 2;
                    }
                }
                
                void pop()
                {
                    head++;
                }

                T& front()
                {
                    return container[head];
                }

                bool empty() const
                {
                    return head >= tail;
                }

                int head    = 0;
                int tail    = 0;
                int growth  = 2;
            };

            namespace String
            {
                std::vector<std::string> split(const std::string& _Input, const std::string& _Delimeter = " ");
                std::string to_upper(std::string);
                std::string to_lower(std::string);

                // from string conversion
                template<typename __type> 
                __type from_string(const std::string& _Input);

                // to string conversion
                template <typename __type>
                std::string to_string(const __type& _Input);
            }

            namespace Time
            {
                inline std::chrono::high_resolution_clock::time_point tic()
                {
                    return std::chrono::high_resolution_clock::now();
                }

                template< typename __type = std::chrono::nanoseconds >
                double elapsed(std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now)
                {
                    return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
                }
            }
        }    
    }
}