#include <UnitTest++.h>
#include <iostream>

#include <denali/concepts/check.h>
#include <denali/concepts/common.h>
#include <denali/concepts/graph_attributes.h>

TEST(GraphAttributes)
{
    denali::concepts::checkSelfConsistent <
            denali::concepts::NodeIterable<
            denali::concepts::Null> > ();

}

int main()
{
    return UnitTest::RunAllTests();
}
