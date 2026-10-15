#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <tuple>
#include <vector>
#include <utility>
#include <stdexcept>

namespace Frenchie
{
    namespace Engine
    {
        using Byte   = uint8_t;
        using Index  = uint32_t;
        using Packed = uint64_t;

        class Entity final
        {
        public:
            Entity(Index _Version, Index _Index) : Meta((static_cast<uint64_t>(_Version) << 32) | _Index){}
            Index version()    const { return static_cast<Index>(Meta >> 32); }
            Index identifier() const { return static_cast<Index>(Meta);       }
            bool operator==(const Entity& _Other) const { return _Other.Meta == Meta; }
            bool operator!=(const Entity& _Other) const { return _Other.Meta != Meta; }
            explicit operator Index() const noexcept {return static_cast<Index>(identifier());}
            static Index null(){return std::numeric_limits<Index>::max();}
        private:
            Packed Meta{0};
        };

        class EntityRegistry final
        {
        private:

            class Slot final
            {
            public:
                Slot(Index _Version, Index _Next, Byte _Alive) : Meta((static_cast<uint64_t>(_Version) << 32) | _Next), Alive(_Alive){}
                Index version() const { return static_cast<Index>(Meta >> 32); }
                Index next()    const { return static_cast<Index>(Meta);       }
                Byte  alive()   const { return Alive; };
            private:
                Packed Meta {0};
                Byte   Alive{0};
            };

            class IPool
            {
            public:
                IPool(){}
                virtual ~IPool(){}
                virtual void erase(const Entity& _Entity) = 0;
            };

            template <typename Component>
            class Pool final : public IPool
            {
            public:
            
                bool contains(const Entity& _Entity) const
                {
                    const Index id  = static_cast<Index>(_Entity);
                    const Index ptr = id < m_Pointers.size() ? m_Pointers[id] : null();
                    return ptr < m_Entities.size() && m_Entities[ptr] == _Entity;
                }

                template<typename ... Args>
                void insert(Entity _Entity, Args&& ... _Args)
                {
                    if(contains(_Entity)) return;
                    
                    Index id = static_cast<Index>(_Entity);

                    if(id >= m_Pointers.size())
                        m_Pointers.resize(id + 1, null());

                    try
                    {
                        m_Components.emplace_back(std::forward<Args>(_Args)...);

                        try
                        {
                            m_Entities.push_back(_Entity);
                        }
                        catch(...)
                        {
                            m_Components.pop_back();
                            return;
                        }
                    }
                    catch(...)
                    {
                        return;
                    }

                    m_Pointers[id] = (Index)m_Components.size() - 1;
                }

                virtual void erase(const Entity& _Entity) override
                {
                    if (!contains(_Entity)) return;

                    const Index id   = static_cast<Index>(_Entity);
                    const Index ptr  = m_Pointers[id];
                    const Index last = m_Components.size() - 1;

                    if (ptr != last)
                    {
                        m_Components[ptr] = std::move(m_Components[last]);
                        m_Entities  [ptr] = m_Entities[last];
                        m_Pointers  [static_cast<Index>(m_Entities[ptr])] = ptr;
                    }

                    m_Components.pop_back();
                    m_Entities.pop_back();
                    m_Pointers[id] = null();
                }

                Component* request(const Entity& _Entity)
                {
                    return
                        contains(_Entity) ?
                            &m_Components[m_Pointers[static_cast<Index>(_Entity)]] :
                                nullptr;
                }

                std::size_t size() const
                {
                    return m_Entities.size();
                }

                const Entity& entity_at(std::size_t _Index) const
                {
                    return m_Entities[_Index];
                }

                template<typename Function>
                void for_each(Function&& _Function)
                {
                    for (std::size_t i = 0; i < m_Components.size(); ++i)
                        _Function(m_Entities[i], m_Components[i]);
                }

                template<typename Function>
                void for_each(Function&& _Function) const
                {
                    for (std::size_t i = 0; i < m_Components.size(); ++i)
                        _Function(m_Entities[i], m_Components[i]);
                }

            private:
                std::vector<Index>     m_Pointers;
                std::vector<Entity>    m_Entities;
                std::vector<Component> m_Components;

                static Index null()
                {
                    return std::numeric_limits<Index>::max();
                }
            };

            template<typename First, typename... Rest>
            class View final
            {
            private:
                class Iterator final
                {
                public:
                    using iterator_category = std::forward_iterator_tag;
                    using value_type        = std::tuple<Entity, First, Rest...>;
                    using difference_type   = std::ptrdiff_t;
                    using reference         = std::tuple<Entity, First&, Rest&...>;

                    Iterator() = default;

                    Iterator(View* _View, std::size_t _Index) : m_View(_View), m_Index(_Index)
                    {
                        skip_non_matching();
                    }

