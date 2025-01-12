#ifndef CACHE_H
#define CACHE_H
#include <iostream>
#include <string>
#include "math.h"
using namespace std;
class Tester;   // forward declaration, will be used for testing
class Person;   // forward declaration
class Cache;    // forward declaration
const int MINID = 1000;
const int MAXID = 9999;
const int MINPRIME = 101;   // Min size for hash table
const int MAXPRIME = 99991; // Max size for hash table
#define EMPTY Person("",0)
#define DELETED Person("DELETED")
#define DELETEDKEY "DELETED"
typedef unsigned int (*hash_fn)(string); // declaration of hash function
class Person{
public:
    friend class Tester;
    friend class Cache;
    Person(string key="", int id=0){m_key = key; m_id = id;}
    string getKey() const {return m_key;}
    int getID() const {return m_id;}
    void setKey(string key){m_key=key;}
    void setID(int id){m_id=id;}
    const Person& operator=(const Person& rhs){
        if (this != &rhs){
            m_key = rhs.m_key;
            m_id = rhs.m_id;
        }
        return *this;
    }
    // Overloaded insertion operator
    friend ostream& operator<<(ostream& sout, const Person &person );
    // Overloaded equality operator
    friend bool operator==(const Person& lhs, const Person& rhs);
private:
    string m_key;   // the search string used as key in the hash table
    int m_id;       // a unique ID number identifying the object
};

class Cache{
public:
    friend class Tester;
    Cache(int size, hash_fn hash);
    ~Cache();
    // Returns Load factor of the new table
    float lambda() const;
    // Returns the ratio of deleted slots in the new table
    float deletedRatio() const;
    // insert only happens in the new table
    bool insert(Person person);
    // remove can happen from either table
    bool remove(Person person);
    // find can happen in either table
    Person getPerson(string key, int id) const;
    void dump() const;

private:
    hash_fn     m_hash;         // hash function

    Person*     m_currentTable; // hash table
    int         m_currentCap;   // hash table size (capacity)
    int         m_currentSize;  // current number of entries
    // m_currentSize includes deleted entries
    int         m_currNumDeleted;// number of deleted entries

    Person*     m_oldTable;     // hash table
    int         m_oldCap;       // hash table size (capacity)
    int         m_oldSize;      // current number of entries
    // m_oldSize includes deleted entries
    int         m_oldNumDeleted;// number of deleted entries

    //private helper functions
    bool isPrime(int number); // provided helper function to calculate validity of prime number
    int findNextPrime(int current); // provided helper function to calculate prime number
    void fillUpTable(); // helper function used to transfer nodes
    void reHash(); // helper function to perform rehash operation
    void deleteOld(); // deallocates old table
    Person findOldCurrent(string, int, Person) const; // used to find person object in old table
    bool oldSearch(Person); // removes person objects from old table
    void hashFunctionHelper(int); // quadratic probing helper
};
#endif
