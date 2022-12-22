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

    WsjcppParserCpp parser;

    std::vector<std::string> vFiles;
    vFiles.push_back("src.wsjcpp/wsjcpp_core/wsjcpp_core.h");
    for (int i = 0; i < vFiles.size(); ++i) {
        if (!parser.parseFile(vFiles[i])) {
            WsjcppLog::err(TAG, "Could not parse file " + vFiles[i]);
            return -1;
        }
        const std::vector<std::string> &vWords = parser.getWords();
        std::cout << "Parsed file: " << vFiles[i] << std::endl
             << "   -> Found words: " << vWords.size() << std::endl;
        // for (int i = 0; i < vWords.size(); ++i) {
        //     std::cout << vWords[i] << std::endl;
        // }
    }

    return 0;
}

