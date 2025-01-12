#include "cache.h"
#include <random>
#include <vector>
const int MINSEARCH = 0;
const int MAXSEARCH = 7;
// the following array defines sample search strings for testing
string searchStr[MAXSEARCH+1]={"c++","python","java","scheme","prolog","c#","c","js"};
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else{ //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

// Tester class which has tester functions
class Tester{
    public:
    void insertNormalAndError(); // tests all cases of normal insertion and error
    int hashFunction(const Cache&, Person); // hash function reimplemented for tester class
    bool findDuplicate(const Cache&, Person); // checks if there are duplicates in a cache object
    void getPerson(); // tests all cases of get person with normal insertion/removal
    void removeNormalAndError(); // tests all cases of normal removal and error
    void insertRehashRetrieve(); // tests cases of insert with rehashing
    float lamba(int, int); // lamba function reimplemented for tester class
    void removeRehashRetrieve(); // tests cases of remove with rehashing
    int duplicate(const Cache&, Person); // returns number of duplicates if there are any
    float deletedRatio(int, int); // deletedRatio function reimplemented for tester class
    void insertAndRemove(); // tests cases of insert and remove combined with rehashing
    void constructor(); // tests constructor
};

unsigned int hashCode(const string str);

int main(){
    Tester tester;
    tester.insertNormalAndError();
    tester.getPerson();
    tester.removeNormalAndError();
    tester.insertRehashRetrieve();
    tester.removeRehashRetrieve();
    tester.insertAndRemove();
    tester.constructor();
    return 0;
}

// hash function
unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for ( unsigned int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}

// insert test cases, tests normal cases of insert without rehashing. includes collision/non collision, and error case
// ^ (duplicates)
void Tester::insertNormalAndError() {
    // creating first Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    bool sizeChange = true;
    int capacity = 49;
    int positionList[50];
    vector<Person> dataList;
    Random RndID(MINID, MAXID);
    Random RndStr(MINSEARCH, MAXSEARCH);
    Cache cache(MINPRIME, hashCode);

    // builds cache object using insert
    // collision
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // saving data for later use
        dataList.push_back(dataObj);
        // saving index for later use
        positionList[i] = hashFunction(cache, dataObj);
        // inserting data in to the Cache object
        cache.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache.m_currentSize) {
            sizeChange = false;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    int counter = 0;
    bool result = true;
    bool correctPlace = true;
    while (counter < cache.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++) {
            for (int i = 0; i < cache.m_currentCap; i++) {
                if ((*it).getKey() == cache.m_currentTable[i].getKey() &&
                    (*it).getID() == cache.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == cache.getPerson((*it).getKey(), (*it).getID()));
    }

    if (correctPlace && sizeChange && result) {
        cout << "INSERT NORMAL 1 PASSED" << endl;
    } else {
        cout << "INSERT NORMAL 1 FAILED" << endl;
    }



    // creating second Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // non collision
    Cache cache2(MINPRIME, hashCode);
    sizeChange = true;
    capacity = 7;
    int positionList2[8];
    vector<Person> dataList2;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[i], RndID.getRandNum());
        // saving data for later use
        dataList2.push_back(dataObj);
        // saving index for later use
        positionList2[i] = hashFunction(cache2, dataObj);
        // inserting data in to the Cache object
        cache2.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache2.m_currentSize) {
            sizeChange = false;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    counter = 0;
    result = true;
    correctPlace = true;
    while (counter < cache2.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++) {
            for (int i = 0; i < cache2.m_currentCap; i++) {
                if ((*it).getKey() == cache2.m_currentTable[i].getKey() &&
                    (*it).getID() == cache2.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList2[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++){
        result = result && (*it == cache2.getPerson((*it).getKey(), (*it).getID()));
    }

    if (correctPlace && sizeChange && result) {
        cout << "INSERT NORMAL 2 PASSED" << endl;
    } else {
        cout << "INSERT NORMAL 2 FAILED" << endl;
    }



    // creating third Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // tests for duplicate insertion
    Cache cache3(MINPRIME, hashCode);
    sizeChange = true;
    bool duplicates = false;
    int num = 0;
    capacity = 4;
    int positionList3[5];
    vector<Person> dataList3;

    // builds cache object using insert and tries to insert duplicate objects
    for (int i = 0; i < capacity; i++) {
        num = 0;
        // generating random data
        Person dataObj = Person(searchStr[i], RndID.getRandNum());
        // saving data for later use
        dataList3.push_back(dataObj);
        // saving index for later use
        positionList3[i] = hashFunction(cache3, dataObj);
        // inserting data in to the Cache object (twice), ideally should be inserted once
        cache3.insert(dataObj);
        cache3.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache3.m_currentSize) {
            sizeChange = false;
        }

        // checks if the duplicates have been inserted
        if (!duplicates){
            duplicates = findDuplicate(cache3, dataObj);
            num = duplicate(cache3, dataObj);
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    counter = 0;
    result = true;
    correctPlace = true;
    while (counter < cache3.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList3.begin(); it != dataList3.end(); it++) {
            for (int i = 0; i < cache3.m_currentCap; i++) {
                if ((*it).getKey() == cache3.m_currentTable[i].getKey() &&
                    (*it).getID() == cache3.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList3[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList3.begin(); it != dataList3.end(); it++){
        result = result && (*it == cache3.getPerson((*it).getKey(), (*it).getID()));
    }

    if (correctPlace && sizeChange && !duplicates && result && num < 2) {
        cout << "INSERT ERROR PASSED" << endl;
    } else {
        cout << "INSERT ERROR FAILED" << endl;
    }
}

// recreating hash function for testing (verify insertion without rehashing)
int Tester::hashFunction(const Cache &cache, Person person) {
    int h = cache.m_hash(person.getKey()) % cache.m_currentCap;
    int counter = 0;
    while (!cache.m_currentTable[h].m_key.empty() && counter <= cache.m_currentCap){
        h = (h + (counter * counter)) % cache.m_currentCap;
        counter++;
    }
    return h;
}

// checks if there are duplicates in a cache object
bool Tester::findDuplicate(const Cache &cache, Person person) {
    int counter = 0;
    for (int i = 0; i < cache.m_currentCap; i++){
        if (cache.m_currentTable[i].m_key == person.m_key && cache.m_currentTable[i].m_id == person.m_id){
            counter++;
        }
    }
    if (counter > 1){
        return true;
    }
    return false;
}

// tests normal cases of get person without rehashing
void Tester::getPerson() {
    // creating first Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // collision
    bool sizeChange = true;
    int capacity = 49;
    int positionList[50];
    vector<Person> dataList;
    Random RndID(MINID, MAXID);
    Random RndStr(MINSEARCH, MAXSEARCH);
    Cache cache(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // saving data for later use
        dataList.push_back(dataObj);
        // saving index for later use
        positionList[i] = hashFunction(cache, dataObj);
        // inserting data in to the Cache object
        cache.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache.m_currentSize) {
            sizeChange = false;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    int counter = 0;
    bool correctPlace = true;
    while (counter < cache.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++) {
            for (int i = 0; i < cache.m_currentCap; i++) {
                if ((*it).getKey() == cache.m_currentTable[i].getKey() &&
                    (*it).getID() == cache.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }

    // tests what getPerson does with a person object which does not exist in the cache object
    Person aPerson = cache.getPerson(searchStr[RndStr.getRandNum()], RndID.getRandNum());
    if (correctPlace && sizeChange && aPerson == EMPTY) {
        cout << "GET PERSON ERROR PASSED" << endl;
    } else {
        cout << "GET PERSON ERROR FAILED" << endl;
    }



    // creating second Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // collision
    sizeChange = true;
    capacity = 49;
    int positionList2[50];
    vector<Person> dataList2;
    Cache cache2(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // saving data for later use
        dataList2.push_back(dataObj);
        // saving index for later use
        positionList2[i] = hashFunction(cache2, dataObj);
        // inserting data in to the Cache object
        cache2.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache2.m_currentSize) {
            sizeChange = false;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    counter = 0;
    correctPlace = true;
    bool result = true;
    while (counter < cache2.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++) {
            for (int i = 0; i < cache2.m_currentCap; i++) {
                if ((*it).getKey() == cache2.m_currentTable[i].getKey() &&
                    (*it).getID() == cache2.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList2[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++){
        result = result && (*it == cache2.getPerson((*it).getKey(), (*it).getID()));
    }

    if (correctPlace && sizeChange && result) {
        cout << "GET PERSON NORMAL 1 PASSED" << endl;
    } else {
        cout << "GET PERSON NORMAL 1 FAILED" << endl;
    }



    // creating third Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // non collision
    Cache cache3(MINPRIME, hashCode);
    sizeChange = true;
    capacity = 7;
    int positionList3[8];
    vector<Person> dataList3;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[i], RndID.getRandNum());
        // saving data for later use
        dataList3.push_back(dataObj);
        // saving index for later use
        positionList3[i] = hashFunction(cache3, dataObj);
        // inserting data in to the Cache object
        cache3.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache3.m_currentSize) {
            sizeChange = false;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    counter = 0;
    correctPlace = true;
    result = true;
    while (counter < cache3.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList3.begin(); it != dataList3.end(); it++) {
            for (int i = 0; i < cache3.m_currentCap; i++) {
                if ((*it).getKey() == cache3.m_currentTable[i].getKey() &&
                    (*it).getID() == cache3.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList3[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++){
        result = result && (*it == cache2.getPerson((*it).getKey(), (*it).getID()));
    }

    if (correctPlace && sizeChange && result) {
        cout << "GET PERSON NORMAL 2 PASSED" << endl;
    } else {
        cout << "GET PERSON NORMAL 2 FAILED" << endl;
    }
}

// tests normal and error cases of remove without rehash
void Tester::removeNormalAndError() {
    // creating first Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // collision
    bool sizeChange = true;
    int capacity = 7;
    int positionList[8];
    vector<Person> dataList;
    Random RndID(MINID, MAXID);
    Random RndStr(MINSEARCH, MAXSEARCH);
    Cache cache(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[i], RndID.getRandNum());
        // saving data for later use
        dataList.push_back(dataObj);
        // saving index for later use
        positionList[i] = hashFunction(cache, dataObj);
        // inserting data in to the Cache object
        cache.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache.m_currentSize) {
            sizeChange = false;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    // also tests if removed objects have been removed
    int counter = 0;
    bool correctPlace = true;
    bool result = true;
    while (counter < cache.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++) {
            for (int i = 0; i < cache.m_currentCap; i++) {
                if ((*it).getKey() == cache.m_currentTable[i].getKey() &&
                    (*it).getID() == cache.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == cache.getPerson((*it).getKey(), (*it).getID()));
    }

    // testing if removed objects have been removed
    cache.remove(dataList.front());
    cache.remove(dataList.back());
    Person test = cache.getPerson(dataList.front().getKey(), dataList.front().getID());
    Person test2 = cache.getPerson(dataList.back().getKey(), dataList.back().getID());

    if (correctPlace && sizeChange && test == EMPTY && test2  == EMPTY && result) {
        cout << "REMOVE NORMAL 1 PASSED" << endl;
    } else {
        cout << "REMOVE NORMAL 1 FAILED" << endl;
    }



    // creating second Cache object with variables initialized, creates dataList to store person objects and positionList
    // to store the correct positions of each person object
    // collision
    // stores people to remove in personList
    sizeChange = true;
    capacity = 49;
    int positionList2[50];
    Person personList[7];
    counter = 0;
    vector<Person> dataList2;
    Cache cache2(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // saving data for later use
        dataList2.push_back(dataObj);
        // saving index for later use
        positionList2[i] = hashFunction(cache2, dataObj);
        // inserting data in to the Cache object
        cache2.insert(dataObj);

        // checks if size is not being updated correctly
        if (i + 1 != cache2.m_currentSize) {
            sizeChange = false;
        }

        // stores specific people to remove in personList
        if (i%7 == 0){
            personList[counter] = dataObj;
            counter++;
        }
    }

    // tests if all person objects have been inserted correctly and in the correct place
    counter = 0;
    result = true;
    correctPlace = true;
    while (counter < cache2.m_currentSize) {
        int index = -1;
        for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++) {
            for (int i = 0; i < cache2.m_currentCap; i++) {
                if ((*it).getKey() == cache2.m_currentTable[i].getKey() &&
                    (*it).getID() == cache2.m_currentTable[i].getID()) {
                    index = i;
                }
            }
            if (index != positionList2[counter]) {
                correctPlace = false;
            }
            counter++;
        }
    }
    for (vector<Person>::iterator it = dataList2.begin(); it != dataList2.end(); it++){
        result = result && (*it == cache2.getPerson((*it).getKey(), (*it).getID()));
    }

    // removes people in personList and checks if they have really been removed
    int i = 0;
    bool removed = true;
    while (i < 7 && removed){
        cache2.remove(personList[i]);
        Person person = cache2.getPerson(personList[i].m_key, personList[i].getID());
        if (!(person == EMPTY)){
            removed = false;
        }
        i++;
    }

    if (correctPlace && sizeChange && removed && result) {
        cout << "REMOVE NORMAL 2 PASSED" << endl;
    } else {
        cout << "REMOVE NORMAL 2 FAILED" << endl;
    }
}

// tests if insertion works properly when rehashing occurs
void Tester::insertRehashRetrieve() {
    // creating first Cache object with variables initialized
    bool isThere = true;
    int capacity = 60;
    bool reHash = false;
    Random RndID(MINID, MAXID);
    Random RndStr(MINSEARCH, MAXSEARCH);
    Cache cache(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache.m_currentSize+1, cache.m_currentCap);
        // inserting data in to the Cache object
        cache.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }
    if (isThere && reHash && cache.m_oldTable == nullptr && cache.m_currentSize == capacity) {
        cout << "INSERT REHASH NORMAL 1 PASSED" << endl;
    } else {
        cout << "INSERT REHASH NORMAL 1 FAILED" << endl;
    }



    // creating second Cache object with variables initialized
    Cache cache2(199, hashCode);
    capacity = 101;
    isThere = true;
    reHash = false;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache2.m_currentSize+1, cache2.m_currentCap);
        // inserting data in to the Cache object
        cache2.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache2.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache2.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    int fourth = cache2.m_oldSize*0.25;
    if (isThere && reHash && cache2.m_oldTable != nullptr && cache2.m_currentSize == fourth*2) {
        cout << "INSERT REHASH NORMAL 2 PASSED" << endl;
    } else {
        cout << "INSERT REHASH NORMAL 2 FAILED" << endl;
    }



    // creating third Cache object with variables initialized
    Cache cache3(199, hashCode);
    capacity = 200;
    isThere = true;
    reHash = false;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache3.m_currentSize+1, cache3.m_currentCap);
        // inserting data in to the Cache object
        cache3.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache3.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache3.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    if (isThere && reHash && cache3.m_oldTable == nullptr && cache3.m_currentSize == capacity) {
        cout << "INSERT REHASH NORMAL 3 PASSED" << endl;
    } else {
        cout << "INSERT REHASH NORMAL 3 FAILED" << endl;
    }



    // creating fourth Cache object with variables initialized
    Cache cache4(199, hashCode);
    capacity = 1000;
    isThere = true;
    reHash = false;
    int duplicateNum = 0;

    // builds cache object using insert (i have to check for duplicates here since there are a lot of insertions
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        duplicateNum = duplicateNum + duplicate(cache4, dataObj);
        // getting lamba value
        float lambaVal = lamba(cache4.m_currentSize+1, cache4.m_currentCap);
        // inserting data in to the Cache object
        cache4.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache4.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache4.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    if (isThere && reHash && cache4.m_oldTable == nullptr && cache4.m_currentSize == (capacity-1)-duplicateNum){
        cout << "INSERT REHASH NORMAL 4 PASSED" << endl;
    } else {
        cout << "INSERT REHASH NORMAL 4 FAILED" << endl;
    }



    // creating fifth Cache object with variables initialized
    Cache cache5(199, hashCode);
    capacity = 100;
    isThere = true;
    reHash = false;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache5.m_currentSize+1, cache5.m_currentCap);
        // inserting data in to the Cache object
        cache5.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache5.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache5.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // tries to insert duplicate person and checks if there are duplicates in the cache object
    Person aPerson = cache5.m_oldTable[capacity];
    cache5.insert(aPerson);
    int duplicates = duplicate(cache5, aPerson);
    fourth = cache5.m_oldSize*0.25-1;

    if (isThere && reHash && cache5.m_oldTable != nullptr && cache5.m_currentSize == fourth && duplicates < 2) {
        cout << "INSERT REHASH NORMAL 5 PASSED" << endl;
    } else {
        cout << "INSERT REHASH NORMAL 5 FAILED" << endl;
    }
}

float Tester::lamba(int size, int capacity) {
    return float(size)/float(capacity);
}

// checks if remove functions correctly after rehashing
void Tester::removeRehashRetrieve() {
    // creating first Cache object with variables initialized, personList stores the person objects which will be removed
    bool isThere = true;
    int capacity = 49;
    bool reHash = false;
    Person personList[capacity];
    Random RndID(MINID, MAXID);
    Random RndStr(MINSEARCH, MAXSEARCH);
    Cache cache(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // stores person object which will be removed
        personList[i] = dataObj;
        // inserting data in to the Cache object
        cache.insert(dataObj);

        // checking if person object has been inserted
        Person person = cache.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // removes objects from cache object and checks if requirements are satisfied
    int removes = 40;
    bool removed = true;
    for (int i = 0; i < removes; i++){
        // getting deletedRatio value
        float deleted = deletedRatio(cache.m_currNumDeleted+1, cache.m_currentSize);
        // removes object
        cache.remove(personList[i]);

        // checking if rehash has been triggered and checks if m_oldTable has been deallocated
        if (deleted > 0.8 && capacity-removes < capacity*0.25){
            if (cache.m_oldTable == nullptr) {
                reHash = true;
            }
        }

        // checking if person object has been removed
        Person person = cache.getPerson(personList[i].getKey(), personList[i].getID());
        if (!(person == EMPTY)){
            removed = false;
        }
    }
    if (isThere && reHash && cache.m_oldTable == nullptr && cache.m_currentSize == capacity-removes && removed) {
        cout << "REMOVE REHASH 1 PASSED" << endl;
    } else {
        cout << "REMOVE REHASH 1 FAILED" << endl;
    }



    // creating second Cache object with variables initialized, personList stores the person objects which will be removed
    isThere = true;
    capacity = 49;
    reHash = false;
    Person personList2[capacity];
    Cache cache2(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // stores person object which will be removed
        personList2[i] = dataObj;
        // inserting data in to the Cache object
        cache2.insert(dataObj);

        // checking if person object has been inserted
        Person person = cache2.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // removes objects from cache object and checks if requirements are satisfied
    removed = true;
    for (int i = 0; i < capacity; i++){
        // getting deletedRatio value
        float deleted = deletedRatio(cache2.m_currNumDeleted+1, cache2.m_currentSize);
        // removes object
        cache2.remove(personList2[i]);

        // checking if rehash has been triggered and checks if m_oldTable has been deallocated
        if (deleted > 0.8){
            if (cache.m_oldTable == nullptr) {
                reHash = true;
            }
        }

        // checking if person object has been removed
        Person person = cache.getPerson(personList2[i].getKey(), personList2[i].getID());
        if (!(person == EMPTY)){
            removed = false;
        }
    }
    if (isThere && reHash && cache2.m_oldTable == nullptr && removed && cache2.m_currentSize == 0) {
        cout << "REMOVE REHASH 2 PASSED" << endl;
    } else {
        cout << "REMOVE REHASH 2 FAILED" << endl;
    }
}

// helper function which tracks if there are duplicates in a cache object
int Tester::duplicate(const Cache & cache, Person person) {
    int counter = 0;
    bool duplicate = false;

    // counts if there are duplicates in the current table
    while(counter < cache.m_currentCap) {
        if (!duplicate) {
            duplicate = (person == cache.m_currentTable[counter]);
        }
        counter++;
    }

    // if there are not any duplicates in the current table and old table exists, checks if there are duplicates
    // in old table
    if (cache.m_oldTable && !duplicate){
        while(counter < cache.m_oldCap) {
            if (!duplicate) {
                duplicate = (person == cache.m_oldTable[counter]);
            }
            counter++;
        }
    }
    if (duplicate){
        return 1;
    }
    return 0;
}

float Tester::deletedRatio(int deleted, int size) {
    return float(deleted)/float(size);
}

// testing combination of insert and remove operations which trigger rehashing
void Tester::insertAndRemove() {
    // creating first Cache object with variables initialized
    bool isThere = true;
    int capacity = 60;
    bool reHash = false;
    Random RndID(MINID, MAXID);
    Random RndStr(MINSEARCH, MAXSEARCH);
    Cache cache(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache.m_currentSize+1, cache.m_currentCap);
        // inserting data in to the Cache object
        cache.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // getting two random ID's (where people objects exist in currentTable) and removing them from currenttable
    Random RndID2(0, 40);
    Random RndID3(0, 35);
    int index = RndID2.getRandNum();
    int index2 = RndID3.getRandNum();
    cache.remove(cache.m_currentTable[index]);
    cache.remove(cache.m_currentTable[index2]);

    // using getPerson to check if the people have been successfully removed from the cache object
    Person test = cache.getPerson(cache.m_currentTable[index].getKey(), cache.m_currentTable[index].getID());
    Person test2 = cache.getPerson(cache.m_currentTable[index2].getKey(), cache.m_currentTable[index2].getID());


    if (isThere && reHash && cache.m_oldTable == nullptr && cache.m_currentSize == capacity && test == EMPTY
    && test2 == EMPTY) {
        cout << "INSERT AND REMOVE 1 PASSED" << endl;
    } else {
        cout << "INSERT AND REMOVE 1 FAILED" << endl;
   }



    // creating second Cache object with variables initialized
    isThere = true;
    capacity = 60;
    reHash = false;
    Person personList[capacity];
    Cache cache2(MINPRIME, hashCode);

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // stores person object which will be removed
        personList[i] = dataObj;
        // getting lamba value
        float lambaVal = lamba(cache2.m_currentSize+1, cache2.m_currentCap);
        // inserting data in to the Cache object
        cache2.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache2.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache2.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // removes objects from cache object and checks if requirements are satisfied
    int removes = 48;
    bool removed = true;
    reHash = false;
    for (int i = 0; i < removes; i++){
        // getting deletedRatio value
        float deleted = deletedRatio(cache2.m_currNumDeleted+1, cache2.m_currentSize);
        // removes object
        cache2.remove(personList[i]);

        // checking if rehash has been triggered and checks if m_oldTable has been deallocated
        if (deleted > 0.8 && capacity-removes < capacity*0.25){
            if (cache2.m_oldTable == nullptr) {
                reHash = true;
            }
        }

        // checking if person object has been removed
        Person person = cache2.getPerson(personList[i].getKey(), personList[i].getID());
        if (!(person == EMPTY)){
            removed = false;
        }
    }

    if (isThere && reHash && cache2.m_oldTable == nullptr && cache2.m_currentSize == capacity-removes && removed) {
        cout << "INSERT AND REMOVE 2 PASSED" << endl;
    } else {
        cout << "INSERT AND REMOVE 2 FAILED" << endl;
    }



    // creating third Cache object with variables initialized
    Cache cache3(199, hashCode);
    capacity = 101;
    isThere = true;
    reHash = false;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache3.m_currentSize+1, cache3.m_currentCap);
        // inserting data in to the Cache object
        cache3.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache3.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache3.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // getting a random number (from the old table which is a valid live index) and removing it
    // checking if the removal was successful
    Random RndID4(110, 198);
    index = RndID4.getRandNum();
    cache3.remove(cache3.m_oldTable[index]);
    Person test3 = cache3.getPerson(cache3.m_oldTable[index].getKey(), cache3.m_oldTable[index].getID());
    int fourth = cache3.m_oldSize*0.25;

    if (isThere && reHash && cache3.m_oldTable != nullptr && cache3.m_currentSize == fourth*3
    && test3 == EMPTY) {
        cout << "INSERT AND REMOVE 3 PASSED" << endl;
    } else {
        cout << "INSERT AND REMOVE 3 FAILED" << endl;
    }



    // creating fourth Cache object with variables initialized
    Cache cache4(199, hashCode);
    capacity = 100;
    isThere = true;
    reHash = false;

    // builds cache object using insert
    for (int i = 0; i < capacity; i++) {
        // generating random data
        Person dataObj = Person(searchStr[RndStr.getRandNum()], RndID.getRandNum());
        // getting lamba value
        float lambaVal = lamba(cache4.m_currentSize+1, cache4.m_currentCap);
        // inserting data in to the Cache object
        cache4.insert(dataObj);

        // checking if rehash has been triggered
        if (lambaVal > 0.5 && cache4.m_oldTable != nullptr){
            reHash = true;
        }

        // checking if person object has been inserted
        Person person = cache4.getPerson(dataObj.getKey(), dataObj.getID());
        if (person == EMPTY){
            isThere = false;
        }
    }

    // performing 2 insert/remove operations which cancel each other out (adding and removing same node)
    // re adding a node again and checking if it has successfully been readded
    // this is so old table can be deallocated
    int removeAndInsert = 3;
    removed = true;
    Person aPerson(searchStr[RndStr.getRandNum()], RndID.getRandNum());
    Person aPerson2(searchStr[RndStr.getRandNum()], RndID.getRandNum());
    cache4.insert(aPerson);
    cache4.remove(aPerson);
    cache4.insert(aPerson2);
    cache4.remove(aPerson2);
    cache4.insert(aPerson);

    // checks if removal and insertion operation worked correctly
    if ((cache4.getPerson(aPerson.getKey(), aPerson.getID()) == EMPTY)
    && !(cache4.getPerson(aPerson2.getKey(), aPerson2.getID()) == EMPTY)){
        removed = false;
    }

    if (isThere && reHash && cache4.m_oldTable == nullptr && cache4.m_currentSize == capacity+removeAndInsert && removed
    && cache4.m_currNumDeleted == removeAndInsert-1) {
        cout << "INSERT AND REMOVE 4 PASSED" << endl;
    } else {
        cout << "INSERT AND REMOVE 4 FAILED" << endl;
    }
}


// testing constructor (this is not necessary because i would accidentally test it worked by trying to make a cache object
// with size 7 or something, but i guess it is good to test it?)
// tests if constructor is built correctly (with incorrect sizes/normal size)
void Tester::constructor() {
    // numbers i will use to input as the size when creating a cache object
    int test1 = 7;
    int test2 = 1000000000;
    int test3 = 102;
    int test4 = 211;

    // creating first Cache object and testing if size will stay as MINPRIME
    Cache cache(MINPRIME, hashCode);
    if (cache.m_currentCap != MINPRIME){
        cout << "CONSTRUCTOR NORMAL 1 FAILED" << endl;
    }else{
        cout << "CONSTRUCTOR NORMAL 1 PASSED" << endl;
    }



    // creating second Cache object and testing if size will stay as MAXPRIME
    Cache cache2(MAXPRIME, hashCode);
    if (cache2.m_currentCap != MAXPRIME){
        cout << "CONSTRUCTOR NORMAL 2 FAILED" << endl;
    }else{
        cout << "CONSTRUCTOR NORMAL 2 PASSED" << endl;
    }



    // creating third Cache object and testing if the size will stay the same since it is a prime number
    Cache cache6(test4, hashCode);
    if (cache6.m_currentCap != test4){
        cout << "CONSTRUCTOR NORMAL 3 FAILED" << endl;
    }else{
        cout << "CONSTRUCTOR NORMAL 3 PASSED" << endl;
    }



    // creating fourth Cache object less than MINPRIME and testing if the size becomes MINPRIME
    Cache cache3(test1, hashCode);
    if (cache3.m_currentCap != MINPRIME){
        cout << "CONSTRUCTOR ERROR 1 FAILED" << endl;
    }else{
        cout << "CONSTRUCTOR ERROR 1 PASSED" << endl;
    }



    // creating fifth Cache object greater than MAXPRIME and testing if the size becomes MAXPRIME
    Cache cache4(test2, hashCode);
    if (cache4.m_currentCap != MAXPRIME){
        cout << "CONSTRUCTOR ERROR 2 FAILED" << endl;
    }else{
        cout << "CONSTRUCTOR ERROR 2 PASSED" << endl;
    }



    // creating sixth Cache object with a number that is not prime and testing to see if the size of the cache object
    // becomes a valid size
    Cache cache5(test3, hashCode);
    int num = cache5.findNextPrime(test3);
    if (cache5.m_currentCap != num){
        cout << "CONSTRUCTOR ERROR 3 FAILED" << endl;
    }else{
        cout << "CONSTRUCTOR ERROR 3 PASSED" << endl;
    }
}
