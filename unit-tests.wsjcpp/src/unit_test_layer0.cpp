
#include <wsjcpp_core.h>
#include <wsjcpp_unit_tests.h>
#include "wsjcpp_parser_cpp.h"

// ---------------------------------------------------------------------
// UnitTestLayer0

class UnitTestLayer0 : public WsjcppUnitTestBase {
    public:
        UnitTestLayer0();
        virtual bool doBeforeTest() override;
        virtual void executeTest() override;
        virtual bool doAfterTest() override;
};

REGISTRY_WSJCPP_UNIT_TEST(UnitTestLayer0)

UnitTestLayer0::UnitTestLayer0()
    : WsjcppUnitTestBase("UnitTestLayer0") {
}

// ---------------------------------------------------------------------

bool UnitTestLayer0::doBeforeTest() {
    // do something before test
    return true;
}

// ---------------------------------------------------------------------

void UnitTestLayer0::executeTest() {
    std::string sTestContent = 
        "// hello \n"
        "int main(int argc, const char* argv[]) {\n"
        "    /* test1 \n"
        "    some  */\n"
        "    std::string TAG = \"MAIN\\\"test\";\n"
        "    return 0;\n"
        "};\n"
    ;
    std::vector<std::string> vExpectedWords = {
        "// hello ",
        "int", "main", "(", "int", "argc", ",", "const", "char", "*", "argv", "[", "]", ")",
        "{",
            "/* test1 \n    some  */",
            "std", "::", "string", "TAG", "=", "\"MAIN\\\"test\"", ";",
            "return", "0", ";", 
        "}", ";",
    };

    WsjcppParserCppLayer0 parser;
    parser.parseByTokens(sTestContent, "unit_test_layer0_sample.cpp");
    const std::vector<WsjcppParserCppLayer0Token> &vTokens = parser.getTokens();
    int nMin = std::min(vTokens.size(), vExpectedWords.size());

    for (int i = 0; i < nMin; i++) {
        std::string sToken = vTokens[i].getValue();

        if (!compare("found words", sToken, vExpectedWords[i])) {
            std::cout << i << ": expected '" << vExpectedWords[i] << "', but got '" << sToken << "' " << std::endl;
            break;
        }
    }
    compare("found words", vTokens.size(), vExpectedWords.size());
}

// ---------------------------------------------------------------------

bool UnitTestLayer0::doAfterTest() {
    // do somethig after test
    return true;
}


