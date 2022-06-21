#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include <set>
#include <string>

using namespace std;

class TransactionManager;
class Transaction;
class Action;
class ReadAction;
class Item;

using Dependents = vector<Transaction*>;
using Actions = vector<Action*>;
using LockedItems = set<Item*>;

class Transaction
{
private:
    TransactionManager& parent;
    Actions actions;
    size_t next, dependencies;
    Dependents dependents;
    LockedItems locked;

    friend Action;
public:
    
    const string id;
    
    Transaction(TransactionManager& parent, string id);

    void proceed();
    void forwardAndProceed();

    void addLock(Item* item);
    bool isLocking(Item* item);

    ~Transaction();
};

#endif