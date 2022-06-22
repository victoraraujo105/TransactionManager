#include <iostream>
#include <map>
#include <string>

#include "TransactionManager.h"


const map<string, ProtocolType> getPt{{"abort", ProtocolType::ABORT}, {"wait-die", ProtocolType::WAIT_DIE}, {"wound-wait", ProtocolType::WOUND_WAIT}};

int main(int argc, char* argv[])
{
    ProtocolType protocol = (argc == 1)? ProtocolType::ABORT: getPt.at(argv[1]);
    TransactionManager tm("in.txt", protocol);
}