#include "cache.h"

// Cache object constructor, initializes all old variables to 0/nullptr, makes the currenttable and sets all other
// variables to 0. sets hash function
Cache::Cache(int size, hash_fn hash){
    m_hash = hash;
    // adjusting size if needed (needs to be in range of MINID and MAXID and needs to be a prime number
    bool prime = isPrime(size);
    if (size < MINPRIME){
        m_currentCap = MINPRIME;
    }else if (size > MAXPRIME){
        m_currentCap = MAXPRIME;
    }else if (!prime){
        m_currentCap = findNextPrime(size);
    }else{
        m_currentCap = size;
    }

    // sets other variables to 0/nullptr
    m_currentSize = 0;
    m_oldCap = 0;
    m_oldSize = 0;
    m_currNumDeleted = 0;
    m_oldNumDeleted = 0;
    m_oldTable = nullptr;
    // creates current table
    m_currentTable = new Person[m_currentCap];
}

// Cache destructor, deallocates memory
Cache::~Cache(){
    // deletes currenttable and oldtable
    delete [] m_currentTable;
    m_currentTable = nullptr;
    delete [] m_oldTable;
    m_oldTable = nullptr;
    // sets hash function to null
    m_hash = nullptr;

    // sets all other variables to 0
    m_currentCap = 0;
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_oldCap = 0;
    m_oldNumDeleted = 0;
    m_oldSize = 0;
}

// inserts object into cache object, checks if person object already exists before inserting
// rehashes if needed (lamba > 0.5) and transfers after every insertion operation
bool Cache::insert(Person person){
    // checks if person object is in between MINID and MAXID and if it has not already been inserted before
    // also checks if the currentsize is under a certain amount (MAXPRIME case)
    if (person.getID() >= MINID && person.getID() <= MAXID && (getPerson(person.getKey(), person.getID())
    == EMPTY) && m_currentSize < MAXPRIME/2){

        // calculates quadratic probing and utilizes hash function
        int h = m_hash(person.getKey()) % m_currentCap;
        int counter = 0;
        // keeps searching for a valid index until m_currentTable[h] is either an empty space or has a deleted key
        // if ^ these cases are never true, will keep probing until the operations reach number of m_currentCap
        while ((!m_currentTable[h].m_key.empty() || m_currentTable[h] == DELETED) && counter <= m_currentCap){
            h = (h + (counter * counter)) % m_currentCap;
            counter++;
        }

        // if there is an available space or deleted key found, person is inserted into currentTable
        if (m_currentTable[h] == DELETED){
            m_currentTable[h] = person;
            m_currNumDeleted--;
        }else if (m_currentTable[h].getKey().empty()){
            m_currentTable[h] = person;
            m_currentSize++;
        }
        
        // if m_oldTable exists, will transfer 25% of oldSize to current table (incremental transferring)
        if (m_oldTable != nullptr){
            fillUpTable();
            // will deallocate m_oldTable if all entries in m_oldTable have been deleted
            if (m_oldNumDeleted == m_oldSize){
                deleteOld();
            }
        }

        // if lamba > 0.5, rehashing needs to occur. also checks if m_oldTable doesn't exist to avoid cases where
        // transferring and rehashing may occur simultaneously
        // will not rehash if m_currentCap is MAXPRIME or higher
        if (lambda() > 0.5 && m_oldTable == nullptr && m_currentCap < MAXPRIME){
            reHash();
        }
        return true;
    }
    return false;
}

