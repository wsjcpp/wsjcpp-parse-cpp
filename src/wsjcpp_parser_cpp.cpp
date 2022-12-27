#include "wsjcpp_parser_cpp.h"
#include <wsjcpp_core.h>
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------
// WsjcppParserCppCursor

WsjcppParserCppCursor::WsjcppParserCppCursor(const std::string &sContent) {
    m_sContent = sContent;
    m_nLength = m_sContent.length();
    m_nCurrentPos = -1;
    m_c0 = 0x0;
    m_c1 = 0x0;
    m_nLineNumber = 0;
    m_nPositionInLine = 0;
}

bool WsjcppParserCppCursor::next() {
    m_nCurrentPos = m_nCurrentPos + 1;
    m_c0 = 0x0;
    m_c1 = 0x0;
    if (m_nCurrentPos < m_nLength) {
        m_c0 = m_sContent[m_nCurrentPos];
    }
    if (m_c0 == '\n') {
        m_nLineNumber++;
        m_nPositionInLine = 0;
    } else {
        m_nPositionInLine++;
    }
    if (m_nCurrentPos + 1 < m_nLength) {
        m_c1 = m_sContent[m_nCurrentPos + 1];
    }
    return m_nCurrentPos < m_nLength;
}

char WsjcppParserCppCursor::getC0() {
    return m_c0;
}

char WsjcppParserCppCursor::getC1() {
    return m_c1;
}

bool WsjcppParserCppCursor::isSeparatedChar() {
    return 
        m_c0 == ','
        || m_c0 == '#'
        || m_c0 == '.'
        || m_c0 == ':'
        || m_c0 == ';'
        || m_c0 == '{'
        || m_c0 == '}'
        || m_c0 == '='
        || m_c0 == '+'
        || m_c0 == '>'
        || m_c0 == ']'
        || m_c0 == '['
        || m_c0 == '<'
        || m_c0 == '('
        || m_c0 == ')'
        || m_c0 == '&'
        || m_c0 == '*'
    ;
}

bool WsjcppParserCppCursor::isSkipEmptyChars() {
    return m_c0 == ' ' || m_c0 == '\n' || m_c0 == '\t';
}

bool WsjcppParserCppCursor::isNumeric() {
    return m_c0 >= '0' && m_c0 <= '9';
}

bool WsjcppParserCppCursor::isAllowedPair() {
    return (m_c0 == ':' && m_c1 == ':')
        || (m_c0 == '=' && m_c1 == '=')
        || (m_c0 == '>' && m_c1 == '=')
        || (m_c0 == '<' && m_c1 == '=')
        || (m_c0 == '+' && m_c1 == '+')
        || (m_c0 == '-' && m_c1 == '-')
        || (m_c0 == '-' && m_c1 == '>')
        || (m_c0 == '+' && m_c1 == '=')
        || (m_c0 == '-' && m_c1 == '=')
        || (m_c0 == '*' && m_c1 == '=')
    ;
}

int WsjcppParserCppCursor::getLineNumber() {
    return m_nLineNumber;
}

int WsjcppParserCppCursor::getPositionInLine() {
    return m_nPositionInLine;
}


// ---------------------------------------------------------------------
// WsjcppParserCppLayer0Token

WsjcppParserCppLayer0Token::WsjcppParserCppLayer0Token(
    const std::string &sToken,
    int nLine,
    int nPositionInLine,
    const std::string &sFilepath
) {
    m_sToken = sToken;
    m_nLine = nLine;
    m_nPositionInLine = nPositionInLine;
    m_sFilepath = sFilepath;
}

