#include <string>
#include <utility>
#include <memory>
#include <complex>
using namespace std;

template <class TYPE>
class Table{
public:
	Table(){}
	virtual bool update(const string& key, const TYPE& value)=0;
	virtual bool remove(const string& key)=0;
	virtual bool find(const string& key, TYPE& value)=0;
	virtual ~Table(){}
};

template <class TYPE>
class SimpleTable:public Table<TYPE>{

	struct Record{
		TYPE data_;
		string key_;
		Record(const string& key, const TYPE& data){
			key_=key;
			data_=data;
		}

	};

	Record** records_;   //the table
	int max_;           //capacity of the array
	int size_;          //current number of records held
	int search(const string& key);
	void sort();
	void grow();
public:
	SimpleTable(int maxExpected);
	SimpleTable(const SimpleTable& other);
	SimpleTable(SimpleTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const SimpleTable& operator=(const SimpleTable& other);
	virtual const SimpleTable& operator=(SimpleTable&& other);
	virtual ~SimpleTable();

        int binarySearch(Record** record, const string& key, int first, int last);
        
};

// pass a key, us binarySearch() function to find the index
//returns index of where key is found. if not found return -1
template <class TYPE>
int SimpleTable<TYPE>::search(const string& key){   
    return binarySearch(records_ , key, 0 , size_-1);
}


//sort the according to key in table
//because every time  table insert a new record table will be sorted, the table is 
// sorted table
// so this sort() function actually is insert last record to the proper location
template <class TYPE>
void SimpleTable<TYPE>::sort(){
    if(size_ == 0 || size_ == 1 ) return;
    
   int unsortedItemIndex = size_-1;
   for (int i = 0 ; i < size_ - 1; i++) {


        if (records_[i]->key_ >= records_[unsortedItemIndex]->key_) {           

            Record* tmp = records_[unsortedItemIndex];
            for(int j = size_-2; j>= i ; j--){
                records_[j+1] = records_[j];
            }
            records_[i] = tmp;
           
            break;            
           
        }
        
   }
}

//grow the array by double the original max expected size
template <class TYPE>
void SimpleTable<TYPE>::grow(){
	Record** newArray=new Record*[2*max_];
	max_ = 2*max_;
	for(int i=0;i<size_;i++){
		newArray[i]=records_[i];
	}
	delete [] records_;
	records_=newArray;
}

// construtor with max expected size parameter
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(int maxExpected): Table<TYPE>(){

    records_ = new Record*[maxExpected];
    max_ = maxExpected;
    size_ = 0;
}

//copy constructor
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(const SimpleTable<TYPE>& other){

    records_ = new Record*[other.max_];
    max_ = other.max_;
    size_ = other.size_;
    for (int i = 0; i < other.size_; i++) {
        Record* tmp = new Record(other.records_[i]->key_, other.records_[i]->data_);
        records_[i] = tmp;
    }
}

//move constructor
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>&& other){
	size_=other.size_;
	max_=other.max_;
	records_=other.records_;
	other.records_=nullptr;
	other.size_=0;
	other.max_=0;
}

// first try to find the key ,if key is exist in table, update the value
// if not exist and if table is full , grow table to its double size, then append
// the key:value pair at the end , then sort
// if the key not exist and the table is not full, append the key:value pair to the 
// table's end, then sort
template <class TYPE>
bool SimpleTable<TYPE>::update(const string& key, const TYPE& value) {
    if (size_ == 0) {
        records_[size_] = new Record(key, value);
        size_++;
        return true;
    } 
    else {
        int idx = search(key);
        if (idx == -1) {
            if (size_ == max_) {
                grow();
            }
            records_[size_++] = new Record(key, value);
            sort();
        } 
        else {
            records_[idx]->data_ = value;
        }
        return true;
    }

}


//remove key:value pair if found key in the table
template <class TYPE>
bool SimpleTable<TYPE>::remove(const string& key) {
    int idx = search(key);
    if (idx != -1) {
        delete records_[idx];
        for (int i = idx; i < size_ - 1; i++) {
            records_[i] = records_[i + 1];
        }
        size_--;
        return true;
    } 
    else {
        return false;
    }
}

