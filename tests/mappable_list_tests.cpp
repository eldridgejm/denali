#include <UnitTest++.h>
#include <iostream>
#include <string>

#include <denali/mappable_list.h>

TEST(MappableList)
{

    typedef denali::MappableList<std::string> MappableList;

    MappableList ml;


}

int main()
{
    return UnitTest::RunAllTests();
}
