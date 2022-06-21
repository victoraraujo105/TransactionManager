#include "Action.h"
#include "Transaction.h"

#include "Item.h"

#include <iostream>
#include <string>

Action::Action(Transaction* const parent, ActionType type, int timestamp):
    parent(parent), type(type), timestamp(timestamp), parentId(parent->id)
{
    parent->actions.push_back(this);
    if (parent->next == parent->actions.size() - 1) parent->proceed();
}

bool Action::done()
{
    return parent->next == parent->actions.size() || *this < *parent->actions[parent->next];
}

AccessAction::AccessAction(Transaction* const parent, Item& item, ActionType type, int timestamp):
    Action(parent, type, timestamp), item(item), itemId(item.id)
{}

void ReadAction::run()
{
    // cout << "r" << parentId << "(" << itemId << ")\n";
    if (item.grantSharedLock(parent))
    {
        parent->forwardAndProceed();
    }
}

void WriteAction::run()
{
    // cout << "w" << parentId << "(" << itemId << ")\n";
    if (item.grantExclusiveLock(parent))
    {
        parent->forwardAndProceed();
    }
}

void CommitAction::run()
{
    // cout << "C(" << parentId << ")\n";
    delete parent;
}