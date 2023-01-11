
#include <wsjcpp_core.h>
#include <wsjcpp_unit_tests.h>

// ---------------------------------------------------------------------
// UnitTestLayer1

class UnitTestLayer1 : public WsjcppUnitTestBase {
    public:
        UnitTestLayer1();
        virtual bool doBeforeTest() override;
        virtual void executeTest() override;
        virtual bool doAfterTest() override;
};

REGISTRY_WSJCPP_UNIT_TEST(UnitTestLayer1)

UnitTestLayer1::UnitTestLayer1()
    : WsjcppUnitTestBase("UnitTestLayer1") {
}

// ---------------------------------------------------------------------

bool UnitTestLayer1::doBeforeTest() {
    // do something before test
    return true;
}

// ---------------------------------------------------------------------

void UnitTestLayer1::executeTest() {
    compare("Not implemented", true, false);
    // TODO unit test source code here
}

// ---------------------------------------------------------------------

bool UnitTestLayer1::doAfterTest() {
    // do somethig after test
    return true;
}


