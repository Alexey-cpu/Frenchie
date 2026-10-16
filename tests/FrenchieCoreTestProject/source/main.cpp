#include <FrenchieCoreMathTest.hpp>
#include <FrenchieCoreSerializationTest.hpp>
#include <FrenchieCoreECSTests.hpp>

int main(int argc, char *argv[])
{
    // math core tests
    Frenchie::Core::Tests::frenchie_math_core_gs_complex_test();
    Frenchie::Core::Tests::frenchie_math_core_gs_vector_test();
    Frenchie::Core::Tests::frenchie_math_core_gs_matrix_test();
    Frenchie::Core::Tests::frenchie_math_core_deterministic_test();

    // ECS core tests
    Frenchie::Core::Tests::frenchie_core_ecs_entity_lifecycle_test();
    Frenchie::Core::Tests::frenchie_core_ecs_components_test();
    Frenchie::Core::Tests::frenchie_core_ecs_view_test();

    // serialization core tests
    Frenchie::Core::Tests::frenchie_core_serialization_dom_tree_test();
    Frenchie::Core::Tests::frenchie_core_serialization_xml_test();
    Frenchie::Core::Tests::frenchie_core_serialization_json_test();

    return 0;
}