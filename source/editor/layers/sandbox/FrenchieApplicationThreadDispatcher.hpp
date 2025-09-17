#pragma once

#include <FrenchieApplication.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

namespace Frenchie
{
    namespace Application
    {
        // Thread
        class Thread
        {
        public:
            Thread(const std::function<void()> _Worker, const std::string& _Name = STRINGIFY(Thread));
            virtual ~Thread();

            std::string get_name() const
            {
                return m_Name;
            }

            // API
            bool paused() const;
            bool finished() const;

            void pause();
            void resume();
            bool execute();

            void finish();

            void on_finished(const std::function<void()>&);

        private:
            std::function<void()> m_Worker    {nullptr};
            std::string           m_Name      {STRINGIFY(Thread)};
            std::function<void()> m_OnFinished{nullptr};

            bool     m_Paused  {false};
            std::atomic<bool>     m_Finished{false};

            mutable std::condition_variable m_Wait;
            mutable std::mutex              m_Mutex;
        };

        class ThreadDispatcher : public Layer
        {
        public:
            ThreadDispatcher() : Layer(STRINGIFY(Layer)){}
            virtual ~ThreadDispatcher(){}

            virtual void frame_start()
            {
                // launch
                while(!m_Queue.empty())
                {
                    auto process = m_Queue.front();

                    if(process != nullptr && process->execute())
                    {
                        m_Executed.push_back(process);
                    }

                    m_Queue.pop();
                }

                // finish
                // for(auto it = m_Executed.begin(); it != m_Executed.end(); it++)
                // {
                //     if((*it)->finished())
                //     {
                //         (*it)->finish();
                //         auto rm = it;
                //         it++;
                //         m_Executed.erase(rm);

                //         if(it == m_Executed.end())
                //             break;
                //     }
                // }
            }
            
            virtual void finish()
            {
                // cancel all on finish
                // for(auto it = m_Executed.begin(); it != m_Executed.end(); it++)
                //     (*it)->cancel();
            }

            virtual bool allows_multiple_instances() const
            {
                return false;
            }

            Frenchie::Core::Reference<Thread> push(const std::function<void()> _Worker, const std::string& _Name = STRINGIFY(Thread))
            {
                m_Queue.push(std::make_shared<Thread>(_Worker, _Name));
                return m_Queue.back();
            }

            std::list<std::shared_ptr<Thread>>::const_iterator begin() const
            {
                return m_Executed.begin();
            }

            std::list<std::shared_ptr<Thread>>::const_iterator end() const
            {
                return m_Executed.end();
            }

            // static API
            static Frenchie::Core::Reference<ThreadDispatcher> instance()
            {
                auto layer = Frenchie::Application::application()->find_layer<ThreadDispatcher>();
                
                if(layer == nullptr) 
                    layer = Frenchie::Application::application()->push_layer<ThreadDispatcher>();

                return layer;
            }

        protected:

            std::queue<std::shared_ptr<Thread>> m_Queue = 
                std::queue<std::shared_ptr<Thread>>();

            std::list<std::shared_ptr<Thread>> m_Executed =    
                std::list<std::shared_ptr<Thread>>();
        };

        class ThreadDispatcherView : public Layer
        {
        public:
            ThreadDispatcherView() : Layer(STRINGIFY(Layer)){}
            virtual ~ThreadDispatcherView(){}

            virtual void frame_update() override
            {
                ImGui::Begin(get_name().c_str());

                if(ImGui::Button("Go"))
                {
                    size_t counter = 0;

                    for(auto it = ThreadDispatcher::instance()->begin(); it != ThreadDispatcher::instance()->end(); ++it)
                    {
                        ++counter;
                    }

                    ThreadDispatcher::instance()->push(
                        []()
                        {
                            std::this_thread::sleep_for(std::chrono::seconds(4));
                        },
                        fmt::format("Thread_{}", counter));
                }

                // show threads
                size_t id = 0;

                for(auto it = ThreadDispatcher::instance()->begin(); it != ThreadDispatcher::instance()->end(); ++it)
                {
                    ImGui::TextUnformatted((*it)->get_name().c_str());
                    ImGui::SameLine();

                    ImGui::PushID(++id);
                    if(ImGui::Button("Pause"))(*it)->pause();
                    ImGui::PopID();

                    ImGui::SameLine();

                    ImGui::PushID(++id);
                    if(ImGui::Button("Resume"))(*it)->resume();
                    ImGui::PopID();

                    if((*it)->paused()) 
                    {
                        ImGui::SameLine();
                        ImGui::TextUnformatted("PAUSED...");
                    }

                    if((*it)->finished()) 
                    {
                        ImGui::SameLine();
                        ImGui::TextUnformatted("FINISHED...");
                    }
                }

                ImGui::End();
            }

            virtual bool allows_multiple_instances() const override
            {
                return false;
            }
        };
    }
}