
#include <wsjcpp_core.h>
#include <wsjcpp_unit_tests.h>
#include "wsjcpp_parser_cpp.h"

// ---------------------------------------------------------------------
// UnitTestParseFunction

class UnitTestParseFunction : public WsjcppUnitTestBase {
    public:
        UnitTestParseFunction();
        virtual bool doBeforeTest() override;
        virtual void executeTest() override;
        virtual bool doAfterTest() override;
};

REGISTRY_WSJCPP_UNIT_TEST(UnitTestParseFunction)

UnitTestParseFunction::UnitTestParseFunction()
    : WsjcppUnitTestBase("UnitTestParseFunction") {
}

// ---------------------------------------------------------------------

bool UnitTestParseFunction::doBeforeTest() {
    // do something before test
    return true;
}

// ---------------------------------------------------------------------

void UnitTestParseFunction::executeTest() {

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
    parser.parseByTokens(sTestContent, "UnitTestParseFunction.cpp");
    const std::vector<std::string> &vWords = parser.getTokens();
    int nMin = std::min(vWords.size(), vExpectedWords.size());

    // for (int i = 0; i < vWords.size(); i++) {
    //     std::string sRes = "OK";
    //     if (vWords[i] != vExpectedWords[i]) {
    //         sRes = "ERR";
    //     }
    //     std::cout << i << ": '" << vWords[i] << "'  '" << vExpectedWords[i] << "' " << sRes << std::endl;
    // }

    for (int i = 0; i < nMin; i++) {
        if (!compare("found words", vWords[i], vExpectedWords[i])) {
            std::cout << i << ": expected '" << vExpectedWords[i] << "', but got '" << vWords[i] << "' " << std::endl;
            break;
        }
    }
    compare("found words", vWords.size(), vExpectedWords.size());
}

// ---------------------------------------------------------------------

bool UnitTestParseFunction::doAfterTest() {
    // do somethig after test
    return true;
}


