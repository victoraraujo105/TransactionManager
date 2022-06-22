#include "Item.h"
#include "Action.h"
#include "Transaction.h"
#include "TransactionManager.h"

#include <iostream>
#include <map>
#include <string>

using namespace std;

LockType operator * (const LockType& a, const LockType& b)
{
    auto k = static_cast<int>(a)*static_cast<int>(b);
    switch (k)
    {
    case 2:
    case 4:
        return LockType::SHARED;
    case 3:
        return LockType::EXCLUSIVE;
    default:
        return LockType::NONE;
    }
}

const map<LockType, string> lockName{{LockType::NONE, "Livre"},{LockType::SHARED, "Compartilhado"}, {LockType::EXCLUSIVE, "Exclusivo"}};

bool Item::deadlocked(Transaction* a)
{
    for (const auto& b: lockers)
    {
        if (b->dependencies.contains(a))
            return true;
        // {
        //     cout << "Deadlock!\n";
        //     cout << "Detectada dependencia mutua entre " << a << " e " << b << '\n';
        //     exit(EXIT_FAILURE);
        // }

    }

    return false;
}

void Item::freeLock(Transaction* txn)
{
    lockers.remove(txn);
    if (lockers.size() == 1 && lockers.contains(peekNext().first))
    {
        lock = nextLockType();
        return popNext();
    }
    if (lockers.size() > 0) return;
    // cout << "Item " << id << "'s queue:\n";
    // for (auto p: queue)
    // {
    //     cout << "T" << p.first->id << ", " << (int) p.second << '\n';
    // }
    if (queue.size() > 0)
    {
        lock = nextLockType();
        do {
            popNext();

        } while (queue.size() > 0 && lock*nextLockType() == LockType::SHARED);
        return;
    } 
    lock = LockType::NONE;
}

void Item::popNext()
{
    const auto& pair = queue.front();
    const auto& txn = pair.first;
    queue.remove(pair);
    lockers.push_back(txn);
    txn->addLock(this);
    txn->proceed();
}


ActionState Item::grantSharedLock(AccessAction* a)
{
    const auto& txn = a->parent;
    if (txn->isLocking(this)) return ActionState::DONE;

    switch (lock)
    {
    case LockType::NONE:
        lock = LockType::SHARED;
    case LockType::SHARED:
        if (queue.size() == 0)
        {
            txn->addLock(this);
            lockers.push_back(txn);
            return ActionState::DONE;
        }
    case LockType::EXCLUSIVE:
        if (queue.size() == 1 && queue.front().first == txn) return ActionState::DONE;
        txn->addDependencies(lockers);
        {
            ActionState state = txn->parent.protocol(a, lockers);
            if (state == ActionState::DONE) return grantSharedLock(a);
            if (state != ActionState::STASHED) return state;
        }
        queue.push_back(pair(txn, LockType::SHARED));
    default:
        break;
    }
    return ActionState::STASHED;
}

ActionState Item::grantExclusiveLock(AccessAction* a)
{
    const auto& txn = a->parent;
    // cout << "incidence: " << incidence << '\n';
    // cout << "ExclusiveLock on item " << id << " requested by T" << txn->id << ".\n";
    if (lockers.size() == 0 || (lockers.size() == 1 && txn->isLocking(this)))
    {
        lock = LockType::EXCLUSIVE;
        txn->addLock(this);
        lockers.push_back(txn);
        // cout << "granted!\n";
        return ActionState::DONE;
    }
    ActionState state = txn->parent.protocol(a, lockers);
    txn->addDependencies(lockers);
    if (state != ActionState::STASHED) return state;
    queue.push_back(pair(txn, LockType::EXCLUSIVE));
    // cout << "not granted!\n";
    return ActionState::STASHED;
}

void Item::printQueue()
{
    bool first = true;
    cout << "Lista de espera de " << id << ":\n\t";
    for (const auto& pair: queue)
    {
        if (first) first = false;
        else cout << ", ";
        cout << "{"<< pair.first << ", " << lockName.at(pair.second) << "}";
    }
    cout << '\n';
}

void Item::printLocked()
{
    bool first = true;
    cout << "Bloqueios " << lockName.at(lock) << "s de " << id << ":\n\t";
    for (const auto& txn: lockers)
    {
        if (first) first = false;
        else cout << ", ";
        cout << txn;
    }
    cout << '\n';
}