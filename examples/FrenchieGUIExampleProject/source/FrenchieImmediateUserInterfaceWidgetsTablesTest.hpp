#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceTablesTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceTablesTest();
            virtual ~FrenchieImmediateUserInterfaceTablesTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI {nullptr};

            int            m_ColumnsCount        {1000};
            int            m_RowsCount           {500 };
            bool           m_RenderColumnHeaders {true};
            bool           m_RenderRowHeaders    {true};
            bool           m_RenderCornerHeader  {true};
            std::string    m_TableDataCellString {std::string()};
        };
    }
}