const std::string &WsjcppParserCppLayer0Token::getValue() {
    return m_sToken;
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer0

WsjcppParserCppLayer0::WsjcppParserCppLayer0() {
    TAG = "WsjcppParserCppLayer0";
    m_nStatus = WsjcppParserCppLayer0Status::NONE;
}

bool WsjcppParserCppLayer0::parseByTokens(const std::string &sContent, const std::string &sContentName) {
    m_sContentName = sContentName;
    m_vTokens.clear();
    m_nStatus = WsjcppParserCppLayer0Status::NONE;
    m_pCur = new WsjcppParserCppCursor(sContent);

    int nLen = sContent.length();
    m_sBuffer = "";
    while (m_pCur->next()) {
        char c0 = m_pCur->getC0();
        char c1 = m_pCur->getC1();
        bool isAlphabet = (c0 >= 'a' && c0 <= 'z') || (c0 >= 'A' && c0 <= 'Z');
        if (m_sBuffer.length() == 0) {
            if (m_pCur->isSkipEmptyChars()) {
                continue; // skip empty lines
            }

            if (c0 == '/' && c1 == '/') { // start oneline comment
                m_sBuffer += c0;
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_ONELINE_COMMENT;
                continue;
            }

            if (c0 == '/' && c1 == '*') { // start multiline comment
                m_sBuffer += c0;
                m_sBuffer += c1;
                m_pCur->next(); // skip c1
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_MULTILINE_COMMENT;
                continue;
            }

            if (c0 == '\\' && c1 == '\n') { // continue line
                m_pCur->next(); // skip c1
                flushBuffer();
                continue;
            }

            if (m_pCur->isAllowedPair()) {
                flushBuffer(c0, c1);
                m_pCur->next(); // skip c1
                continue;
            }

            if (c0 == '(') {
                std::cout << "HERE" << std::endl;
            }

            if (m_pCur->isSeparatedChar()) {
                flushBuffer(c0);
                continue;
            }

            if (isAlphabet || c0 == '_' ) { // start token
                m_sBuffer += c0;
                continue;
            }

            if (m_pCur->isNumeric()) { // start number, TODO state
                m_sBuffer += c0;
                continue;
            }

            if (m_nStatus == WsjcppParserCppLayer0Status::NONE && c0 == '"') {
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_STRING;
                m_sBuffer += c0;
                continue;
            }

            throwErrorUnknownSymbol(c0, "first char");
        }

        // oneline comment
        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_ONELINE_COMMENT) {
            if (c0 == '\n') {
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
            } else {
                m_sBuffer += c0;
            }
            continue;
        }

        // multiline comment
        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_MULTILINE_COMMENT) {
            if (c0 == '*' && c1 == '/') {
                flushBuffer(c0, c1);
                m_pCur->next(); // skip c1
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
            } else {
                m_sBuffer += c0;
            }
            continue;
        }

        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_STRING) {
            if (c0 == '\\') { // string escaping char
                m_sBuffer += c0;
                m_sBuffer += c1;
                m_pCur->next();
                continue;
            }

            if (c0 == '"') { // end string
                flushBuffer(c0);
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
                continue;
            }

            m_sBuffer += c0;
            continue;
        }

        if (m_pCur->isSkipEmptyChars()) {
            flushBuffer();
            continue;
        }

        if (m_pCur->isAllowedPair()) {
            flushBuffer();
            flushBuffer(c0, c1);
            m_pCur->next(); // skip c1
            continue;
        }

        if (m_pCur->isSeparatedChar()) {
            flushBuffer();
            flushBuffer(c0);
            continue;
        }

        if (isAlphabet || m_pCur->isNumeric() || c0 == '_' ) {
            m_sBuffer += c0;
            continue;
        }

        throwErrorUnknownSymbol(c0, "non empty buffer");
    }
    
    flushBuffer();
    delete m_pCur;
    m_pCur = nullptr;
    return true;
}

const std::vector<std::string> &WsjcppParserCppLayer0::getTokens() {
    return m_vTokens;
}

void WsjcppParserCppLayer0::flushBuffer() {
    if (m_sBuffer.length() > 0) {
        WsjcppParserCppLayer0Token(
            m_sBuffer,
            m_pCur->getLineNumber(),
            m_pCur->getPositionInLine(),
            m_sContentName
        );

        m_vTokens.push_back(m_sBuffer);
        m_sBuffer = "";
    }
}

void WsjcppParserCppLayer0::flushBuffer(char c0) {
    m_sBuffer += c0;
    flushBuffer();
}

void WsjcppParserCppLayer0::flushBuffer(char c0, char c1) {
    m_sBuffer += c0;
    m_sBuffer += c1;
    flushBuffer();
}

void WsjcppParserCppLayer0::throwErrorUnknownSymbol(char c0, const std::string &sMessage) {

    std::string sError = "[" + sMessage + "] Unknown simbol '";
    sError += c0;
    sError += "'; Current buffer: '" + m_sBuffer + "';\n    Latest words:\n";
    std::vector<std::string> vLatest;
    int nMin = std::max(int(m_vTokens.size() - 10), 0);
    int nMax = m_vTokens.size() - 1;
    for (int i = nMin; i < nMax; i++) {
        sError += std::to_string(i) + ": '" + m_vTokens[i] + "'\n";
    }
    WsjcppLog::throw_err(TAG, sError);
}


// ---------------------------------------------------------------------
// WsjcppParserCpp

WsjcppParserCpp::WsjcppParserCpp() {
    TAG = "WsjcppParserCpp";
}

bool WsjcppParserCpp::parse(const std::string &sFilepath) {
    std::string sContent;
    if (!WsjcppCore::readTextFile(sFilepath, sContent)) {
        return false;
    }
    WsjcppParserCppLayer0 layer0;
    layer0.parseByTokens(sContent, sFilepath);
    
    // parseByWords(sContent);
    return true;
}