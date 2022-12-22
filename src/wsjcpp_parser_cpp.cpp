#include "wsjcpp_parser_cpp.h"
#include <wsjcpp_core.h>
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------
// CppIncludeParserCursor

CppIncludeParserCursor::CppIncludeParserCursor(const std::string &sContent) {
    m_sContent = sContent;
    m_nLength = m_sContent.length();
    m_nCurrentPos = -1;
    m_c0 = 0x0;
    m_c1 = 0x0;
}

bool CppIncludeParserCursor::next() {
    m_nCurrentPos = m_nCurrentPos + 1;
    m_c0 = 0x0;
    m_c1 = 0x0;
    if (m_nCurrentPos < m_nLength) {
        m_c0 = m_sContent[m_nCurrentPos];
    }
    if (m_nCurrentPos + 1 < m_nLength) {
        m_c1 = m_sContent[m_nCurrentPos + 1];
    }
    return m_nCurrentPos < m_nLength;
}

char CppIncludeParserCursor::getC0() {
    return m_c0;
}

char CppIncludeParserCursor::getC1() {
    return m_c1;
}

bool CppIncludeParserCursor::isSeparatedChar() {
    return 
        m_c0 == ','
        || m_c0 == '#'
        || m_c0 == '.'
        || m_c0 == ':'
        || m_c0 == ';'
        || m_c0 == '{'
        || m_c0 == '}'
        || m_c0 == '>'
        || m_c0 == '<'
        || m_c0 == '('
        || m_c0 == ')'
        || m_c0 == '&'
        || m_c0 == '*'
    ;
}

bool CppIncludeParserCursor::isSkipEmptyChars() {
    return m_c0 == ' ' || m_c0 == '\n' || m_c0 == '\t';
}

bool CppIncludeParserCursor::isNumeric() {
    return m_c0 >= '0' && m_c0 <= '9';
}


// ---------------------------------------------------------------------
// CppIncludeParser

CppIncludeParser::CppIncludeParser() {
    TAG = "CppIncludeParser";
    m_nStatus = CppIncludeParserStatus::NONE;
}

bool CppIncludeParser::parseFile(const std::string &sFilepath) {
    std::string sContent;
    if (!WsjcppCore::readTextFile(sFilepath, sContent)) {
        return false;
    }

    sContent = removeComments(sContent);
    std::vector<std::string> vWords = parseByWords(sContent);

    for (int i = 0; i < vWords.size(); ++i) {
        std::cout << vWords[i] << std::endl;
    }
    return true;
}

std::string CppIncludeParser::removeComments(const std::string &sContent) {
    int nLen = sContent.length();
    std::string sNewContent = "";
    bool bMultilineComment = false;
    bool bOnelineComment = false;
    for (int i = 0; i < nLen; i++) {
        char c0 = sContent[i];
        char c1 = 0x0;
        if (i+1 < nLen) {
            c1 = sContent[i+1];
        }

        if (bMultilineComment && c0 == '*' && c1 == '/') { // end multiline comment
            bMultilineComment = false;
            i++;
            continue;
        }
        if (!bMultilineComment && c0 == '/' && c1 == '*') { // start multiline comment
            bMultilineComment = true;
            i++;
            continue;
        }

        if (bMultilineComment) {
            continue;
        }

        if (bOnelineComment && c0 == '\n') { // end oneline comment
            bOnelineComment = false;
            continue;
        }

        if (!bOnelineComment && c0 == '/' && c1 == '/') { // start oneline comment
            bOnelineComment = true;
            i++;
            continue;
        }

        if (bOnelineComment) {
            continue;
        }

        sNewContent += sContent[i];
    }
    return sNewContent;
}

