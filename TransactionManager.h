#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "Transaction.h"
#include "Item.h"
#include "Action.h"
#include "SetList.h"

#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>

using namespace std;

using Items = map<string, Item>;
using Transactions = map<string, Transaction*>;
auto cmp = [](Action* a, Action* b){ return *a < *b; };
using ActionQ = set<Action*, decltype(cmp)>;

enum class CommandType
{
    BEGIN, READ, WRITE, COMMIT
};

inline string& ltrim(string& s, string t = " \n\n\r\f\v")
{
    return s.erase(0, s.find_first_not_of(t));
}

inline string& toUpper(string& s)
{
    transform(s.begin(), s.end(), s.begin(), [](auto c){ return toupper(c); });
    return s;
}


enum class ProtocolType
{
    ABORT, WAIT_DIE, WOUND_WAIT
};

using TransactionQ = SetList<Transaction*>;

using Protocol = ActionState (*)(AccessAction*, TransactionQ);

class ItemMap: public Items
{
public:
    inline Item& operator [] (string id)
    {
        if (!contains(id)) insert({id, Item(id)});
        return at(id);
    }
};


class TransactionManager
{
private:
    Transactions trans;

    ItemMap items;
    ActionQ onWait;

    int actionCount;

    friend Transaction;
    friend Item;

    static ActionState abort(AccessAction* a, TransactionQ lockers)
    {
        const auto& txn = a->parent;
        for (const auto& b: lockers)
        {
            if (txn != b && txn->dependents.contains(b))
            {
                cout << a->to_string() << " -- deadlock!\n";
                cout << "Detectada dependencia mutua entre " << a->parent << " e " << b << "!\n";
                exit(EXIT_SUCCESS);
                return ActionState::DEADLOCKED;
            }
        }
        return ActionState::STASHED;
    }

    // should a be rollbacked?
    static ActionState wait_die(AccessAction* a, TransactionQ lockers)
    {
        const auto& txn = a->parent;
        for (const auto& b: lockers)
            if ((*b) < (*txn))
            {
                txn->rollback();
                // cout << 
                return ActionState::ROLLBACKED;
            }
        return ActionState::STASHED;
    }
    
    // 
    static ActionState wound_wait(AccessAction* a, TransactionQ lockers)
    {
        ActionState state = ActionState::DONE;
        bool rollbacked = false;
        const auto& txn = a->parent;
        for (const auto& b: lockers)
            if ((*txn) < (*b))
            {
                b->rollback();
                cout << b << " -- rollbacked\n";
                rollbacked = true;
            } else if (txn != b) state = ActionState::STASHED;
        if (rollbacked) a->item.printLocked();
        return state;
    }

    static const map<ProtocolType, Protocol> getProtocol;

    const Protocol protocol;

    void cmd_begin(string id)
    {
        // trans[id] = make_shared<Transaction>();
        // cout << "id: " << id 
        new Transaction(*this, id, actionCount++);
    }

    void cmd_read(string transaction, string item)
    {
        // trans.at(transaction).addAction
        // onWait.insert(make_shared<ReadAction>(actionCount++));
        new ReadAction(trans[transaction], items[item], actionCount++);
    }
    
    void cmd_write(string transaction, string item)
    {
        // onWait.insert(make_shared<WriteAction>(actionCount++));
        new WriteAction(trans[transaction], items[item], actionCount++);
    }

    void cmd_commit(string id)
    {
        new CommitAction(trans[id], actionCount++);
        // trans[id]->toCommit = true;
        // trans.erase(id);
    }

    void printLockTable()
    {
        for (const auto& item: items)
        {
            cout << "Item " << item.first << " - " << lockName.at(item.second.lock) << ":\n";
            bool first = true;
            for (const auto& locker: item.second.lockers)
            {
                if (first) first = false;
                else cout << ", ";
                cout << "T" << locker->id;
            }
            cout << '\n';

            cout << "\nFila:\n";
            first = true;
            for (const auto& locker: item.second.queue)
            {
                if (first) first = false;
                else cout << ", ";
                cout << "T" << locker.first->id << ": " << lockName.at(locker.second);
            }
            cout << '\n';
            cout << '\n';

        }
        cout << '\n';
    }

    void printDependencies()
    {
        cout << "Dependencias por transaçao:\n";
        for (const auto& pair: trans)
        {
            const auto& txn = pair.second;
            cout << txn << ": ";
            bool first = true;
            for (const auto& dep: txn->dependencies)
            {
                if (first) first = false;
                else cout << ", ";
                cout << dep;
            }
            cout << '\n';
        }
    }

public:
    
    TransactionManager(string input_path, ProtocolType protocol_type = ProtocolType::ABORT):
        actionCount(0), protocol(getProtocol.at(protocol_type))
    {
        ifstream input(input_path);
        stringstream original_schedule;
        stringstream final_schedule;

        while (input)
        {
            string cmd, arg;
            getline(input, cmd, '(');
            ltrim(cmd);
            toUpper(cmd);
            getline(input, arg, ')');

            // cout << "\ncmd: " << cmd << '\n';
            // cout << "arg: " << arg << '\n';
            
            if (cmd == "BT")
            {
                cout << "BT(" << arg << ") -- done\n";
                cmd_begin(arg);

            } else if (cmd == "C")
            {
                cmd_commit(arg);

            } else if (cmd.starts_with("R"))
            {
                cmd_read(cmd.substr(1), arg);
            } else if (cmd.starts_with("W"))
            {
                cmd_write(cmd.substr(1), arg);
            }

            while (!onWait.empty())
            {
                auto action = onWait.extract(onWait.begin()).value();
                action->run();
                // printDependencies();
                // print lock table
                // print
                // cout << action->to_string() << " -- ";
                // if (action->state == ActionState::DONE) cout << "done";
                // else cout << "stashed";
                // cout << '\n';
                // printLockTable();
            }
        }
    }
};

#endif