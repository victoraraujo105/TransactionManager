#ifndef ITEM_H
#define ITEM_H

#include "List.h"
#include <memory>
#include <string>
#include <map>

#include "List.h"

using namespace std;

enum class LockType {
    NONE = 1, SHARED = 2, EXCLUSIVE = 3
};

extern const map<LockType, string> lockName;

LockType operator * (const LockType& a, const LockType& b);

// LockType operator + (const LockType& a, const LockType& b)
// {
//     switch (a)
//     {
//         case LockType::NONE:
//             return b;
//         case LockType::SHARED:
//             return a;
//         default:

//     }
// }


class TransactionManager;
class Transaction;
class AccessAction;
enum class ActionState;

using LockRequest = pair<Transaction*, LockType>;
using Lockers = List<Transaction*>;

class Item
{
private:
    LockType lock;
    Lockers lockers;
    List<LockRequest> queue;

    friend TransactionManager;
    friend Transaction;
    friend AccessAction;

    void popNext();

    LockType nextLockType()
    {
        return queue.front().second;
    }

    const LockRequest& peekNext() { return queue.front(); }

    void freeLock(Transaction* txn);

    bool deadlocked(Transaction* a);

    bool waitDie(Transaction* a);
    
    bool woundWait(Transaction* a);

public:
    string id;

    Item(string id):
        lock(LockType::NONE), id(id)
    {}

    ActionState grantSharedLock(AccessAction* txn);
    ActionState grantExclusiveLock(AccessAction* txn);

    void printQueue();
    void printLocked();
};

#endif