//find the key in the table, fetch back the the value by assign it to a reference
//param: key is the key to be seached;  value is a TYPE reference for fetch back value
// return true if found key, and assign data to reference value
template <class TYPE>
bool SimpleTable<TYPE>::find(const string& key, TYPE& value){
    if(size_ == 0){
        return false;
    }
    else{
	int idx=search(key);
	if(idx==-1)
		return false;
	else{
		value=records_[idx]->data_;
		return true;
	}
    }
}

// assignment operator
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(const SimpleTable<TYPE>& other) {
    if (this != &other) {
        if (records_) {
            int sz = size_;
            for (int i = 0; i < sz; i++) {
                delete records_[i];
            }
            delete [] records_;
        }
        records_ = new Record*[other.max_];
        max_ = other.max_;
        size_ = other.size_;
        for (int i = 0; i < other.size_; i++) {
            Record* tmp = new Record(other.records_[i]->key_, other.records_[i]->data_);
            records_[i] = tmp;
        }

    }
    return *this;
}


//move assignment operator
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>&& other) {

    if (this != &other) {
        if (records_) {
            int sz = size_;
            for (int i = 0; i < sz; i++) {
                delete records_[i];
            }
            delete [] records_;
        }
        records_ = other.records_;
        max_ = other.max_;
        size_ = other.size_;
        other.records_ = nullptr;
        other.size_ = 0;
        other.max_ = 0;
    }
    return *this;
}

// destructor
template <class TYPE>
SimpleTable<TYPE>::~SimpleTable() {
    if (records_) {
        int sz = size_;
        for (int i = 0; i < sz; i++) {
            remove(records_[0]->key_);
        }
        delete [] records_;
    }
}

// binary search 
// record : arrary of record
// key: searched key
// first: search start index
// last: search end index
// return index of the key if key is found, otherwise return -1
template <class TYPE>
int SimpleTable<TYPE>::binarySearch(Record** record, const string& key, int first, int last) {
    int mid;
    if (last == first) {
        if (record[first]->key_ == key) {
            return first;
        } else return -1;
    } else if (first > last) {
        if (first >= size_) {
            if (record[last]->key_ == key) {
                return last;
            } else return -1;
        } else {
            if (record[first]->key_ == key) {
                return first;
            } else return -1;
        }

    } else {
        mid = (first + last) / 2;
        if (record[mid]->key_ == key) {
            return mid;
        } else if (record[mid]->key_ < key) {
            //if(mid-1 < 0) return -1;
            ///else{
            binarySearch(record, key, mid + 1, last);
            //}
        }
        else {
            binarySearch(record, key, first, mid - 1);

        }

    }

}

/*
// return the postion that new record should be inserted after
template <class TYPE>
int SimpleTable<TYPE>::binaryInsert(const string& key, const TYPE& value,int first, int last){
    
    Record** record = records_;
    int mid = (first + last) / 2;
    
    if (last == first) {
        if (key > record[first]->key_) return first;
        else return first-1;
        
    } else if (first > last) {
        
        if (key > record[last]->key_ ) {
            return last;
        } 
        else return last -1;

    } else {
        
        if (key >= record[mid]->key_ && key <= record[mid+1]) {
            return mid;
        } 
        else if (key >= record[mid]->key_ ) {
            //if(mid-1 < 0) return -1;
            ///else{
            binarySearch(record, key, mid + 1, last);
            //}
        }
        else {
            binarySearch(record, key, first, mid);

        }

    }

}
*/







//########  Part C  ######//


template <class TYPE>
class LPTable:public Table<TYPE>{
    struct Record{
        TYPE data_;
        string key_;
        Record(const string& key, const TYPE& data){
                key_=key;
                data_=data;
        }
    };

