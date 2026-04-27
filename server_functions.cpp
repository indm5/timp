#include "server_functions.h"
#include <map>

QString parsing(QString str) {
    QString cleanStr = str;
    cleanStr.remove('\r');
    cleanStr.remove('\n');
    cleanStr = cleanStr.trimmed();
    cleanStr = cleanStr.toLower();

    std::map<QString, int> dict = {
        {"exit", 1},
        {"registration", 2}, {"reg", 2},
        {"authorization", 3}, {"auth", 3}, {"unlog", 4},
        {"un", 4}, {"check", 5}, {"ch", 5},
        {"information", 6}, {"info", 6}
    };

    int num = 0;
    if (dict.find(cleanStr) != dict.end()) {
        num = dict[cleanStr];
    }

    switch (num) {
    case 1:
        return "Connection will be closed!\r\n";
    case 2:
        return "Registration completed!\r\n";
    case 3:
        return "Authorization completed!\r\n";
    case 4:
        return "Unlogging completed!\r\n";
    case 5:
        return "Check command received!\r\n";
    case 6:
        return "Information:\r\nCommands: registration, authorization, check, info, exit\r\n";
    default:
        return "Unknown command. Type 'info'\r\n";
    }
}