// removes a person object if it exists, and from all the tables it is in
bool Cache::remove(Person person){
    bool removed = false;
    // uses quadratic probing and the hash function to get the index of the key
    int h = m_hash(person.getKey()) % m_currentCap;
    int counter = 0;
    // keeps probing until the person object is found. if person object is not found, will run until the number of
    // m_currentCap is reached
    while (!(m_currentTable[h] == person) && counter <= m_currentCap){
        h = (h + (counter * counter)) % m_currentCap;
        counter++;
    }

    // if the person object is found in the current table, it is "deleted"
    if (m_currentTable[h] == person) {
        m_currentTable[h] = DELETED;
        m_currNumDeleted++;
        removed = true;
    }

    // if oldTable exists, the person object will also be removed from there if found (and not deleted already)
    // then, it will incrementally transfer additional nodes in m_oldTable
    if (m_oldTable != nullptr){
        removed = oldSearch(person);
        fillUpTable();

        // if all elements in oldTable have been deleted, old table is deallocated
        if (m_oldNumDeleted == m_oldSize){
            deleteOld();
        }
    }

    // if 80% of the current table is deleted (from its total size) and old table doesn't exist, will rehash. prevents
    // rehashing and transferring from occurring simultaneously
    if (deletedRatio() > 0.8 && m_oldTable == nullptr){
        reHash();
        // if all elements in oldTable have been deleted, old table is deallocated
        if (m_oldNumDeleted == m_oldSize){
            deleteOld();
        }
    }
    return removed;
}

// returns the person object if found. will look through all tables. returns an empty person object if not found
Person Cache::getPerson(string key, int id) const{
    // uses quadratic probing and hash function to get index of the person object
    int h = m_hash(key) % m_currentCap;
    int count = 0;

    // keeps iterating until m_currenTable[h] equals the person object
    // if ^ this condition is not satisfied, will keep running until num is m_currentCap
    while (!(m_currentTable[h].getKey() == key && m_currentTable[h].getID() == id) && count <= m_currentCap){
        h = (h + (count * count)) % m_currentCap;
        count++;
    }

    // if the person object is found, will return the object
    if (m_currentTable[h].getKey() == key && m_currentTable[h].getID() == id && !(m_currentTable[h] == DELETED)){
        return m_currentTable[h];
    }

    // if the person object is not found in the currentTable but oldTable exists, checks old table
    // else if oldTable doesn't exist or person object is not found in oldTable, will return an empty object
    Person aPerson = EMPTY;
    if (m_oldTable != nullptr){
        aPerson = findOldCurrent(key, id, aPerson);
    }
    return aPerson;
}

float Cache::lambda() const {
    return float(m_currentSize)/ float(m_currentCap);
}

float Cache::deletedRatio() const {

    return float(m_currNumDeleted)/float(m_currentSize);
}

// provided function
void Cache::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

// provided function, returns if isPrime
bool Cache::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

// provided function, returns next prime number
int Cache::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) {
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0)
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

// provided function, overloaded operator to print person objects
ostream& operator<<(ostream& sout, const Person &person ) {
    if (!person.m_key.empty())
        sout << person.m_key << " (ID " << person.m_id << ")";
    else
        sout << "";
    return sout;
}

// provided function, overloaded operator to compare person objects
bool operator==(const Person& lhs, const Person& rhs){
    return ((lhs.m_key == rhs.m_key) && (lhs.m_id == rhs.m_id));
}

// helper function for transferring 25% of nodes from oldTable to currentTable (incremental transfer)
void Cache::fillUpTable() {
    // calculates 25% of oldSize and keeps iterating through oldTable to find live nodes to transfer
    int fourth = m_oldSize*0.25;
    int counter = 0;
    int index = 0;

    // if the number of live nodes is not less than 25% of old size
    if (m_oldSize-m_oldNumDeleted >= fourth){
        // will keep iterating until 25% of oldSize has been transferred
        while (counter < fourth){
            // checks for live node in oldTable
            if (m_oldTable[index].getKey() != DELETEDKEY && !m_oldTable[index].getKey().empty()){
                // if it is a live node, uses helper function to be transferred to currentTable
                hashFunctionHelper(index);
                m_oldTable[index] = DELETED;
                m_oldNumDeleted++;
                counter++;
            }
            index++;
        }
    }else{
        // else for "remainders", transfers the rest of the nodes (less than 25% of oldSize
        for (int i = 0; i < m_oldCap; i++){
            // checks for live node in oldTable
            if (m_oldTable[i].getKey() != DELETEDKEY && !m_oldTable[i].getKey().empty()){
                // if it is a live node, uses helper function to be transferred to currentTable
                hashFunctionHelper(i);
                m_oldTable[i] = DELETED;
                m_oldNumDeleted++;
            }
        }
        m_oldNumDeleted = m_oldSize;
    }
}

