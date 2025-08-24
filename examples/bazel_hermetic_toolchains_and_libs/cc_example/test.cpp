#include "lib.h"

#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions)
{
    printMessage("Unwrap Your Build");
    EXPECT_EQ(7 * 6, 42);
}
