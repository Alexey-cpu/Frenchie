#include <FrenchieCoreMathTest.hpp>
#include <FrenchieCoreSerializationTest.hpp>

int main(int argc, char *argv[])
{
    // math core tests
    Frenchie::Core::Tests::frenchie_math_core_gs_complex_test();
    Frenchie::Core::Tests::frenchie_math_core_gs_vector_test();
    Frenchie::Core::Tests::frenchie_math_core_gs_matrix_test();

    // serialization core tests
    Frenchie::Core::Tests::frenchie_core_serialization_dom_tree_test();
    Frenchie::Core::Tests::frenchie_core_serialization_xml_test();
    Frenchie::Core::Tests::frenchie_core_serialization_json_test();

    return 0;
}