#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include <set>
#include <string>
#include <iostream>

#include "SetList.h"

using namespace std;

class TransactionManager;
class Transaction;
class Action;
class ReadAction;
class Item;

using Dependents = SetList<Transaction*>;
using Dependencies = Dependents;
using Actions = vector<Action*>;
using LockedItems = SetList<Item*>;

class Transaction
{
private:
    TransactionManager& parent;
    Actions actions;
    size_t next;
    Dependencies dependencies;
    Dependents dependents;
    LockedItems locked;
    bool rollbacked;
    int timestamp;

    friend Action;
    friend Item;
    friend TransactionManager;

public:

    void rollback();

    void restart()
    {
        cout << this;
        if (rollbacked)
        {
            cout << " -- restarted\n";
            rollbacked = false;
        } else  cout << " -- continued\n";
        proceed();
    }

    // void rollback(Item* item);

    void addDependencies(Dependencies& deps)
    {
        for (const auto& dep: deps)
        {
            if (dep == this) continue;
            dependencies.push_back(dep);
            dep->dependents.push_back(this);
        }
    }

    void removeDependencies(Dependencies& deps)
    {
        for (const auto& dep: deps)
        {
            if (dep == this) continue;
            dependencies.remove(dep);
            dep->dependents.remove(this);
        }
    }

    void removeDependecy(Transaction* dep)
    {
        dependencies.remove(dep);
        // dep->dependents.remove(dep);
        if (dependencies.empty()) restart();
    }
    
    const string id;
    
    Transaction(TransactionManager& parent, string id, int timestamp);

    void proceed();
    void forwardAndProceed();

    void addLock(Item* item);
    bool isLocking(Item* item);

    template <class O>
    inline friend O& operator << (O& o, const Transaction* txn)
    {
        o << "T" << txn->id;
        return o;
    }

    inline bool operator < (const Transaction& b)
    {
        return timestamp < b.timestamp;
    }

    ~Transaction();
};

#endif