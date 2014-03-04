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

        ElementRep(const Value& value) : value(value), valid(true) {}
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
        virtual void notify() const = 0;
        ~Observer() {}
    };

    int insert(const ValueType& value) {
        Super::insert(value);
    }

    void remove(int id) {
        Super::remove(id);
    }

    void attachObserver(Observer* observer) {
        _observers.push_back(observer);
    }

    void detachObserver(Observer* observer) {
        _observers.remove(observer);
    }

private:
    typedef std::list<Observer*> Observers;
    Observers _observers;

    void notify() const
    {
        for (typename Observers::const_iterator it = _observers.begin();
                it != _observers.end(); ++it)
        {
            it->notify();
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
