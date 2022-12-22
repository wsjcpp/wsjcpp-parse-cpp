#ifndef WSJCPP_PARSER_CPP_H
#define WSJCPP_PARSER_CPP_H

#include <string>
#include <vector>
#include <map>

enum class WsjcppParserCppStatus {
    NONE = 1,
    START_PARSE_STRING = 2,
};

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

class WsjcppParserCpp {
    public:
        WsjcppParserCpp();

        bool parseFile(const std::string &sFilepath);
        std::string removeComments(const std::string &sContent);
        void parseByWords(const std::string &sContent);
        const std::vector<std::string> &getWords();

    private:

        void flushBuffer();
        void throwErrorUnknownSymbol(char c0, const std::string &sMessage);

        std::string TAG;
        std::string m_sBuffer;
        std::vector<std::string> m_vWords;
        WsjcppParserCppStatus m_nStatus;
};

#endif // WSJCPP_PARSER_CPP_H