std::vector<std::string> CppIncludeParser::parseByWords(const std::string &sContent) {
    CppIncludeParserCursor cur(sContent);

    int nLen = sContent.length();
    m_sBuffer = "";
    while (cur.next()) {
        char c0 = cur.getC0();
        char c1 = cur.getC1();
        bool isAlphabet = (c0 >= 'a' && c0 <= 'z') || (c0 >= 'A' && c0 <= 'Z');
        if (m_sBuffer.length() == 0) {
            if (cur.isSkipEmptyChars()) {
                continue; // skip empty lines
            }
            if (c0 == '{' || c0 == '}') {
                m_sBuffer += c0;
                flushBuffer();
                continue;
            }

            if (
                (c0 == ':' && c1 == ':')
                || (c0 == '=' && c1 == '=')
                || (c0 == '>' && c1 == '=')
                || (c0 == '<' && c1 == '=')
                || (c0 == '+' && c1 == '=')
                || (c0 == '-' && c1 == '=')
                || (c0 == '*' && c1 == '=')
            ) {
                m_sBuffer += c0;
                m_sBuffer += c1;
                cur.next(); // skip c1
                flushBuffer();
                continue;
            }

            if (
                (c0 == ':' && c1 != ':')
                || (c0 == '=' && c1 != '=')
            ) {
                m_sBuffer += c0;
                flushBuffer();
                continue;
            }

            if (isAlphabet || c0 == '_' ) { // start token
                m_sBuffer += c0;
                continue;
            }

            if (cur.isNumeric()) { // start number, TODO state
                m_sBuffer += c0;
                continue;
            }

            if (cur.isSeparatedChar()) {
                // if (c0 == '<') {
                //     std::cout << "HERE" << std::endl;
                // }
                m_sBuffer += c0;
                flushBuffer();
                continue;
            }

            if (m_nStatus == CppIncludeParserStatus::NONE && c0 == '"') {
                m_nStatus = CppIncludeParserStatus::START_PARSE_STRING;
                m_sBuffer += c0;
                continue;
            }

            throwErrorUnknownSymbol(c0, "first char");
        }

        if (m_nStatus == CppIncludeParserStatus::START_PARSE_STRING) {
            if (c0 == '\\') { // string escaping char
                m_sBuffer += c0;
                cur.next();
                m_sBuffer += cur.getC0();
                continue;
            }

            if (c0 == '"') { // end string
                m_sBuffer += c0;
                flushBuffer();
                m_nStatus = CppIncludeParserStatus::NONE;
                continue;
            }

            m_sBuffer += c0;
            continue;
        }

        if (cur.isSkipEmptyChars()) {
            flushBuffer();
            continue;
        }
        
        if (cur.isSeparatedChar()) {
            flushBuffer();
            m_sBuffer += c0;
            continue;
        }

        if (isAlphabet || cur.isNumeric() || c0 == '_' ) {
            m_sBuffer += c0;
            continue;
        }

        if (c0 == ':' && c1 != ':')  {
            flushBuffer();
            m_sBuffer += c0;
            continue;
        }

        throwErrorUnknownSymbol(c0, "non empty buffer");
    }

    if (m_sBuffer.length() > 0) {
        m_vWords.push_back(m_sBuffer);
    }
    return m_vWords;
}

void CppIncludeParser::flushBuffer() {
    if (m_sBuffer.length() > 0) {
        m_vWords.push_back(m_sBuffer);
        m_sBuffer = "";
    }
}

void CppIncludeParser::throwErrorUnknownSymbol(char c0, const std::string &sMessage) {

    std::string sError = "[" + sMessage + "] Unknown simbol '";
    sError += c0;
    sError += "'; Current buffer: '" + m_sBuffer + "';\n    Latest words:\n";
    std::vector<std::string> vLatest;
    int nMin = std::max(int(m_vWords.size() - 10), 0);
    int nMax = m_vWords.size() - 1;
    for (int i = nMin; i < nMax; i++) {
        sError += std::to_string(i) + ": '" + m_vWords[i] + "'\n";
    }
    WsjcppLog::throw_err(TAG, sError);
}
