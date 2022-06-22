#include <iostream>
#include <map>
#include <string>

#include "TransactionManager.h"


// const map<string, ProtocolType> getPt{{"abort", ProtocolType::ABORT}, {"wait-die", ProtocolType::WAIT_DIE}, {"wound-wait", ProtocolType::WOUND_WAIT}};

int main()
{
    // ProtocolType protocol = (argc == 1)? ProtocolType::ABORT: getPt.at(argv[1]);
    cout << "in1.txt:\n";
    TransactionManager tm1("in1.txt", ProtocolType::WOUND_WAIT);
    cout << '\n';
    cout << "in2.txt:\n";
    TransactionManager tm2("in2.txt", ProtocolType::WOUND_WAIT);
    cout << '\n';
    cout << "in3.txt:\n";
    TransactionManager tm3("in3.txt", ProtocolType::WOUND_WAIT);
}