    Record** records_;   //the table
    int max_;           //max allowed record in the table
    int realSize_;    // real allocated size of records_
    static const int  factor = 2;  // factor used to calculate realSize_
    int size_;          //current number of records held
    


    
    //helper function 
    bool isEmpty();
    void clearTable();
    int prevousIndex(int index);
    int nextIndex(int index);
    bool isEmptySpotBetweenHashIndexAndCurrent(int emptySpotIndex, int hashInex);
    void initializeTableElementToNullptr();
    void hashtableClusterAdjuster(int emptySpotIndex);
    

public:
    
    LPTable(int maxExpected);
    LPTable(const LPTable& other);
    LPTable(LPTable&& other);
    virtual bool update(const string& key, const TYPE& value);
    virtual bool remove(const string& key);
    virtual bool find(const string& key, TYPE& value);
    virtual const LPTable& operator=(const LPTable& other);
    virtual const LPTable& operator=(LPTable&& other);
    virtual ~LPTable();

    

    // helper
    int search(const string& key);
    string getKey(int i);
    int myHash_fn(const string& key);
};


//constructor
//param: maxExpected - maximum items allowed 
template <class TYPE>
LPTable<TYPE>::LPTable(int maxExpected): Table<TYPE>(){
    
    
    max_ = maxExpected;
    realSize_  = (int) factor*max_;
    records_ = new Record*[realSize_];
    
    //initialize the table element pointer to nullptr
    initializeTableElementToNullptr();
    
    size_ = 0;
    
}

// Copy constructor: using assignment operator
template <class TYPE>
LPTable<TYPE>::LPTable(const LPTable<TYPE>& other){
    
    // initialize the table first
        max_ = 0;
        realSize_ = 0;
        records_ = nullptr;
        size_ = 0;
    
    // Use assignment operator        
    *this = other ;   
}


//move constructor
template <class TYPE>
LPTable<TYPE>::LPTable(LPTable<TYPE>&& other){
    
     // initialize the table first
        max_ = 0;
        realSize_ = 0;
        records_ = nullptr;
        size_ = 0;
    
    // Use move assignment operator   
        *this = move(other);

}


// add or update key value pair
// praram: key - is a string , value is a value of type TYPE
// return true if record is updated or is added, otherwise return false
template <class TYPE>
bool LPTable<TYPE>::update(const string& key, const TYPE& value){
    
    // Declare a hash function variable
    hash<string> hash_fn;
    
    // calculate the index for the new records
    size_t idx = myHash_fn(key);
    

    // if table is full to see if the key is exist same key with the passed key
    // if exist, update the value
    if (size_ == max_) {        
        
        
        size_t loopIndex = idx;
        if(records_[loopIndex] == nullptr ){
            return false;
        }
        else {            
            if (records_[loopIndex]->key_ == key) {
                records_[loopIndex]->data_ = value;
                loopIndex = -1;
            }
            else {
                if (loopIndex == realSize_ - 1) loopIndex = 0;
                else loopIndex = loopIndex + 1;
            }
        }
        
        
        while ( loopIndex != -1 && loopIndex != idx) {
            
            if(records_[loopIndex]== nullptr){
                return false;
            }
            else{
                if (records_[loopIndex]->key_ == key) {
                    records_[loopIndex]->data_ = value;
                    loopIndex = -1;
                } else {
                    loopIndex = nextIndex(loopIndex);
                }
            }          
        }
        if(loopIndex == -1)return true;
        else return false;
     
    }
    else { //!!! table is not full      

        // construct a new record
        Record* newRec = new Record(key, value);        

        // try to insert new element into the table: 
        // If the table already has a record with a matching key, 
        // the record's value is replaced by the value passed to this function. 
        size_t index = idx;
        

        if (records_[index] == nullptr) {
            records_[index] = newRec;
            size_++;
            index = -1;
            
        } 
        else {
            if(records_[index]->key_ == key ) {
                records_[index]->data_ = value;
                index = -1;                
            }else{
                index = nextIndex(index);
            }
            
        }
        
        while (index != -1 && index != idx) {

            //if the key matching the existing element's key, replace the existing value to
            // to the passed value
            
            if (records_[index] != nullptr ){
                
               if(records_[index]->key_ == key ) {
                records_[index]->data_ = value;
                index = -1;                
               }
               
            }
            else{// spot is empty, add new record
                records_[index] = newRec;
                size_++;
                index = -1;
                
            }

            if (index != -1) {
                // if index is beyond the end of array, 
                // start index from beginning of the array until it reached the 
                // the start point of the while loop: idx
                index = nextIndex(index);
            }
            
        }
        if(index == -1) return true;
        else return false;        
    }
}


