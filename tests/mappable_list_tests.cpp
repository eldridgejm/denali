#include <UnitTest++.h>
#include <iostream>
#include <string>

#include <denali/mappable_list.h>

TEST(MappableList)
{

    typedef denali::MappableList<std::string> MappableList;

    MappableList ml;

    int el1 = ml.insert("This is a test.");
    int el2 = ml.insert("This is also a test.");
    int el3 = ml.insert("And so is this.");

    typedef denali::MappableListIterator<MappableList> Iterator;



}

int main()
{
    return UnitTest::RunAllTests();
}
