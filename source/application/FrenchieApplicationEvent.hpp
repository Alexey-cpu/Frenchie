// STL
#include <queue>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace Events
        {
            class Event
            {
                public:
                    Event(unsigned int _ID) :  m_ID(_ID){}
                    virtual ~Event(){}

                protected:
                    int m_ID = -1;
            };

            class MouseEvent : public Event
            {
                public:
                    MouseEvent(unsigned int _ID, glm::vec3 _Position) : Event(_ID), m_Position(_Position){}
                    virtual ~MouseEvent(){}

                protected:
                    glm::vec3 m_Position;
            };

            class KeyEvent : public Event
            {
                public:
                    KeyEvent(unsigned int _ID) : Event(_ID){}
                    virtual ~KeyEvent(){}
                protected:
                    glm::vec3 m_Position;
            };

            class EventLoop
            {
                public:
                    EventLoop(){}
                    virtual ~EventLoop(){}

                protected:
                    std::queue<Event> m_EventQueue = std::queue<Event>();
            };
        }
    }
};