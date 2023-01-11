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

        int getLineNumber();
        int getPositionInLine();

    private:
        std::string m_sContent;
        char m_c0;
        char m_c1;
        int m_nCurrentPos;
        int m_nLength;
        int m_nLineNumber;
        int m_nPositionInLine;
};

enum class WsjcppParserCppLayer0Status {
    NONE = 1,
    START_PARSE_STRING_DOUBLE_QUOTES = 20,
    START_PARSE_STRING_SINGLE_QUOTES = 21,
    START_PARSE_ONELINE_COMMENT = 30,
    START_PARSE_MULTILINE_COMMENT = 40,
};

class WsjcppParserCppLayer0Buffer {
    public:
        WsjcppParserCppLayer0Buffer(const std::string &sFilepath);
        void reset();
        void append(WsjcppParserCppCursor *pCur);

        bool isEmpty() const;
        const std::string &getValue() const;
        int getLineNumber() const;
        int getPositionInLine() const;
        const std::string &getFilepath() const;
    private:
        std::string m_sBuffer;
        int m_nLength;
        int m_nLineNumber;
        int m_nPositionInLine;
        std::string m_sFilepath;
};

class WsjcppParserCppLayer0Token {
    public:
        WsjcppParserCppLayer0Token(
            const WsjcppParserCppLayer0Buffer *pToken
        );
        const std::string &getValue() const;
        int getLineNumber() const;
        int getPositionInLine() const;
        const std::string &getFilepath() const;
    private:
        std::string m_sToken;
        int m_nLine;
        int m_nPositionInLine;
        std::string m_sFilepath;
};

class WsjcppParserCppLayer0 {
    public:
        WsjcppParserCppLayer0();
        bool parseByTokens(const std::string &sContent, const std::string &sContentName);
        const std::vector<WsjcppParserCppLayer0Token> &getTokens();
        

    private:
        void flushBuffer();
        void throwErrorUnknownSymbol(const std::string &sMessage);
        
        std::string TAG;
        WsjcppParserCppLayer0Buffer *m_pBuffer;
        WsjcppParserCppCursor *m_pCur;
        WsjcppParserCppLayer0Status m_nStatus;
        std::vector<WsjcppParserCppLayer0Token> m_vTokens;
        std::string m_sContentName;
};

class WsjcppParserCppLayer1Token {
    public:
        WsjcppParserCppLayer1Token();
        int getLineNumber() const;
        int getPositionInLine() const;
        const std::string &getFilepath() const;
    private:
        int m_nLineNumber;
        int m_nPositionInLine;
        std::string m_sFilepath;
};

class WsjcppParserCppLayer1 {
    public:
        WsjcppParserCppLayer1();
        bool parseByTokens(const std::vector<WsjcppParserCppLayer0Token> &m_vTokens, const std::string &sContentName);
        const std::vector<WsjcppParserCppLayer1Token> &getTokens();
        

    private:
        // void flushBuffer();
        // void throwErrorUnknownSymbol(const std::string &sMessage);
        
        std::string TAG;
        // WsjcppParserCppLayer0Buffer *m_pBuffer;
        // WsjcppParserCppCursor *m_pCur;
        // WsjcppParserCppLayer0Status m_nStatus;
        std::vector<WsjcppParserCppLayer1Token> m_vTokens;
        // std::string m_sContentName;
};

class WsjcppParserCpp {
    public:
        WsjcppParserCpp();

        bool parse(const std::string &sFilepath);
        
    private:
        std::string TAG;
};

#endif // WSJCPP_PARSER_CPP_H
