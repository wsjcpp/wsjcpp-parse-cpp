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
    m_nLineNumber = 1;
    m_nPositionInLine = 1;
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
        || m_c0 == '?' // short if
        || m_c0 == '/' // mut
        || m_c0 == '~'
        || m_c0 == '-'
        || m_c0 == '!'
        || m_c0 == '|'
        || m_c0 == '^'
        || m_c0 == '%'
    ;
}

bool WsjcppParserCppCursor::isSkipEmptyChars() {
    return m_c0 == ' ' || m_c0 == '\n' || m_c0 == '\r' || m_c0 == '\t';
}

bool WsjcppParserCppCursor::isNumeric() {
    return m_c0 >= '0' && m_c0 <= '9';
}

bool WsjcppParserCppCursor::isAllowedPair() {
    return (m_c0 == ':' && m_c1 == ':')
        || (m_c0 == '=' && m_c1 == '=')
        || (m_c0 == '!' && m_c1 == '=')
        || (m_c0 == '+' && m_c1 == '=')
        || (m_c0 == '/' && m_c1 == '=')
        || (m_c0 == '*' && m_c1 == '=')
        || (m_c0 == '>' && m_c1 == '=')
        || (m_c0 == '<' && m_c1 == '=')
        || (m_c0 == '+' && m_c1 == '+')
        || (m_c0 == '-' && m_c1 == '-')
        || (m_c0 == '-' && m_c1 == '>')
        || (m_c0 == '+' && m_c1 == '=')
        || (m_c0 == '-' && m_c1 == '=')
        || (m_c0 == '*' && m_c1 == '=')
        || (m_c0 == '&' && m_c1 == '&')
        || (m_c0 == '|' && m_c1 == '|')
    ;
}

int WsjcppParserCppCursor::getLineNumber() {
    return m_nLineNumber;
}

int WsjcppParserCppCursor::getPositionInLine() {
    return m_nPositionInLine;
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer0Buffer

WsjcppParserCppLayer0Buffer::WsjcppParserCppLayer0Buffer(const std::string &sFilepath) {
    reset();
    m_sFilepath = sFilepath;
}

void WsjcppParserCppLayer0Buffer::reset() {
    m_sBuffer = "";
    m_nLength = 0;
    m_nLineNumber = -1;
    m_nPositionInLine = -1;
}

void WsjcppParserCppLayer0Buffer::append(WsjcppParserCppCursor *pCur) {
    m_sBuffer += pCur->getC0();
    if (m_nLength == 0) {
        m_nLineNumber = pCur->getLineNumber();
        m_nPositionInLine = pCur->getPositionInLine();
    }
    m_nLength++;
}

const std::string &WsjcppParserCppLayer0Buffer::getValue() const {
    return m_sBuffer;
}

bool WsjcppParserCppLayer0Buffer::isEmpty() const {
    return m_nLength == 0;
}

int WsjcppParserCppLayer0Buffer::getLineNumber() const {
    return m_nLineNumber;
};

int WsjcppParserCppLayer0Buffer::getPositionInLine() const {
    return m_nPositionInLine;
};

const std::string &WsjcppParserCppLayer0Buffer::getFilepath() const {
    return m_sFilepath;
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer0Token

WsjcppParserCppLayer0Token::WsjcppParserCppLayer0Token(
    const WsjcppParserCppLayer0Buffer *pToken
) {
    m_sToken = pToken->getValue();
    m_nLine = pToken->getLineNumber();
    m_nPositionInLine = pToken->getPositionInLine();
    m_sFilepath = pToken->getFilepath();
}

const std::string &WsjcppParserCppLayer0Token::getValue() const {
    return m_sToken;
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer0

WsjcppParserCppLayer0::WsjcppParserCppLayer0() {
    TAG = "WsjcppParserCppLayer0";
    m_nStatus = WsjcppParserCppLayer0Status::NONE;
    m_pBuffer = nullptr;
}

bool WsjcppParserCppLayer0::parseByTokens(const std::string &sContent, const std::string &sContentName) {
    m_sContentName = sContentName;
    m_vTokens.clear();
    m_pBuffer = new WsjcppParserCppLayer0Buffer(sContentName);
    m_pBuffer->reset();
    m_nStatus = WsjcppParserCppLayer0Status::NONE;
    m_pCur = new WsjcppParserCppCursor(sContent);

    while (m_pCur->next()) {
        char c0 = m_pCur->getC0();
        char c1 = m_pCur->getC1();
        bool isAlphabet = (c0 >= 'a' && c0 <= 'z') || (c0 >= 'A' && c0 <= 'Z');
        if (m_pBuffer->isEmpty()) {
            if (m_pCur->isSkipEmptyChars()) {
                continue; // skip empty lines
            }

            if (c0 == '/' && c1 == '/') { // start oneline comment
                m_pBuffer->append(m_pCur);
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_ONELINE_COMMENT;
                continue;
            }

            if (c0 == '/' && c1 == '*') { // start multiline comment
                m_pBuffer->append(m_pCur);
                m_pCur->next(); // skip c1
                m_pBuffer->append(m_pCur);
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_MULTILINE_COMMENT;
                continue;
            }

            if (c0 == '\\' && c1 == '\n') { // continue line
                m_pCur->next(); // skip c1
                flushBuffer();
                continue;
            }

            if (m_pCur->isAllowedPair()) {
                m_pBuffer->append(m_pCur);
                m_pCur->next(); // skip c1
                m_pBuffer->append(m_pCur);
                flushBuffer();
                continue;
            }

            if (m_pCur->isSeparatedChar()) {
                m_pBuffer->append(m_pCur);
                flushBuffer();
                continue;
            }

            if (isAlphabet || c0 == '_' ) { // start token
                m_pBuffer->append(m_pCur);
                continue;
            }

            if (m_pCur->isNumeric()) { // start number, TODO state
                m_pBuffer->append(m_pCur);
                continue;
            }

            if (m_nStatus == WsjcppParserCppLayer0Status::NONE && c0 == '"') {
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_STRING_DOUBLE_QUOTES;
                m_pBuffer->append(m_pCur);
                continue;
            }

            if (m_nStatus == WsjcppParserCppLayer0Status::NONE && c0 == '\'') {
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_STRING_SINGLE_QUOTES;
                m_pBuffer->append(m_pCur);
                continue;
            }

            throwErrorUnknownSymbol("first char");
        }

        // oneline comment
        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_ONELINE_COMMENT) {
            if (c0 == '\n') {
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
            } else {
                m_pBuffer->append(m_pCur);
            }
            continue;
        }

        // multiline comment
        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_MULTILINE_COMMENT) {
            if (c0 == '*' && c1 == '/') {
                m_pBuffer->append(m_pCur);
                m_pCur->next(); // skip c1
                m_pBuffer->append(m_pCur);
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
            } else {
                m_pBuffer->append(m_pCur);
            }
            continue;
        }

        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_STRING_DOUBLE_QUOTES) {
            if (c0 == '\\') { // string escaping char
                m_pBuffer->append(m_pCur);
                m_pCur->next();
                m_pBuffer->append(m_pCur);
                continue;
            }

            if (c0 == '"') { // end string
                m_pBuffer->append(m_pCur);
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
                continue;
            }

            m_pBuffer->append(m_pCur);
            continue;
        }

         if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_STRING_SINGLE_QUOTES) {
            if (c0 == '\\') { // string escaping char
                m_pBuffer->append(m_pCur);
                m_pCur->next();
                m_pBuffer->append(m_pCur);
                continue;
            }

            if (c0 == '\'') { // end string
                m_pBuffer->append(m_pCur);
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
                continue;
            }

            m_pBuffer->append(m_pCur);
            continue;
        }

        if (m_pCur->isSkipEmptyChars()) {
            flushBuffer();
            continue;
        }

        if (m_pCur->isAllowedPair()) {
            flushBuffer();
            m_pBuffer->append(m_pCur);
            m_pCur->next(); // skip c1
            m_pBuffer->append(m_pCur);
            flushBuffer();
            continue;
        }

        if (m_pCur->isSeparatedChar()) {
            flushBuffer();
            m_pBuffer->append(m_pCur);
            flushBuffer();
            continue;
        }

        if (isAlphabet || m_pCur->isNumeric() || c0 == '_' ) {
            m_pBuffer->append(m_pCur);
            continue;
        }

        throwErrorUnknownSymbol("non empty buffer");
    }
    
    flushBuffer();
    delete m_pCur;
    m_pCur = nullptr;
    delete m_pBuffer;
    m_pBuffer = nullptr;
    return true;
}

