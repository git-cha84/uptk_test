#include "Client.h"
#include "Log.h"
#include "Server.h"
#include "config.h"

int workTime = 0;
int maxClient = 0;

void parsingCmdLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-workTime") == 0) {
            workTime = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-maxClient") == 0) {
            maxClient = atoi(argv[i + 1]);
        }
    }
}

int main(int argc, char* argv[]) {
    debugInit((char*)DEF_LOG_FILE);
    debugLog("Begin program:");

    parsingCmdLine(argc, argv);

    if (!workTime)
        workTime = GlobalConf::DEFAULT_WORK_TIME;

    if (!maxClient)
        maxClient = GlobalConf::MAX_CLIENTS;

    auto server = std::make_shared<Server>();
    Client client;
    client.setServer(server);
    client.setWorkTime(workTime);
    client.runPoolThreads(maxClient);

    return 0;
}
