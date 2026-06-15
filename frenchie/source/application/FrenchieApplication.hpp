#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

/*! \defgroup <Application> (Application)
*  @brief The module contains application launching instance.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationInstance> (Application)
        *  @ingroup Application
        *  @brief The module contains application loop class.
        *  @{
        */

        /**
         * @brief Represents appliaction instance
         * @details This class reprsents layered appication loop that executes range of layer processing functions untill the application is closed.
         * Every application layer is responsible for a limited scope of functions and operations. Besides layers Frenchie uses platform and rendering backends.
         * 
         * Platform backend abstracts system specific functions for context window creation, manipulation and events catching.
         * Graphics backend abstracts graphics processing unit (GPU) rendering API and is in charge of loading stuff on GPU for rendering.
         */
        class ApplicationInstance
        {
        public:
            ApplicationInstance();
            virtual ~ApplicationInstance();

            typedef std::list<std::shared_ptr<Layer>>::const_iterator const_iterator;

            // API
            /**
             * @brief Detects if application is closed
             * @return returns true if application is closed 
             */
            bool is_closed();

            /**
             * @brief Forces application to close
             */
            void close();

            /**
             * @brief Application launching function
             * @return returns 1 if application is running, otherwise returns 0
             * @details this function launches application and executes untill application is closed.
             */
            int execute();

            /**
             * @brief application layers list begin iterator
             * @return returns interator to the beginning of application layers list
             */
            const_iterator begin() const;

            /**
             * @brief application layers list end iterator
             * @return returns interator to the end of application layers list
             */
            const_iterator end() const;

            /**
             * @brief Application layers list size
             * @return returns number of layers in application layers list
             */
            size_t size() const;

            /**
             * @brief Function to push layer within application layers list
             * 
             * @tparam Type pushed layer type 
             * @tparam Arguments arguments used by a pushed layer constructor
             * @param  _Parameters parameters for layer constructor
             * @return returns pushed layer.
             * @details Function creates layer, pushes it into application layers list and returns pushed layer.
             * If layer does not allow multiple instances and application layers list already contains layer
             * the contained instance is returned.
             */
            template<typename Type, typename ... Arguments>
            std::shared_ptr<Type> push_layer(Arguments... _Parameters)
            {
                // create layer
                auto layer = std::make_shared<Type>(_Parameters...);

                // check if layer allows multiple instances
                if(contains_layer<Type>() && !layer->allows_multiple_instances())
                    return find_layer<Type>();

                m_Awakes.push_back(layer);
                return layer;
            }

            /**
             * @brief Function to find layer of a given type within application layers list
             * @return returns layer if it's found or nullptr otherwise
             */
            template<typename Type>
            std::shared_ptr<Type> find_layer()
            {
                auto layer = std::find_if(
                    m_Layers.begin(),
                    m_Layers.end(),
                    [](std::shared_ptr<Layer> _Layer)->bool
                    {
                        return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                    }
                );

                return layer != m_Layers.end() ? std::dynamic_pointer_cast<Type>(*layer) : nullptr;
            }

            /**
             * @brief Detects if the layer of a given type exists within application layers list
             * @return returns true if the layer of a given type exists within application layers list
             */
            template<typename Type>
            bool contains_layer()
            {
                return std::find_if(
                        m_Layers.begin(),
                        m_Layers.end(),
                        [](std::shared_ptr<Layer> _Layer)->bool
                        {
                            return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                        }) != m_Layers.end();
            }

        protected:

            bool awake();
            void frame_start();
            void frame_update();
            void frame_input();
            void frame_render();
            void frame_finish();
            void finish();
            void quit();

            std::list<std::shared_ptr<Layer>> m_Layers;
            std::list<std::shared_ptr<Layer>> m_Awakes;
        };

        /**
         * @brief returns application instance
         */
        Frenchie::Application::ApplicationInstance* application();

        /*! @} */
    };
};