const std::vector<WsjcppParserCppLayer0Token> &WsjcppParserCppLayer0::getTokens() {
    return m_vTokens;
}

void WsjcppParserCppLayer0::flushBuffer() {
    if (!m_pBuffer->isEmpty()) {
        m_vTokens.push_back(WsjcppParserCppLayer0Token(m_pBuffer));
        m_pBuffer->reset();
    }
}

void WsjcppParserCppLayer0::throwErrorUnknownSymbol(const std::string &sMessage) {

    std::string sError = "[" + sMessage + "] Unknown simbol (" + std::to_string(m_pCur->getC0()) + ") '";
    sError += m_pCur->getC0();
    sError += "'; File: " + m_sContentName + ":" + std::to_string(m_pCur->getLineNumber()) + ":" + std::to_string(m_pCur->getPositionInLine());
    sError += "; Current buffer: '" + m_pBuffer->getValue() + "';\n    Latest words:\n";
    std::vector<std::string> vLatest;
    int nMin = std::max(int(m_vTokens.size() - 10), 0);
    int nMax = m_vTokens.size() - 1;
    for (int i = nMin; i < nMax; i++) {
        sError += std::to_string(i) + ": '" + m_vTokens[i].getValue() + "'\n";
    }
    WsjcppLog::throw_err(TAG, sError);
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer1Token

WsjcppParserCppLayer1Token::WsjcppParserCppLayer1Token() {

}

int WsjcppParserCppLayer1Token::getLineNumber() const {
    return m_nLineNumber;
}

int WsjcppParserCppLayer1Token::getPositionInLine() const {
    return m_nPositionInLine;
}

const std::string &WsjcppParserCppLayer1Token::getFilepath() const {
    return m_sFilepath;
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer1

WsjcppParserCppLayer1::WsjcppParserCppLayer1() {
    TAG = "WsjcppParserCppLayer1";
}
        
bool WsjcppParserCppLayer1::parseByTokens(const std::vector<WsjcppParserCppLayer0Token> &m_vTokens, const std::string &sContentName) {
    // TODO
    return false;
}

const std::vector<WsjcppParserCppLayer1Token> &WsjcppParserCppLayer1::getTokens() {
    return m_vTokens;
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


