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

    WsjcppParserCppLayer0 parser;

    std::vector<std::string> vFiles = WsjcppCore::getListOfFiles("test-files");
    if (vFiles.size() == 0) {
        vFiles.push_back("src.wsjcpp/wsjcpp_core/wsjcpp_core.h");
    }
    for (int i = 0; i < vFiles.size(); ++i) {
        std::string sFilepath = vFiles[i];
        std::string sContent;
        if (!WsjcppCore::readTextFile(sFilepath, sContent)) {
            WsjcppLog::err(TAG, "Could not read file " + sFilepath);
            return -1;
        }

        if (!parser.parseByTokens(sContent)) {
            WsjcppLog::err(TAG, "Could not parse file " + sFilepath);
            return -1;
        }
        const std::vector<std::string> &vWords = parser.getTokens();
        std::cout << "Parsed file: " << vFiles[i] << std::endl
             << "   -> Found words: " << vWords.size() << std::endl;
        // for (int i = 0; i < vWords.size(); ++i) {
        //     std::cout << vWords[i] << std::endl;
        // }
    }

    return 0;
}

