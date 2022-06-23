#ifndef SETLIST_H
#define SETLIST_H

#include <list>

using namespace std;

template <class T>
class SetList: public list<T>
{
    using base = list<T>;
public:
    bool contains(const T& a)
    {
        for (const auto& b: *this)
        {
            if (a == b) return true;
        }
        return false;
    }

    void push_back (const T& val)
    {
        if (!contains(val)) base::push_back(val);
    }

    void push_back (T&& val)
    {
        if (!contains(val)) base::push_back(val);
    }

    T pop_front()
    {
        T front = base::front();
        base::pop_front();
        return front;
    }

    T pop_back()
    {
        T back = base::back();
        base::pop_back();
        return back;
    }
};

#endif