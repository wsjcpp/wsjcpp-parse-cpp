#ifndef WSJCPP_PARSER_CPP_H
#define WSJCPP_PARSER_CPP_H

#include <string>
#include <vector>
#include <map>



class WsjcppParserCppCursor {
    public:
        WsjcppParserCppCursor(const std::string &sContent);
        bool next();

        char getC0();
        char getC1();

        bool isSeparatedChar();
        bool isSkipEmptyChars();
        bool isNumeric();
        bool isAllowedPair();

    private:
        std::string m_sContent;
        char m_c0;
        char m_c1;
        int m_nCurrentPos;
        int m_nLength;
};

enum class WsjcppParserCppLayer0Status {
    NONE = 1,
    START_PARSE_STRING = 2,
    START_PARSE_ONELINE_COMMENT = 3,
    START_PARSE_MULTILINE_COMMENT = 4,
};

class WsjcppParserCppLayer0Token {
    public:
        WsjcppParserCppLayer0Token(const std::string &sToken);
        const std::string &getValue();
    private:
        std::string m_sToken;
};

class WsjcppParserCppLayer0 {
    public:
        WsjcppParserCppLayer0();
        bool parseByTokens(const std::string &sContent);
        const std::vector<std::string> &getTokens();
        

    private:
        void flushBuffer();
        void throwErrorUnknownSymbol(char c0, const std::string &sMessage);
        
        std::string TAG;
        std::string m_sBuffer;
        WsjcppParserCppLayer0Status m_nStatus;
        std::vector<std::string> m_vTokens;
};

class WsjcppParserCpp {
    public:
        WsjcppParserCpp();

        bool parse(const std::string &sFilepath);
        
    private:
        std::string TAG;
};

#endif // WSJCPP_PARSER_CPP_H
