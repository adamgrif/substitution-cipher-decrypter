// MyHash.h

// Skeleton for the MyHash class template.  You must implement the first seven
// member functions; we have implemented the eighth.
#ifndef MYHASH_H
#define MYHASH_H

template<typename KeyType, typename ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5);
    ~MyHash();
    void reset();
    void associate(const KeyType& key, const ValueType& value);
    int getNumItems() const;
    double getLoadFactor() const;

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    MyHash(const MyHash&) = delete;
    MyHash& operator=(const MyHash&) = delete;
    
private:
    
    struct Node
    {
        // No need for default constructor as the MyHash constructor initializers all pointers to nullptr
        Node(const KeyType& k, const ValueType& v){
            key = k;
            value = v;
            next = nullptr;
        }
        KeyType key;
        ValueType value;
        Node* next;
    };
    
    Node** m_table;
    
    double m_load;
    int m_nBuckets;
    int m_nItems;
    
    
    void clear();
    bool exceedsLoad(int items);
    void increaseTable();
    void insertOrUpdate(const KeyType &key, const ValueType &value, unsigned int b);
    
};


template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor)
: m_table(new Node*[100]()), m_nBuckets(100), m_nItems(0), m_load(maxLoadFactor)
{
    // Initialize the hash table array to 100 Node pointers all set to nullptr, bucket count to 100,
    // item count to 0, and the load factor to what was passed in or 0.5
    
    // Correct the load factor if it is 0 or less or more than 2
    if (maxLoadFactor <= 0.0){
        m_load = 0.5;
    } else if (maxLoadFactor > 2.0){
        m_load = 2.0;
    }
}


template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::~MyHash()
{
    clear();
}


template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::reset()
{
    // Delete everything
    clear();
    
    // Set the array to a new collection of Node* initialized to nullptr
    m_table = new Node*[100]();
    m_nBuckets = 100;
    m_nItems = 0;
}


template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType &key, const ValueType &value)
{
    unsigned int hash(const KeyType& k);
    // Calculate bucket number
    unsigned int bucket = hash(key) % m_nBuckets;
    
    // Then insert or update the key/value pair.  Increasing table size is checked within the insertOrUpdate function
    insertOrUpdate(key, value, bucket);
}


template<typename KeyType, typename ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const
{
    return m_nItems;
}


template<typename KeyType, typename ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const
{
    return static_cast<double>(m_nItems) / m_nBuckets;
}


template<typename KeyType, typename ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned int hash(const KeyType& k);
    // Calculate the bucket number for the given key
    unsigned int bucket = hash(key) % m_nBuckets;
    
    // Search for the given key in that bucket
    for (Node* p = m_table[bucket]; p != nullptr; p = p->next){
        if (p->key == key){
            // If the key is found, return a pointer to the corresponding value
            return &(p->value);
        }
    }
    
    // If not found, return nullptr
    return nullptr;
}




template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::clear()
{
    for (int i = 0; i < m_nBuckets; i++){
        // For every bucket
        for (Node* p = m_table[i]; p != nullptr; ){
            // For every node in the bucket
            Node* t = p;
            p = p->next;
            // Delete that node
            delete t;
        }
    }
    delete [] m_table;
}


template<typename KeyType, typename ValueType>
bool MyHash<KeyType, ValueType>::exceedsLoad(int items)
{
    return (static_cast<double>(items) / m_nBuckets > m_load);
}


template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::increaseTable()
{
    // Hold the old table temporarily
    Node** temp = m_table;
    
    // Set the table to a new array of twice the size with Node* all initialized to nullptr
    m_table = new Node*[m_nBuckets * 2]();
    
    // Inserting all the items will re-adjust item count, so set to 0 here
    m_nItems = 0;
    
    // Insert all the old Nodes into the new table
    for (int i = 0; i < m_nBuckets; i++){
        for (Node* p = temp[i]; p != nullptr; p = p->next){
            unsigned int hash(const KeyType& k);
            unsigned int bucket = hash(p->key) % (m_nBuckets * 2);
            insertOrUpdate(p->key, p->value, bucket);
        }
    }
    
    // Delete all the old Nodes
    for (int i = 0; i < m_nBuckets; i++){
        for (Node* p = temp[i]; p != nullptr; ){
            Node* t = p;
            p = p->next;
            delete t;
        }
    }
    delete [] temp;
    
    // Number of buckets has been doubled
    m_nBuckets *= 2;
    

}


template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::insertOrUpdate(const KeyType &key, const ValueType &value, unsigned int b)
{
    // If the key is already in the table, updates the value.
    // Otherwise, inserts new Node with key/value pair at the end of the linked list in the bucket.
    // Will increase table size if adding an item will exceed the load factor
    
    Node* p = m_table[b];
    
    // If there are no Nodes in the bucket
    if (p == nullptr){
        // If adding the item will exceed the load factor, increase the table size
        if (exceedsLoad(m_nItems + 1)){
            increaseTable();
        }
        
        // Add a new Node with the key/value pair and set that bucket to point to this Node
        Node* n = new Node(key, value);
        m_table[b] = n;
        // Increase number of items by 1
        m_nItems++;
        return;
    }

    while (p->next != nullptr){
        // Go through until the last item in the list
        // If the key is already there, update the value
        if (p->key == key){
            p->value = value;
            return;
        }
        p = p->next;
    }
    
    // Check the last Node in the list to see if the key is already there, and if so, update the value
    if (p->key == key){
        p->value = value;
        return;
    }


    // If adding the item will exceed the load factor, increase the table size
    if (exceedsLoad(m_nItems + 1)){
        increaseTable();
    }
    
    // Then add the new key/value pair at the end of the list in the bucket
    Node* n = new Node(key, value);
    p->next = n;
    
    // Increase number of items by 1
    m_nItems++;
}

#endif // MYHASH_H