// remove the item that with same key in the table and adjust the cluster that removed
// item located in
// param: key-- string
// return true if key is found and removed
template <class TYPE>
bool LPTable<TYPE>::remove(const string& key) {   
    
    
    int matchedIndex = search(key);
    
    if(matchedIndex == -1) return false;
    else{
        // remove
        delete records_[matchedIndex];
        records_[matchedIndex] = nullptr;
        size_--;



        // Declare a hash function variable by using STL
        hash<string> hash_fn;

        
        // calculate the removed item's hash index 
        int adjustingHashIndex = myHash_fn(key);
        
        int emptySpotIndex = matchedIndex;  
        
        hashtableClusterAdjuster(emptySpotIndex);

        return true;
        
    }   
        
}



//give a key and value reference, try to find same key in the table and 
// assign the found item's value to value
//param: key a string, value the reference of TYPE
// return true if find same key 
template <class TYPE>
bool LPTable<TYPE>::find(const string& key, TYPE& value) {
    
    bool result = false;
    
    // Declare a hash function variable by using STL
    hash<string> hash_fn;
    
    // calculate the index for the new records
    size_t idx = myHash_fn(key);

    
    size_t index = idx;
//    if(records_[index] == nullptr){
//        result = ;
//    }
    while ( result != true && records_[index] != nullptr) {

        //if the key matching the existing element's key, replace the existing value to
        // to the passed value
        if (records_[index]->key_ == key) {

            value = records_[index]->data_;
            result = true;
        }
        
        index = nextIndex(index);
        

    }

    return result;
}

// Assignment operator
template <class TYPE>
const LPTable<TYPE>& LPTable<TYPE>::operator=(const LPTable<TYPE>& other){
    
    // Check if it is a self assignment first
    if(this != &other){
        
        // clear current table if it is not empty and its capacity is different from other
        if(!isEmpty()){
            
            // delete current table's records
            clearTable();
        }
        
         
        // Delete current table
        delete [] records_;      
        
        
        // configure the current table
        max_ = other.max_;
        realSize_ = factor*max_;
        records_ = new Record*[realSize_];
        initializeTableElementToNullptr();
        size_ = other.size_;
        
        
        //copy records to the current table
        for(int i = 0; i< realSize_; i++){
            if(other.records_[i] != nullptr){
                Record* tmp = new Record(other.records_[i]->key_, other.records_[i]->data_);
                this->records_[i] = tmp;
            }           
        }
    }
    
    return *this;

}


//move assignment operator
template <class TYPE>
const LPTable<TYPE>& LPTable<TYPE>::operator=(LPTable<TYPE>&& other){
    
    // Check if it is a self assignment, if it is self assignment do nothing
    if (this != &other) {
        if (!isEmpty()) { // if this list is not empty, empty it first

            //clear the table
            clearTable();

        }

        delete [] records_;


        // configure the current table
        max_ = other.max_;
        realSize_ = factor*max_;
        records_ = new Record*[realSize_];
        initializeTableElementToNullptr();
        size_ = other.size_;


        this->records_ = other.records_;
        other.records_ = nullptr;
        other.size_ = 0;
        other.max_ = 0;
        other.realSize_ = 0;




    }// do nothing for self assignment

    return *this;

}


//destructor
template <class TYPE>
LPTable<TYPE>::~LPTable(){
    clearTable();
    delete [] records_;
}


// check if table is empty
// return true if is empty 
template <class TYPE>
bool LPTable<TYPE>::isEmpty(){
    return size_ == 0;
}


