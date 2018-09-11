#pragma once

#ifndef ARRAY_H
#define ARRAY_H

#include <string>
#include <iostream>
#include <cassert>
#include <fstream>

#include "improperIndexException.h"

using namespace std;

template<typename Type>
class Array {

public:

    Array();
    Array(int numRows);
    Array(const Array<Type>& newArray);
    virtual ~Array();
    Array<Type> operator=(Array<Type> newArray);
    Type operator[](int index);
    Type at(int index);
    void setItemAt(int index, Type item);
    void addItem(Type item);
    void insertItemAt(Type item, int index);
    void removeItemAt(int index);
    void clear();
    int numItems();

private:
    Type* _array;
    int _numRowsAlloc; // number of slots allocated in memory
    int _numRowsUsed;  // number of memory slots which contain country data

    // internal methods
    void manageMemory();
};

//default constructor
template<typename Type>
Array<Type>::Array() {

    try {
        _numRowsUsed = 0;
        _numRowsAlloc = 4; //power of 2 makes for nice memory allocation algorithms
        _array = new Type[_numRowsAlloc];
        //check if memory allocation was succcessful
        if (_array == 0) {
            throw ImproperIndexException("Error: Could not allocate memory");
        }
    }
    catch (ImproperIndexException e) {
        cout << e.msg() << endl;
        _numRowsAlloc = 0;
    }
}

//overloaded constructor
template<typename Type>
Array<Type>::Array(int numRows) {
    try {
        _numRowsUsed = 0;
        _numRowsAlloc = numRows;
        _array = new Type[_numRowsAlloc];
        //check if memory allocation was succcessful
        if (_array == 0) {
            throw ImproperIndexException("Error: Could not allocate memory");
        }
    }
    catch (ImproperIndexException e) {
        cout << e.msg() << endl;
        _numRowsAlloc = 0;
    }
}

//copy constructor
template<typename Type>
Array<Type>::Array(const Array<Type> &newArray) {
    _array = newArray._array;
    _numRowsAlloc = newArray._numRowsAlloc;
    _numRowsUsed = newArray._numRowsUsed;
}

//destuctor
template<typename Type>
Array<Type>::~Array() {
}

//overloaded assignment operator
template<typename Type>
Array<Type> Array<Type>::operator=(Array<Type> newArray) {
    _array = newArray._array;
    _numRowsAlloc = newArray._numRowsAlloc;
    _numRowsUsed = newArray._numRowsUsed;
    return *this;
}

//overloaded [] operator as indexer
template<typename Type>
Type Array<Type>::operator[](int index) {
    return at(index);
}

//return the item at a given index
template<typename Type>
Type Array<Type>::at(int index) {
    try {
        if ((index > _numRowsUsed - 1) || (index < 0)) {
            throw ImproperIndexException();
        }
        return *(_array + index);
    }
    catch (ImproperIndexException e) {
    }
}

//replace the item at the specified index with a new item
template<typename Type>
void Array<Type>::setItemAt(int index, Type item) {
    try {
        if ((index > _numRowsUsed - 1) || (index < 0)) {
            throw ImproperIndexException();
        }
        *(_array + index) = item;
    }
    catch (ImproperIndexException e) {
    }
}

//add an item to the end of the database
template<typename Type>
void Array<Type>::addItem(Type item) {

    manageMemory();

    //set the next (first unused) element of _array to item
    //*(_array + _numRowsUsed) = item;
    _array[_numRowsUsed] = item;

    _numRowsUsed++;
}

//add an item to the database at the specified index
template<typename Type>
void Array<Type>::insertItemAt(Type item, int index) {
    try {
        if ((index > _numRowsUsed) || (index < 0)) {
            throw ImproperIndexException("Error: Invalid item index. Insertion aborted.");
        }
        manageMemory();
        //elements which were at or to the right of the specified index are moved one spot to the right
        for (int i = _numRowsUsed-1; i >= index; i--) {
            _array[i + 1] = _array[i];
        }
        *(_array + index) = item;
        _numRowsUsed++;
    }
    catch (ImproperIndexException e) {
        cout << e.msg() << endl;
    }
}

//remove the specified country from the database
template<typename Type>
void Array<Type>::removeItemAt(int index) {
    try {
        if ((index > _numRowsUsed - 1) || (index < 0)) {
            throw ImproperIndexException();
        }
        //elements which were to the right of the removed item are moved one spot to the left
        for (int i = index;i < _numRowsUsed;i++) {
            _array[i] = _array[i + 1];
        }

        //number of items stored is one fewer
        _numRowsUsed--;

        manageMemory();
    }
    catch (ImproperIndexException e) {
    }
}

//clear all items from the database
template<typename Type>
void Array<Type>::clear() {
    for (int i = _numRowsUsed-1; i >= 0; i--) {
        removeItemAt(i);
    }
}

//return the number of items currently stored in the database
template<typename Type>
int Array<Type>::numItems() {
    return _numRowsUsed;
}

//allocate and deallocate memory as needed
template<typename Type>
void Array<Type>::manageMemory() {

    //double allocated memory if necessary
    if (_numRowsUsed >= _numRowsAlloc) {
        Type *database_new;
        database_new = new Type[_numRowsAlloc * 2];
        for (int i = 0;i < _numRowsAlloc;i++) {
            database_new[i] = _array[i];
        }
        delete[] _array;
        _array = database_new;
        _numRowsAlloc *= 2;
    }

    //free up some memory if the array becomes small enough
    if (((2 * _numRowsUsed)+1 < _numRowsAlloc) && (_numRowsUsed > 0)) {
        Type *database_new;
        database_new = new Type[_numRowsAlloc / 2];
        for (int i = 0;i < _numRowsUsed;i++) {
            database_new[i] = _array[i];
        }
        delete[] _array;
        _array = database_new;
        _numRowsAlloc /= 2;
    }
}

#endif // ARRAY_H
