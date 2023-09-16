#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "httplib.h"

#include "cOSMOverpass.h"

void cOSMOverpass::doQuery()
{

    // https://github.com/yhirose/cpp-httplib

    httplib::Client cli(myHost);

    auto res = cli.Get(myQuery);

    std::cout << res->status << "\n";

    // myStatus = res->status;

    myDownload = res->body;
}
