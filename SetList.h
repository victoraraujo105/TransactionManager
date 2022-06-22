#ifndef SETLIST_H
#define SETLIST_H

#include <map>
#include <list>
#include <iostream>

using namespace std;

template <class T>
class SetList
{
private:
    list<T> data;
    map<T, _List_iterator<T>> pos;

public:
    void push_back(T e)
    {
        if (contains(e))
        {
            // return;
        // data.erase(pos.at(e));
            // data.remove(e);
        }
        pos.insert({e, data.insert(data.end(), e)});
    }

    T pop_back()
    {
        auto back = data.back();
        data.remove(back);
        return back;
    }

    T& front() { return data.front(); }

    bool remove(T e)
    {
        data.remove(e);
        return pos.erase(e) > 0;
    }

    bool contains(T e) const
    {
        return pos.contains(e);
    }

    size_t size() { return data.size(); }

    auto begin() const -> decltype(data.begin())
    {
        return data.begin();
    }

    auto end() const -> decltype(data.end())
    {
        return data.end();
    }

    inline void clear()
    {
        data.clear();
        pos.clear();
    }

    inline bool empty()
    {
        return data.empty();
    }
};

#endif