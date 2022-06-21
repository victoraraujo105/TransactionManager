#include "Transaction.h"

#include "Action.h"
#include "Item.h"
#include "TransactionManager.h"

#include <iostream>

using namespace std;

Transaction::Transaction(TransactionManager& parent, string id):
    parent(parent), next(0), dependencies(0), id(id)
{
    parent.trans[id] = this;
}

void Transaction::proceed()
{
    if (actions.size() > next)
    {
        parent.onWait.insert(actions[next]);
    }
}    

void Transaction::forwardAndProceed()
{
    next++;
    proceed();
    // cout << "T" << id << " actions:\n";
    // for (auto& a: actions)
    // cout << a->to_string() << '\n';
}

void Transaction::addLock(Item* item)
{
    locked.emplace(item);
}

bool Transaction::isLocking(Item* item)
{
    return locked.contains(item);
}

Transaction::~Transaction()
{
    for (const auto& item: locked) item->freeLock();
    parent.trans.erase(id);
}