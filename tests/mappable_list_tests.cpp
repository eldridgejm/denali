#include <UnitTest++.h>
#include <iostream>
#include <string>

#include <denali/mappable_list.h>

TEST(MappableList)
{

    typedef denali::MappableList<std::string> MappableList;

    MappableList ml;

    ml.insert("This is a test.");
    ml.insert("This is also a test.");
    ml.insert("And so is this.");

}

int main()
{
    return UnitTest::RunAllTests();
}
