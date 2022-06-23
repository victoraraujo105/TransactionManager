#include "Action.h"
#include "Transaction.h"

#include "Item.h"

#include <iostream>
#include <string>
#include "SetList.h"

using namespace std;

Action::Action(Transaction* const parent, ActionType type, int timestamp):
    parent(parent), type(type), timestamp(timestamp), parentId(parent->id), state(ActionState::STASHED)
{
    parent->actions.push_back(this);
    if (parent->next == parent->actions.size() - 1) parent->proceed();
}

void Action::addDependencies(SetList<Transaction*> dependencies)
{
    for (const auto& dep: dependencies) parent->dependencies.push_back(dep);
}


AccessAction::AccessAction(Transaction* const parent, Item& item, ActionType type, int timestamp):
    Action(parent, type, timestamp), item(item), itemId(item.id)
{}

void ReadAction::run()
{
    // cout << "r" << parentId << "(" << itemId << ")\n";
    state = item.grantSharedLock(this);
    if (state == ActionState::DONE) {
        cout << to_string() << " -- done\n";
        parent->forwardAndProceed();
    } else if (state == ActionState::STASHED) {
        cout << to_string() << " -- stashed\n";
        item.printLocked();
    } else if (state == ActionState::ROLLBACKED) {
        cout << to_string() << " -- rollbacked\n";
        item.printLocked();
    } else if (state == ActionState::DEADLOCKED) {
        parent->rollback();
    }
}

void WriteAction::run()
{
    // cout << "w" << parentId << "(" << itemId << ")\n";
    state = item.grantExclusiveLock(this);
    if (state == ActionState::DONE) {
        cout << to_string() << " -- done\n";
        parent->forwardAndProceed();
    } else if (state == ActionState::STASHED) {
        cout << to_string() << " -- stashed\n";
        item.printLocked();
    } else if (state == ActionState::ROLLBACKED) {
        cout << to_string() << " -- rollbacked\n";;
        item.printLocked();
    } else if (state == ActionState::DEADLOCKED) {
        parent->rollback();
    }
}

void CommitAction::run()
{
    // cout << "C(" << parentId << ")\n";
    state = ActionState::DONE;
    cout << to_string() << " -- done\n";
    delete parent;
}