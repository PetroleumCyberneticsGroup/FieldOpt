#include "posixutilities.h"

PosixUtilities::PosixUtilities()
{
}

bool PosixUtilities::executeCommand(QString cmd, QString workdir)
{
    QString combined = "cd " + workdir + "; " + cmd;
    FILE* pipe = popen(combined.toStdString().c_str(), "r");
    if (!pipe) {
        return false;
    }
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return true;
}
