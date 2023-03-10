#include <string>
#include <vector>
#include <iostream>

#include <wsjcpp_core.h>
#include "wsjcpp_parser_cpp.h"

int main(int argc, const char* argv[]) {
    std::string TAG = "MAIN";
    std::string appName = std::string(WSJCPP_APP_NAME);
    std::string appVersion = std::string(WSJCPP_APP_VERSION);
    if (!WsjcppCore::dirExists(".logs")) {
        WsjcppCore::makeDir(".logs");
    }
    WsjcppLog::setPrefixLogFile("parser");
    WsjcppLog::setLogDirectory(".logs");
    WsjcppLog::info(TAG, "Start");

    WsjcppParserCppLayer0 parser0;
    WsjcppParserCppLayer1 parser1;

    std::vector<std::string> vFiles = WsjcppCore::getListOfFiles("test-files");
    if (vFiles.size() == 0) {
        vFiles.push_back("src.wsjcpp/wsjcpp_core/wsjcpp_core.h");
    }
    for (int i = 0; i < vFiles.size(); ++i) {
        std::string sFilepath = vFiles[i];
        std::string sContent;
        if (!WsjcppCore::readTextFile(sFilepath, sContent)) {
            WsjcppLog::err(TAG, "[0] Could not read file " + sFilepath);
            return -1;
        }

        if (!parser0.parseByTokens(sContent, sFilepath)) {
            WsjcppLog::err(TAG, "[0] Could not parse file " + sFilepath);
            return -1;
        }
        const std::vector<WsjcppParserCppLayer0Token> &vTokens = parser0.getTokens();
        std::cout << "Parsed file: " << vFiles[i] << std::endl
             << "   -> Found words: " << vTokens.size() << std::endl;
        // for (int i = 0; i < vTokens.size(); ++i) {
        //     std::cout << vTokens[i].getValue() << std::endl;
        // }
        if (!parser1.parseByTokens(vTokens, sFilepath)) {
            WsjcppLog::err(TAG, "[1] Could not parse file " + sFilepath);
            return -1;
        }
    }

    return 0;
}

