#pragma once

// Core
#include <FrenchieCoreProcess.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationSynchronousTimer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <FrenchieCoreTextDocument.hpp>

#include <mutex>

namespace Frenchie
{
    namespace Editor
    {
        /*
        template<typename Type>
        class AsyncDataUpdater
        {
        public:

            typedef Type type;

            AsyncDataUpdater()
            {
                Frenchie::Core::ThreadPool::instance()->enqueue([this](){produce();});
            }

            virtual ~AsyncDataUpdater()
            {
                // stop producer and consumer threads
                m_Stoped = true;

                // wait untill consumer and producer finish their job
                m_Wait.notify_all();
                while(!m_Finished); // wait untill finish
            }

            // public API
            void enqueue(const std::function<void()>& _Function)
            {
                // here we should place brackets as 'std::unique_lock<std::mutex>'
                // unlocks only when it goes out-of scope...
                {
                    std::unique_lock<std::mutex> lock(m_Mutex);
                    
                    if(!m_Stoped)
                        m_Commands.push(std::move(_Function));
                }
                
                // notify a single thread to start executing task
                m_Wait.notify_one();
            }

            // public virtual API
            virtual type produce_data() = 0;

            Type& get_data() const
            {
                std::unique_lock<std::mutex> lock(m_Mutex);

                if(!m_Datas.empty()) 
                {
                    m_Data = m_Datas.front();
                    m_Datas.pop();
                }

                return m_Data;
            }

        protected:

            mutable std::condition_variable           m_Wait;
            mutable std::mutex                        m_Mutex;
            mutable std::queue<std::function<void()>> m_Commands;
            mutable Type                              m_Data;
            mutable std::atomic<bool>                 m_Stoped  {false};
            mutable std::atomic<bool>                 m_Finished{false};
            mutable std::queue<Type>                  m_Datas;

            void produce()
            {
                while(!m_Stoped)
                {
                    // update data
                    {
                        // wait untill commands queue is not empty
                        std::unique_lock<std::mutex> lock(m_Mutex);
                        m_Wait.wait(lock, [this](){return m_Stoped || !m_Commands.empty();});

                        if(m_Stoped || m_Commands.empty()) break;

                        auto then = Frenchie::Core::Time::tic();

                        // execute commands
                        m_Commands.push([this](){m_Datas.push(produce_data());});

                        while(!m_Commands.empty())
                        {
                            auto command = m_Commands.front();

                            if(command != nullptr) 
                                command();

                            m_Commands.pop();
                        }

                        //std::cout << "elapsed " << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << "\n";

                        m_Wait.notify_one();
                    }
                }

                //std::cout << "producer finished ... \n";
                m_Finished = true;
            }
        };

        struct Text
        {
            std::wstring Text;
            int          Rows;
        };

        class TextModel : public AsyncDataUpdater<Text>
        {
        public:
            TextModel(const std::wstring& _PieceTable) : 
                m_PieceTable(std::make_unique<Frenchie::Core::PieceTable>(_PieceTable)){}

            ~TextModel(){}

            std::unique_ptr<Frenchie::Core::PieceTable> m_PieceTable;

            int m_Start = 0;
            int m_End   = 0;
            int m_Rows   = 0;

            void request(int _Start, int _End)
            {
                if(m_Start == _Start && m_End == _End) return;

                enqueue([this, _Start, _End]()
                {
                    m_Start = _Start;
                    m_End   = _End;
                });
            }

            virtual type produce_data() override
            {
                std::wstring text = std::wstring();
                int          rows = 0;

                for(auto it = m_PieceTable->begin(); it != m_PieceTable->end(); it++)
                {
                    for (int i = 0; i < it->Length; i++)
                    {
                        if(it->Buffer->at(i) == '\n') ++rows;

                        if(rows >= m_Start && rows <= m_End)
                            text += it->Buffer->at(i);

                        // if(rows >= m_End)
                        //     return {text, m_Rows};
                    }
                }

                m_Rows = m_PieceTable->get_lines_count();

                return {text, m_Rows};
            }
        };
        */

        class PieceTableDrawer : public Frenchie::Application::Layer
        {
        public:
            PieceTableDrawer();
            virtual ~PieceTableDrawer();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual bool allows_multiple_instances() const override;

        protected:
            std::unique_ptr<Frenchie::Core::TextDocument> m_Table;
            ImVec2 m_CursorGeometricalPosition;

            // commands
            void document_insert_symbol_command();
            void document_move_cursor_left_command();
            void document_move_cursor_right_command();
            void document_move_cursor_down_command();
            void document_move_cursor_up_command();
        };
    }
}