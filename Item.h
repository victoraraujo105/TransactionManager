#ifndef ITEM_H
#define ITEM_H

#include <list>
#include <memory>
#include <string>

using namespace std;

enum class LockType {
    NONE = 1, SHARED = 2, EXCLUSIVE = 3
};

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


class Transaction;
class AccessAction;

using LockRequest = pair<Transaction*, LockType>;

class Item
{
private:
    LockType lock;
    int incidence;
    list<LockRequest> queue;

    friend Transaction;
    friend AccessAction;

    void popNext();

    LockType nextLockType()
    {
        return queue.front().second;
    }

    const LockRequest& peekNext() { return queue.front(); }

    void freeLock();

public:
    string id;

    Item(string id):
        lock(LockType::NONE), incidence(0), id(id)
    {}

    bool grantSharedLock(Transaction* txn);
    bool grantExclusiveLock(Transaction* txn);
};

#endif