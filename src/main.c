#include "tree.h"
#include "unit_test.h"

#ifdef RUN_UNIT_TESTS
void declare_tree_tests ();
#endif

int main (int argc, const char** argv)
{
#ifdef RUN_UNIT_TESTS
    declare_tree_tests();
    return unit_test_run(argc, argv);
#else
    return 0;
#endif
}