                    reference operator*() const
                    {
                        const Entity entity = m_View->m_FirstPool->entity_at(m_Index);

                        return reference(
                            entity,
                            *m_View->m_Registry.template request_component<First>(entity),
                            *m_View->m_Registry.template request_component<Rest>(entity)...);
                    }

                    Iterator& operator++()
                    {
                        ++m_Index;
                        skip_non_matching();
                        return *this;
                    }

                    Iterator operator++(int)
                    {
                        Iterator previous = *this;
                        ++(*this);
                        return previous;
                    }

                    bool operator==(const Iterator& _Other) const
                    {
                        return m_View == _Other.m_View && m_Index == _Other.m_Index;
                    }

                    bool operator!=(const Iterator& _Other) const
                    {
                        return !(*this == _Other);
                    }

                private:
                    View*       m_View {nullptr};
                    std::size_t m_Index{0};

                    void skip_non_matching()
                    {
                        while(m_Index < m_View->m_FirstPool->size() &&
                              !m_View->matches(m_View->m_FirstPool->entity_at(m_Index)))
                            ++m_Index;
                    }
                };

                EntityRegistry& m_Registry;
                Pool<First>*    m_FirstPool;

                bool matches(const Entity& _Entity) const
                {
                    return m_Registry.is_not_null(_Entity) &&
                        ((m_Registry.template request_component<First>(_Entity) != nullptr) &&
                         ... &&
                         (m_Registry.template request_component<Rest>(_Entity) != nullptr));
                }

            public:
                
                explicit View(EntityRegistry& _Registry) :
                    m_Registry(_Registry),
                    m_FirstPool(_Registry.template request_pool<First>()){}

                Iterator begin(){ return Iterator(this, 0); }
                Iterator end()  { return Iterator(this, m_FirstPool->size()); }
            };

            std::vector<Slot> m_Slots;
            Index             m_Next {Entity::null()};

            std::map<
                std::type_index,
                    std::unique_ptr<IPool>> m_Pools;

            template<typename Type>
            Pool<Type>* request_pool()
            {
                if(auto found = m_Pools.find(typeid(Type)); found == m_Pools.end())
                    m_Pools[typeid(Type)] = std::unique_ptr<IPool>(new Pool<Type>());

                return dynamic_cast<Pool<Type>*>(m_Pools[typeid(Type)].get());
            }

        public:
            EntityRegistry(){}
            EntityRegistry(const EntityRegistry&) = delete;
            EntityRegistry& operator=(const EntityRegistry&) = delete;

            // entity
            bool is_null(const Entity& _Entity) const
            {
                return !is_not_null(_Entity);
            }

            bool is_not_null(const Entity& _Entity) const
            {
                const Index id = static_cast<Index>(_Entity);
                return id < m_Slots.size() && m_Slots[id].alive() && m_Slots[id].version() == _Entity.version();
            }

            Entity create_entity()
            {
                if(m_Next == Entity::null())
                {
                    m_Slots.push_back(Slot(0, m_Next, 1));
                    return Entity(m_Slots.back().version(), m_Slots.size() - 1);
                }

                Index index = m_Next;
                m_Next = m_Slots[index].next();
                m_Slots[index] = Slot(m_Slots[index].version(), Entity::null(), true);
                
                return Entity(m_Slots[index].version(), index);
            }

            void remove_entity(const Entity& _Entity)
            {
                if(is_null(_Entity)) return;

                // remove all entity components
                for(auto& pool : m_Pools)
                    pool.second->erase(_Entity);

                // remove entity
                const Index id = static_cast<Index>(_Entity);
                m_Slots[id] = Slot(m_Slots[id].version() + 1, m_Next, false);
                m_Next = id;
            }

            // component
            template<typename Type, typename ... Args>
            void create_component(const Entity& _Entity, Args&&... _Args)
            {
                if(auto pool = request_pool<Type>(); pool != nullptr && is_not_null(_Entity))
                    pool->insert(_Entity, std::forward<Args>(_Args)...);
            }

            template<typename Type>
            Type* request_component(const Entity& _Entity)
            {
                if(auto pool = request_pool<Type>(); pool != nullptr && is_not_null(_Entity))
                    return pool->request(_Entity);
                return nullptr;
            }

            template<typename Type>
            void remove_component(const Entity& _Entity)
            {
                if(auto pool = request_pool<Type>(); pool != nullptr && is_not_null(_Entity))
                    pool->erase(_Entity);
            }

            template<typename First, typename... Rest>
            View<First, Rest...> view()
            {
                return View<First, Rest...>(*this);
            }
        
            // iteration
            template<typename First, typename... Rest, typename Function>
            void for_each(Function&& function)
            {
                if(auto pool = request_pool<First>(); pool != nullptr)
                {
                    pool->for_each([this, &function](const Entity& entity, First& first)
                    {
                        if (is_null(entity))
                            return;

                        if ((... && (request_component<Rest>(entity) != nullptr)))
                            function(entity, first, *request_component<Rest>(entity)...);
                    });
                }
            }
        };
    }
}
