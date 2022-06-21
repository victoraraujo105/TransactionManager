#include "Item.h"
#include "Transaction.h"

#include <iostream>

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

void Item::freeLock()
{
    incidence--;
    if (incidence == 1 && peekNext().first->isLocking(this))
    {
        lock = LockType::EXCLUSIVE;
        popNext();
    }
    if (incidence > 0) return;
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
            incidence++;
        } while (queue.size() > 0 && lock*nextLockType() == LockType::SHARED);
        return;
    } 
    lock = LockType::NONE;
}

void Item::popNext()
{
    auto txn = queue.front().first;
    queue.pop_front();
    txn->addLock(this);
    txn->proceed();
}


bool Item::grantSharedLock(Transaction* txn)
{
    if (txn->isLocking(this)) return true;

    switch (lock)
    {
    case LockType::NONE:
        lock = LockType::SHARED;
    case LockType::SHARED:
        if (queue.size() == 0)
        {
            txn->addLock(this);
            incidence++;
            return true;
        }
    case LockType::EXCLUSIVE:
        queue.emplace_back(pair(txn, LockType::SHARED));
    default:
        break;
    }
    return false;
}
bool Item::grantExclusiveLock(Transaction* txn)
{
    // cout << "incidence: " << incidence << '\n';
    // cout << "ExclusiveLock on item " << id << " requested by T" << txn->id << ".\n";
    if (incidence == 0 || (incidence == 1 && txn->isLocking(this)))
    {
        lock = LockType::EXCLUSIVE;
        if (incidence == 0)
        {
            txn->addLock(this);
            incidence++;
        }
        // cout << "granted!\n";
        return true;
    }
    queue.emplace_back(pair(txn, LockType::EXCLUSIVE));
    // cout << "not granted!\n";
    return false;
}