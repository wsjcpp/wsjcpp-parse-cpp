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

WsjcppParserCppLayer0Buffer::WsjcppParserCppLayer0Buffer() {
    reset();
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

bool WsjcppParserCppLayer0Buffer::isEmpty() {
    return m_nLength == 0;
}

int WsjcppParserCppLayer0Buffer::getLineNumber() {
    return m_nLineNumber;
};

int WsjcppParserCppLayer0Buffer::getPositionInLine() {
    return m_nPositionInLine;
};

const std::string &WsjcppParserCppLayer0Buffer::getValue() const {
    return m_sBuffer;
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

const std::string &WsjcppParserCppLayer0Token::getValue() const {
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
    m_buffer.reset();
    m_nStatus = WsjcppParserCppLayer0Status::NONE;
    m_pCur = new WsjcppParserCppCursor(sContent);

    while (m_pCur->next()) {
        char c0 = m_pCur->getC0();
        char c1 = m_pCur->getC1();
        bool isAlphabet = (c0 >= 'a' && c0 <= 'z') || (c0 >= 'A' && c0 <= 'Z');
        if (m_buffer.isEmpty()) {
            if (m_pCur->isSkipEmptyChars()) {
                continue; // skip empty lines
            }

            if (c0 == '/' && c1 == '/') { // start oneline comment
                m_buffer.append(m_pCur);
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_ONELINE_COMMENT;
                continue;
            }

            if (c0 == '/' && c1 == '*') { // start multiline comment
                m_buffer.append(m_pCur);
                m_pCur->next(); // skip c1
                m_buffer.append(m_pCur);
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_MULTILINE_COMMENT;
                continue;
            }

            if (c0 == '\\' && c1 == '\n') { // continue line
                m_pCur->next(); // skip c1
                flushBuffer();
                continue;
            }

            if (m_pCur->isAllowedPair()) {
                m_buffer.append(m_pCur);
                m_pCur->next(); // skip c1
                m_buffer.append(m_pCur);
                flushBuffer();
                continue;
            }

            if (m_pCur->isSeparatedChar()) {
                m_buffer.append(m_pCur);
                flushBuffer();
                continue;
            }

            if (isAlphabet || c0 == '_' ) { // start token
                m_buffer.append(m_pCur);
                continue;
            }

            if (m_pCur->isNumeric()) { // start number, TODO state
                m_buffer.append(m_pCur);
                continue;
            }

            if (m_nStatus == WsjcppParserCppLayer0Status::NONE && c0 == '"') {
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_STRING_DOUBLE_QUOTES;
                m_buffer.append(m_pCur);
                continue;
            }

            if (m_nStatus == WsjcppParserCppLayer0Status::NONE && c0 == '\'') {
                m_nStatus = WsjcppParserCppLayer0Status::START_PARSE_STRING_SINGLE_QUOTES;
                m_buffer.append(m_pCur);
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
                m_buffer.append(m_pCur);
            }
            continue;
        }

        // multiline comment
        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_MULTILINE_COMMENT) {
            if (c0 == '*' && c1 == '/') {
                m_buffer.append(m_pCur);
                m_pCur->next(); // skip c1
                m_buffer.append(m_pCur);
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
            } else {
                m_buffer.append(m_pCur);
            }
            continue;
        }

        if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_STRING_DOUBLE_QUOTES) {
            if (c0 == '\\') { // string escaping char
                m_buffer.append(m_pCur);
                m_pCur->next();
                m_buffer.append(m_pCur);
                continue;
            }

            if (c0 == '"') { // end string
                m_buffer.append(m_pCur);
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
                continue;
            }

            m_buffer.append(m_pCur);
            continue;
        }

         if (m_nStatus == WsjcppParserCppLayer0Status::START_PARSE_STRING_SINGLE_QUOTES) {
            if (c0 == '\\') { // string escaping char
                m_buffer.append(m_pCur);
                m_pCur->next();
                m_buffer.append(m_pCur);
                continue;
            }

            if (c0 == '\'') { // end string
                m_buffer.append(m_pCur);
                flushBuffer();
                m_nStatus = WsjcppParserCppLayer0Status::NONE;
                continue;
            }

            m_buffer.append(m_pCur);
            continue;
        }

        if (m_pCur->isSkipEmptyChars()) {
            flushBuffer();
            continue;
        }

        if (m_pCur->isAllowedPair()) {
            flushBuffer();
            m_buffer.append(m_pCur);
            m_pCur->next(); // skip c1
            m_buffer.append(m_pCur);
            flushBuffer();
            continue;
        }

        if (m_pCur->isSeparatedChar()) {
            flushBuffer();
            m_buffer.append(m_pCur);
            flushBuffer();
            continue;
        }

        if (isAlphabet || m_pCur->isNumeric() || c0 == '_' ) {
            m_buffer.append(m_pCur);
            continue;
        }

        throwErrorUnknownSymbol("non empty buffer");
    }
    
    flushBuffer();
    delete m_pCur;
    m_pCur = nullptr;
    return true;
}

const std::vector<WsjcppParserCppLayer0Token> &WsjcppParserCppLayer0::getTokens() {
    return m_vTokens;
}

void WsjcppParserCppLayer0::flushBuffer() {
    if (!m_buffer.isEmpty()) {
        m_vTokens.push_back(WsjcppParserCppLayer0Token(
            m_buffer.getValue(),
            m_buffer.getLineNumber(),
            m_buffer.getPositionInLine(),
            m_sContentName
        ));
        m_buffer.reset();
    }
}

void WsjcppParserCppLayer0::throwErrorUnknownSymbol(const std::string &sMessage) {

    std::string sError = "[" + sMessage + "] Unknown simbol (" + std::to_string(m_pCur->getC0()) + ") '";
    sError += m_pCur->getC0();
    sError += "'; File: " + m_sContentName + ":" + std::to_string(m_pCur->getLineNumber()) + ":" + std::to_string(m_pCur->getPositionInLine());
    sError += "; Current buffer: '" + m_buffer.getValue() + "';\n    Latest words:\n";
    std::vector<std::string> vLatest;
    int nMin = std::max(int(m_vTokens.size() - 10), 0);
    int nMax = m_vTokens.size() - 1;
    for (int i = nMin; i < nMax; i++) {
        sError += std::to_string(i) + ": '" + m_vTokens[i].getValue() + "'\n";
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