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
    m_nLineNumber = pToken->getLineNumber();
    m_nPositionInLine = pToken->getPositionInLine();
    m_sFilepath = pToken->getFilepath();
}

const std::string &WsjcppParserCppLayer0Token::getValue() const {
    return m_sToken;
}

int WsjcppParserCppLayer0Token::getLineNumber() const {
    return m_nLineNumber;
}

int WsjcppParserCppLayer0Token::getPositionInLine() const {
    return m_nPositionInLine;
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
// WsjcppParserCppLayer1Buffer

WsjcppParserCppLayer1Buffer::WsjcppParserCppLayer1Buffer(const std::string &sFilepath) {
    reset();
    m_sFilepath = sFilepath;
}

void WsjcppParserCppLayer1Buffer::reset() {
    m_sBuffer = "";
    m_nLineNumber = -1;
    m_nPositionInLine = -1;
    m_vBuffer.clear();
}

void WsjcppParserCppLayer1Buffer::append(WsjcppParserCppLayer0Token token) {
    if (m_vBuffer.size() == 0) {
        m_sBuffer = token.getValue();
        m_nLineNumber = token.getLineNumber();
        m_nPositionInLine = token.getPositionInLine();
        m_vBuffer.push_back(token);
        return;
    }
    m_vBuffer.push_back(token);
    m_sBuffer += token.getValue();
}

bool WsjcppParserCppLayer1Buffer::isEmpty() const {
    return m_vBuffer.size() == 0;
}

const std::string &WsjcppParserCppLayer1Buffer::getValue() const {
    return m_sBuffer;
}

int WsjcppParserCppLayer1Buffer::getLineNumber() const {
    return m_nLineNumber;
}

int WsjcppParserCppLayer1Buffer::getPositionInLine() const {
    return m_nPositionInLine;
}

const std::string &WsjcppParserCppLayer1Buffer::getFilepath() const {
    return m_sFilepath;
}

// ---------------------------------------------------------------------
// WsjcppParserCppLayer1Token

WsjcppParserCppLayer1Token::WsjcppParserCppLayer1Token(WsjcppParserCppLayer1TokenType nType) {
    m_nType = nType;
}

WsjcppParserCppLayer1Token::WsjcppParserCppLayer1Token(
    WsjcppParserCppLayer1TokenType nType,
    const std::string &sValue
) {
    m_nType = nType;
    m_sValue = sValue;
}


WsjcppParserCppLayer1TokenType WsjcppParserCppLayer1Token::getType() const {
    return m_nType;
}

const std::string &WsjcppParserCppLayer1Token::getValue() const {
    return m_sValue;
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
    m_pBuffer = nullptr;
}
        
bool WsjcppParserCppLayer1::parseByTokens(const std::vector<WsjcppParserCppLayer0Token> &vTokens, const std::string &sContentName) {
    int i = 0;
    int nLength = vTokens.size();
    m_pBuffer = new WsjcppParserCppLayer1Buffer(sContentName);
    auto nType = WsjcppParserCppLayer1TokenType::NONE;

    while (i < nLength) {
        WsjcppParserCppLayer0Token token0 = vTokens[i];
        std::string sToken0 = token0.getValue();
        std::string sToken1 = "";
        if (i+1 < nLength) {
            sToken1 = vTokens[i+1].getValue();
        }
        std::string sToken2 = "";
        if (i+2 < nLength) {
            sToken2 = vTokens[i+2].getValue();
        }

        // include defined
        if (sToken0 == "#" && sToken1 == "include" && nType == WsjcppParserCppLayer1TokenType::NONE) {
            nType = WsjcppParserCppLayer1TokenType::INCLUDE;
            i += 2;
            continue;
        }

        if (WsjcppCore::startsWith(sToken0, "\"") && nType == WsjcppParserCppLayer1TokenType::INCLUDE) {
            m_vTokens.push_back(WsjcppParserCppLayer1Token(
                nType,
                sToken0
            ));
            i++;
            nType = WsjcppParserCppLayer1TokenType::NONE;
            continue;
        }

        if (sToken0 == "<" && nType == WsjcppParserCppLayer1TokenType::INCLUDE) {
            m_vTokens.push_back(WsjcppParserCppLayer1Token(
                nType,
                sToken0 + sToken1 + vTokens[i+2].getValue() // TODO check
            ));
            i += 2;
            nType = WsjcppParserCppLayer1TokenType::NONE;
            continue;
        }

        // oneline comment
        if (WsjcppCore::startsWith(sToken0, "//") && nType == WsjcppParserCppLayer1TokenType::NONE) {
            m_vTokens.push_back(WsjcppParserCppLayer1Token(
                WsjcppParserCppLayer1TokenType::COMMENT,
                sToken0
            ));
            i++;
            continue;
        }

        // begin function
        if (isStartFunction(vTokens, i) && nType == WsjcppParserCppLayer1TokenType::NONE) {
            std::cout << "Found" << std::endl;
            m_vTokens.push_back(WsjcppParserCppLayer1Token(
                WsjcppParserCppLayer1TokenType::RETURN_TYPE,
                sToken0
            ));
            i++;
            m_vTokens.push_back(WsjcppParserCppLayer1Token(
                WsjcppParserCppLayer1TokenType::FUNCTION_NAME,
                sToken1
            ));
            i += 2;
            continue;
        }

        // TODO error
        i++;
    }
    // TODO
    return false;
}

const std::vector<WsjcppParserCppLayer1Token> &WsjcppParserCppLayer1::getTokens() {
    return m_vTokens;
}

bool WsjcppParserCppLayer1::isFuncNameOrType(const std::string &sToken) {
    if (sToken.length() == 0) {
        return false;
    }
    char c0 = sToken[0];
    if (
        !(c0 > 'a' && c0 < 'z')
        && !(c0 > 'A' && c0 < 'Z')
        && c0 != '_'
    ) {
        return false;
    }
    for (int i = 1; i < sToken.length(); i++) {
        c0 = sToken[i];
        if (
            !(c0 > 'a' && c0 < 'z')
            && !(c0 > 'A' && c0 < 'Z')
            && !(c0 > '0' && c0 < '9')
            && c0 != '_'
        ) {
            return false;
        }
    }
    std::cout << "Can be function name: " << sToken << std::endl;
    return true;
}

bool WsjcppParserCppLayer1::isStartFunction(const std::vector<WsjcppParserCppLayer0Token> &vTokens, int nStartPos) {
    std::string sToken0 = "";
    std::string sToken1 = "";
    std::string sToken2 = "";

    if (nStartPos < vTokens.size()) {
        sToken0 = vTokens[nStartPos].getValue();
    }
    if (nStartPos+1 < vTokens.size()) {
        sToken1 = vTokens[nStartPos+1].getValue();
    }
    if (nStartPos+2 < vTokens.size()) {
        sToken2 = vTokens[nStartPos+2].getValue();
    }
    
    if (isFuncNameOrType(sToken0) && isFuncNameOrType(sToken1) && sToken2 == "(") {
        return true;
    }
    return false;
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