// helper function, rehashes if lamba > 0.5 or deletedRatio > 0.8
void Cache::reHash() {
    // copies all current variables to old variables and gets 25% of oldSize
    m_oldCap = m_currentCap;
    m_oldSize = m_currentSize;
    m_oldNumDeleted = m_currNumDeleted;
    m_oldTable = new Person[m_oldCap];
    int fourth = m_oldSize * 0.25;
    for(int i = 0; i < m_currentCap; i++){
        m_oldTable[i] = m_currentTable[i];
    }

    // builds the new current table
    m_currentCap = findNextPrime((m_currentSize-m_currNumDeleted)*4);
    m_currNumDeleted = 0;
    delete [] m_currentTable;
    m_currentTable = nullptr;
    m_currentTable = new Person[m_currentCap];
    m_currentSize = 0;

    // transfers 25% of oldSize to the current table
    int counter = 1;
    int index = 0;
    // runs until counter == 25% of oldSize and until all items have been deleted
    while(counter < fourth && m_oldNumDeleted != m_oldSize){
        // finds live nodes in oldTable
        if (!(m_oldTable[index] == DELETED) && !m_oldTable[index].getKey().empty()){
            // if live node in old table is found, uses helper function to be transferred to current table
            hashFunctionHelper(index);
            m_oldTable[index] = DELETED;
            counter++;
        }
        index++;
    }
}

// helper function, deallocates old variables
void Cache::deleteOld() {
    m_oldNumDeleted = 0;
    m_oldCap = 0;
    m_oldSize = 0;
    delete [] m_oldTable;
    m_oldTable = nullptr;
}

// helper function, looks through oldTable to find person object, else returns empty object
Person Cache::findOldCurrent(string key, int id, Person aPerson) const{
    // uses quadratic probing and hash function to find index of person
    int h = m_hash(key) % m_oldCap;
    int count = 0;

    // runs until person object if found. if person object is not found, runs until num is m_oldCap
    while (!(m_oldTable[h].getKey() == key && m_oldTable[h].getID() == id) && count <= m_oldCap){
        h = (h + (count * count)) % m_oldCap;
        count++;
    }

    // if person is found in oldTable, returns it. else, returns empty object
    if (m_oldTable[h].getKey() == key && m_oldTable[h].getID() == id && !(m_oldTable[h] == DELETED)){
        return m_oldTable[h];
    }
    return aPerson;
}

// helper function, removes person object from oldTable if found
bool Cache::oldSearch(Person person) {
    // uses quadratic probing and hash function to find index of person in oldTable
    int h = m_hash(person.getKey()) % m_oldCap;
    int counter = 0;

    // iterates until person object is found. if person object is not found, will run until num is m_oldCap
    while (!(m_oldTable[h] == person) && counter <= m_oldCap){
        h = (h + (counter * counter)) % m_oldCap;
        counter++;
    }

    // if person object is found in oldTable, person is removed
    if (m_oldTable[h] == person) {
        m_oldTable[h] = DELETED;
        m_oldNumDeleted++;
        return true;
    }
    return false;
}


// helper function, helps insert objects from old table to current table using quadratic probing and hash function
void Cache::hashFunctionHelper(int index) {
    // hash function and quadratic probing
    int h = m_hash(m_oldTable[index].getKey()) % m_currentCap;
    int count = 0;

    // iterates until there is space for person object (empty or deleted). if there is no space, will run until
    // num is currentCap
    while ((!m_currentTable[h].m_key.empty() || m_currentTable[h] == DELETED) && count <= m_currentCap){
        h = (h + (count * count)) % m_currentCap;
        count++;
    }

    // if a space is found, inserts the person object in currentTable
    if (m_currentTable[h].getKey().empty() || m_currentTable[h] == DELETED){
        m_currentTable[h] = m_oldTable[index];
        m_currentSize++;
        m_oldNumDeleted++;
    }
}
