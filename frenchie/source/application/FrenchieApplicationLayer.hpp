#pragma once

// STL
#include <algorithm>
#include <string>
#include <memory>
#include <list>

/*! \defgroup <Application> (Application)
*  @brief The module contains application launching instance.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationLayer> (Application layer)
        *  @ingroup Application
        *  @brief The module contains application loop class.
        *  @{
        */

        /**
         * @brief This class implements applipcation layer.
         * @class Layer
         */
        class Layer
        {
        public:

            /**
             * @brief Construct a new Layer object
             * @param _Name layer name 
             */
            Layer(const std::string& _Name = std::string());

            /**
             * @brief default destructor
             */
            virtual ~Layer();

            /**
             * @brief returns this layer name
             * @return returns this layer name
             */
            std::string get_name() const;

            /**
             * @brief sets this layer name
             * @param _Name wanted name for this layer
             */
            void set_name(const std::string& _Name);

            /**
             * @brief Checks if this layer is closed
             * @return returns true if layer is closed.
             * @details if layer is closed it's poped out-of allication layers list.
             */
            bool is_closed() const;

            /**
             * @brief clsoses layer
             */
            void close();

            // This is layer initialization function that is called once when the layer is pushed into application pipeline

            /**
             * @brief This is layer initialization function that is called once when the layer is pushed into application pipeline
             * @return returns true if layer initialization succeeded
             */
            virtual bool awake();

            /**
             * @brief In this function we prepare layer for current frame processing. Ususally we compute projection and camera matrixes here.
             */
            virtual void frame_start();

            /**
             * @brief In this function we usually modify geometry, compute transform matrixes, prepare rendering commands e.t.c
             */
            virtual void frame_update();

            /**
             * @brief In this function we usually catch application input and it's context window events.
             */
            virtual void frame_input();

            /**
             * @brief In this function rendering commands are executed. Essentially in sends all geometry onto GPU.
             */
            virtual void frame_render();

            /**
             * @brief In this function we usually do post processing after rendering.
             */
            virtual void frame_finish();

            /**
             * @brief In this function we safelly stop layer processing before it's poped out-of application layers list.
             */
            virtual void finish();

            /**
             * @brief In this functrion we safelly clean-up layer resources when application is closed.
             */
            virtual void quit();

            // This function indentified if the layer can have multiple instances

            /**
             * @brief This function defines if layer can have multiple instances
             * @returns returns true if layer allows multiple instances
             */
            virtual bool allows_multiple_instances() const;

        protected:

            // info
            std::string m_Name   = "DefaultLayer";
            bool        m_Opened = true;
        };

        /*! @} */
    };
};
