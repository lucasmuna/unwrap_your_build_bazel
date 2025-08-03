#include "lib.h"

/*
 * We could have used GTEST and/or tested `example.cpp` it self but, as it
 * would require more setup, I decided to keep it simple and just show how
 * to create a simple `cc_test`.
 */

int main()
{
    printMessage("Unwrap Your Build");
    return 0;
}
