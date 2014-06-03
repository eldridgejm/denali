// Copyright (c) 2014, Justin Eldridge, Mikhail Belkin, and Yusu Wang
// at The Ohio State University. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DENALI_MAPPABLE_LIST_H
#define DENALI_MAPPABLE_LIST_H

#include <list>
#include <vector>

namespace denali {

/// \brief A list supporting random access.
template <typename Value>
class MappableListBase
{

    struct ElementRep
    {
        bool valid;
        int prev_element, next_element;
        Value value;

        ElementRep(const Value& value) : valid(true), value(value) {}
    };

    size_t _size;
    std::vector<ElementRep> _elements;

    int _first_element;
    int _first_free_element;

public:

    typedef Value ValueType;

    MappableListBase() :
            _size(0), _first_element(-1), _first_free_element(-1)
    {}

    ValueType& operator[](int n) { 
        return _elements[n].value; 
    }

    const ValueType& operator[](int n) const { 
        return _elements[n].value; 
    }

    size_t size() const {
        return _size;
    }

    /// \brief Insert a value into the structure, returning an element.
    int insert(const Value& value)
    {
        int n;

        // if there isn't a free element, we push a new one
        if (_first_free_element == -1)
        {
            n = _elements.size(); 
            _elements.push_back(ElementRep(value)); 
        } else {
            n = _first_free_element;
            _elements[n].value = value;
            _elements[n].valid = true;
            _first_free_element = _elements[n].next_element;
        }

        _elements[n].next_element = _first_element;

        if (_first_element != -1) {
            _elements[_first_element].prev_element = n;
        }

        _first_element = n;

        _elements[n].prev_element = -1;

        _size++;

        return n;
    }

    /// \brief Remove the element.
    void remove(int n)
    {
        if (_elements[n].next_element != -1) {
            _elements[_elements[n].next_element].prev_element = _elements[n].prev_element;
        }

        if (_elements[n].prev_element != -1)
        {
            _elements[_elements[n].prev_element].next_element = _elements[n].next_element;
        } else {
            _first_element = _elements[n].next_element;
        }

        _elements[n].next_element = _first_free_element;
        _first_free_element = n;

        _elements[n].valid = false;

        _size--;
    }

    bool isValid(int n) const {
        return n >= 0 && n < _elements.size() && _elements[n].valid;
    }

    int getFirst() const {
        return _first_element;
    }

    int getNext(int n) const {
        return _elements[n].next_element;
    }

    int getMaxIdentifier() const {
        return _elements.size();
    }
};


/// \brief An iterator over the items in a mappable list.
template <typename MappableList>
class MappableListIterator
{

    int _index;
    MappableList& _mlist;

public:

    typedef typename MappableList::ValueType ValueType;

    MappableListIterator(MappableList& mlist) :
            _mlist(mlist), _index(mlist.getFirst()) 
    { }

    bool done() const {
        return !_mlist.isValid(_index);
    }

    void operator++() {
        _index = _mlist.getNext(_index);
    }

    ValueType& operator*() { 
        return _mlist[_index]; 
    }

    const ValueType& operator*() const { 
        return _mlist[_index]; 
    }

};


template <typename Super>
class ObservableMappableListMixin : public Super
{
public:

    typedef typename Super::ValueType ValueType;

    class Observer
    {
    public:
        virtual void notify() const = 0;
        ~Observer() {}
    };

    int insert(const ValueType& value) {
        int n = Super::insert(value);
        notify();
        return n;
    }

    void remove(int id) {
        Super::remove(id);
        notify();
    }

    void attachObserver(Observer& observer) {
        _observers.push_back(&observer);
    }

    void detachObserver(Observer& observer) {
        _observers.remove(&observer);
    }

private:
    typedef std::list<Observer*> Observers;
    Observers _observers;

    void notify() const
    {
        for (typename Observers::const_iterator it = _observers.begin();
                it != _observers.end(); ++it)
        {
            (*it)->notify();
        }
    }

};


/// \brief A list supporting quick lookups and random access.
/// \ingroup mappable_list
template <typename Value>
class MappableList : public 
        ObservableMappableListMixin 
        <MappableListBase <Value> >
{
    typedef 
    ObservableMappableListMixin 
    <MappableListBase <Value> >
    Mixin;

public:

};


} // namespace denali

#endif