// param: key is the key being searched
// return -1 if not found, otherwise return  index of matching record
template <class TYPE>
int LPTable<TYPE>::search(const string& key){
    bool result = false;
    
    size_t matchedIndex = -1;
    
    // Declare a hash function variable by using STL
    hash<string> hash_fn;
    
    // calculate the index for the new records
    size_t idx = myHash_fn(key);

    
    size_t index = idx;
    if(records_[index] == nullptr){
        return matchedIndex;
    }
    while ( matchedIndex == -1) {

        //if the key matching the existing element's key, replace the existing value to
        // to the passed value
        if (records_[index]->key_ == key) {
            matchedIndex = index;
        }
              
        index = nextIndex(index);
        
        if(records_[index] == nullptr) break;
    }

    return matchedIndex;  
}

//clear all table's item to nullptr
template <class TYPE>
void LPTable<TYPE>::clearTable(){
    for(int i = 0; i< realSize_; i++){
        delete this->records_[i];
        this->records_[i] = nullptr;        
    }           
}

// pass a index in the table, return previous index in the cluster being adjusted
template <class TYPE>
int LPTable<TYPE>::prevousIndex(int index){
    if (index == 0) index = realSize_ - 1;
    else index = index - 1;
    return index;
}

// pass a index in the table, return next index in the cluster being adjusted
template <class TYPE>
int LPTable<TYPE>::nextIndex(int index){
    if (index == realSize_ - 1) index = 0;
    else index = index + 1;
    return index;
}





// helper funtion: checked if the item followed the empty spot index is locate between
// the hash index of the current key and current index

// param: emptySpotIndex the index of empty spot;  hashIndex is the current key's hash index
// 
// return true if the Item's hash index is located between
// the hash index of the current key and current index
template <class TYPE>
bool LPTable<TYPE>::isEmptySpotBetweenHashIndexAndCurrent(int emptySpotIndex, int hashIndex){
    
    bool result = false;
    
    // looking for startIndexOfCluster
    if(hashIndex == emptySpotIndex ) {        
        result = true;        
    }
    else {
        int index = prevousIndex(emptySpotIndex);
        while (records_[index] != nullptr && !result) {
            if (hashIndex == index) result = true;
            else index = prevousIndex(index);
        }        
    }
    
    return result;  
    
    
}


// initialize empty table element to nullptr
template <class TYPE>
void LPTable<TYPE>::initializeTableElementToNullptr(){
    for(int i= 0; i< realSize_; i++){
        records_[i] = nullptr;
    }
}



// adjust hashtable  after remove item from table
// param: emptySpotIndex- the index of the removed item
template <class TYPE>
void LPTable<TYPE>::hashtableClusterAdjuster(int emptySpotIndex){
    // Declare a hash function variable
    hash<string> hash_fn;
    
    int initialEmptySpotIndex = emptySpotIndex;

    int i = nextIndex(emptySpotIndex);
    while (records_[i] != nullptr) {
        
        string key = records_[i]->key_;

        int hashInex = myHash_fn(key);
        
        if (isEmptySpotBetweenHashIndexAndCurrent(emptySpotIndex, hashInex)) {

            records_[emptySpotIndex] = records_[i];
            records_[i] = nullptr;
            emptySpotIndex = i;
        }


        i = nextIndex(i);

        // if loop goes beyond endOfCluster
        if (records_[i] == nullptr) {
            // empty spot is not moved 
            if (initialEmptySpotIndex == emptySpotIndex) {
                return;
            }
            // empty spot is removed
            else {
                initialEmptySpotIndex = emptySpotIndex;

                i = nextIndex(emptySpotIndex);
            }
        }
    }
    
        
    
    
}



// getter
// prarm: i is the index
// return key in the index of i

template <class TYPE>
string LPTable<TYPE>::getKey(int i) {
    if (records_[i] != nullptr) return records_[i]->key_;
    else return nullptr;
}

// customized hash function
//param: key the key string
//return hash index of the key
template <class TYPE>
int LPTable<TYPE>::myHash_fn(const string& key) {

    // Declare a hash function variable
    hash<string> hash_fn;

    // calculate the index for the new records
    return hash_fn(key) % realSize_;
}

