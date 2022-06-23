#ifndef ACTION_H
#define ACTION_H

#include <cstdlib>
#include <string>
#include <iostream>

#include "SetList.h"

using namespace std;

enum class ActionType {
    READ, WRITE, COMMIT
};

class Transaction;
class TransactionManager;
class Item;

enum class ActionState
{
    STASHED, DONE, DEADLOCKED, ROLLBACKED
};

class Action
{
protected:
    Transaction* const parent;
    const string& parentId;
    const int timestamp;
    ActionState state;

    friend Item;

    void addDependencies(SetList<Transaction*> dep);
public:
    const ActionType type;
    
    Action(Transaction* const parent, ActionType type, int timestamp);

    inline bool operator < (const Action& other) const
    {
        return timestamp < other.timestamp;
    }

    virtual void run() = 0;
    
    virtual string to_string() = 0;

    virtual ~Action()
    {
        cout << "Action Base Destroyer Called.\n";
    }

    // template <class O>
    // friend O& operator << (O& o, const Action& a)
    // {
    //     o << a.to_string();
    //     return o;
    // }

};

class AccessAction: public Action
{
protected:
    Item& item;
    const string itemId;
    friend TransactionManager;
public:
    AccessAction(Transaction* const parent, Item& item, ActionType type, int timestamp);

};

class ReadAction: public AccessAction
{
public:
    ReadAction(Transaction* const parent, Item& item, int timestamp):
        AccessAction(parent, item, ActionType::READ, timestamp)
    {}

    void run() override;

    string to_string() override
    {
        return "R"+ parentId + "(" + itemId + ")";
    }

    virtual ~ReadAction()
    {
        cout << "Action " << to_string() << " destroyed.\n";
    }
};


class WriteAction: public AccessAction
{
public:
    WriteAction(Transaction* const parent, Item& item, int timestamp):
        AccessAction(parent, item, ActionType::WRITE, timestamp)
    {}
    
    void run() override;

    string to_string() override
    {
        return "W"+ parentId + "(" + itemId + ")";
    }
    
    virtual ~WriteAction()
    {
        cout << "Action " << to_string() << " destroyed.\n";
    }
};

class CommitAction: public Action
{
public:
    CommitAction(Transaction* const parent, int timestamp):
        Action(parent, ActionType::COMMIT, timestamp)
    {}
    
    void run() override;

    string to_string() override
    {
        return "C("+ parentId + ")";
    }
    
    virtual ~CommitAction()
    {
        cout << "Action " << to_string() << " destroyed.\n";
    }
};

#endif