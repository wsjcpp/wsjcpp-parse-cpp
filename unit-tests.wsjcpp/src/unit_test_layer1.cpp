
#include <wsjcpp_core.h>
#include <wsjcpp_unit_tests.h>
#include "wsjcpp_parser_cpp.h"

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
    std::string sTestContent = 
        "// hello \n"
        "#include <iostream>\n"
        "int main(int argc, const char* argv[]) {\n"
        "    /* test1 \n"
        "    some  */\n"
        "    std::string TAG = \"MAIN\\\"test\";\n"
        "    return 0;\n"
        "};\n"
    ;
    std::vector<std::string> vExpectedWords0 = {
        "// hello ",
        "#", "include", "<", "iostream", ">",
        "int", "main", "(", "int", "argc", ",", "const", "char", "*", "argv", "[", "]", ")",
        "{",
            "/* test1 \n    some  */",
            "std", "::", "string", "TAG", "=", "\"MAIN\\\"test\"", ";",
            "return", "0", ";", 
        "}", ";",
    };

    WsjcppParserCppLayer0 parser0;
    parser0.parseByTokens(sTestContent, "unit_test_layer0_sample.cpp");
    const std::vector<WsjcppParserCppLayer0Token> &vTokens0 = parser0.getTokens();
    int nMin = std::min(vTokens0.size(), vExpectedWords0.size());

    for (int i = 0; i < nMin; i++) {
        std::string sToken = vTokens0[i].getValue();

        if (!compare("found words", sToken, vExpectedWords0[i])) {
            std::cout << i << ": expected '" << vExpectedWords0[i] << "', but got '" << sToken << "' " << std::endl;
            break;
        }
    }
    compare("found tokens (0)", vTokens0.size(), vExpectedWords0.size());

    // next layer
    

    std::vector<WsjcppParserCppLayer1Token> vExpectedWords1 = {
        WsjcppParserCppLayer1Token(WsjcppParserCppLayer1TokenType::COMMENT, "// hello"),
        WsjcppParserCppLayer1Token(WsjcppParserCppLayer1TokenType::INCLUDE, "<iostream>"),
        WsjcppParserCppLayer1Token(WsjcppParserCppLayer1TokenType::RETURN_TYPE, "int"),
        WsjcppParserCppLayer1Token(WsjcppParserCppLayer1TokenType::FUNCTION_NAME, "main")
        // "int", "main", "(", "int", "argc", ",", "const", "char", "*", "argv", "[", "]", ")",
        // "{",
        //     "/* test1 \n    some  */",
        //     "std", "::", "string", "TAG", "=", "\"MAIN\\\"test\"", ";",
        //     "return", "0", ";", 
        // "}", ";",
    };
    WsjcppParserCppLayer1 parser1;
    parser1.parseByTokens(vTokens0, "unit_test_layer0_sample.cpp");
    const std::vector<WsjcppParserCppLayer1Token> &vTokens1 = parser1.getTokens();
    int nMin1 = std::min(vTokens1.size(), vExpectedWords1.size());
    for (int i = 0; i < nMin1; i++) {
        std::string sGotToken1 = vTokens1[i].getValue();
        std::string sExpectedToken1 = vExpectedWords1[i].getValue();
        WsjcppParserCppLayer1TokenType nGotToken1Type = vTokens1[i].getType();
        WsjcppParserCppLayer1TokenType nExpectedToken1Type = vExpectedWords1[i].getType();
        

        if (
            !compare("token1-value", sGotToken1, sExpectedToken1)
            || !compare("token1-type", int(nGotToken1Type), int(nExpectedToken1Type))
        ) {
            std::cout << i << ": expected '" << sExpectedToken1 << "', but got '" << sGotToken1 << "' " << std::endl;
            break;
        }
    }
    compare("found tokens (1)", vTokens1.size(), vExpectedWords1.size());
}

// ---------------------------------------------------------------------

bool UnitTestLayer1::doAfterTest() {
    // do somethig after test
    return true;
}


