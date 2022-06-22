#include "Transaction.h"

#include "Action.h"
#include "Item.h"
#include "TransactionManager.h"

#include <iostream>

using namespace std;

Transaction::Transaction(TransactionManager& parent, string id, int timestamp):
    parent(parent), next(0), id(id), rollbacked(false), timestamp(timestamp)
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
    locked.push_back(item);
}

bool Transaction::isLocking(Item* item)
{
    return locked.contains(item);
}

void Transaction::rollback()
{
    next = 0;
    rollbacked = true;
    while (!locked.empty())
    {
        locked.pop_back()->freeLock(this);
    }
    for (const auto& dep: dependents)
    {
        dep->removeDependecy(this);
        // dep->dependencies.remove(this);
    }
    dependents.clear();
}

Transaction::~Transaction()
{
    for (const auto& item: locked) item->freeLock(this);
    for (const auto& dep: dependents) dep->removeDependecy(this);
    parent.trans.erase(id);
}