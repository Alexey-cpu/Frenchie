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
        class App
        {
        public:
            App() = delete;
            App(const App&) = delete;
            App& operator=(const App&) = delete;

            typedef std::list<std::shared_ptr<Layer>>::const_iterator const_iterator;

            // API
            /**
             * @brief Detects if application is closed
             * @return returns true if application is closed 
             */
            static bool is_closed();

            /**
             * @brief Forces application to close
             */
            static void close();

            /**
             * @brief Application launching function
             * @return returns 1 if application is running, otherwise returns 0
             * @details this function launches application and executes untill application is closed.
             */
            static int execute();

            /**
             * @brief application layers list begin iterator
             * @return returns interator to the beginning of application layers list
             */
            static const_iterator begin();

            /**
             * @brief application layers list end iterator
             * @return returns interator to the end of application layers list
             */
            static const_iterator end();

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
            static std::shared_ptr<Type> push_layer(Arguments... _Parameters)
            {
                // return found layer if it does not allow multiple instances
                std::shared_ptr<Type> found = find_layer<Type>();
                if(found != nullptr && !found->allows_multiple_instances())
                    return found;

                // create layer
                auto layer = std::make_shared<Type>(_Parameters...);
                m_Awakes.push_back(layer);
                return layer;
            }

            /**
             * @brief Function to find layer of a given type within application layers list
             * @return returns layer if it's found or nullptr otherwise
             */
            template<typename Type>
            static std::shared_ptr<Type> find_layer()
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

        protected:

            static bool awake();
            static void frame_start();
            static void frame_update();
            static void frame_input();
            static void frame_render();
            static void frame_finish();
            static void finish();
            static void quit();

            static std::list<std::shared_ptr<Layer>> m_Layers;
            static std::list<std::shared_ptr<Layer>> m_Awakes;
        };

        /*! @} */
    };
};