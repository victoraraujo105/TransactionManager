#include <iostream>
#include <map>
#include <string>

#include "TransactionManager.h"


const map<string, ProtocolType> getPt{{"abort", ProtocolType::ABORT}, {"wait-die", ProtocolType::WAIT_DIE}, {"wound-wait", ProtocolType::WOUND_WAIT}};

int main(int argc, char* argv[])
{
    string input_path = "in.txt";
    auto protocol = ProtocolType::ABORT;
    for (int i = 1; i < argc; i++)
    {
        // cout << "argv: ." << argv[i] << ".\n";
        if (string(argv[i]) == "-f")
        {
            i++;
            input_path = argv[i];
        } else if (string(argv[i]) == "-p")
        {
            i++;
            protocol = getPt.at(argv[i]);
        }
    }
    
    TransactionManager tm(input_path, protocol);
}