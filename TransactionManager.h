#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "Transaction.h"
#include "Item.h"
#include "Action.h"

#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

using Items = map<string, Item>;
using Transactions = map<string, Transaction*>;
using ActionQ = set<Action*>;

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

    void cmd_begin(string id)
    {
        // trans[id] = make_shared<Transaction>();
        // cout << "id: " << id 
        new Transaction(*this, id);
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

public:
    
    TransactionManager(string input_path):
        actionCount(0)
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
                cout << action->to_string() << " -- ";
                if (action->done()) cout << "done";
                else cout << "stashed";
                cout << '\n';
            }
        }
    }
};

#endif