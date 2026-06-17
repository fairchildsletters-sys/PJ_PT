#ifndef HASHMAP_H
#define HASHMAP_H

#include "LinkedList.h"
#include "MathUtils.h"
#include "Hasher.h"
#include <string>

template <typename K, typename V>
struct HashEntry
{
    K key;
    V value;

    HashEntry() : key(K()), value(V()) {}
    HashEntry(K k, V v) : key(k), value(v) {}

    // Overloaded equality operator comparing only keys
    bool operator==(const HashEntry &other) const
    {
        return this->key == other.key;
    }
};

template <typename K, typename V>
class HashMap
{
private:
    LinkedList<HashEntry<K, V>> *buckets;
    size_t size;
    size_t capacity;

    // Rehashes when load factor is exceeded
    void rehash()
    {
        size_t newCapacity = MathUtils::getNextPrime(capacity * 2);
        LinkedList<HashEntry<K, V>> *newBuckets = new LinkedList<HashEntry<K, V>>[newCapacity];

        for (size_t i = 0; i < capacity; ++i)
        {
            Node<HashEntry<K, V>> *current = buckets[i].getHead();
            while (current != nullptr)
            {
                size_t newIndex = Hasher<K>::hash(current->data.key, newCapacity);
                newBuckets[newIndex].insertAtHead(current->data);
                current = current->next;
            }
        }
        delete[] buckets;
        buckets = newBuckets;
        capacity = newCapacity;
    }

public:
    HashMap(size_t initialCapacity = 10007) : size(0), capacity(initialCapacity)
    {
        buckets = new LinkedList<HashEntry<K, V>>[capacity];
    }

    ~HashMap()
    {
        delete[] buckets; // delete operator
    }

    void clear()
    {
        for (size_t i = 0; i < capacity; ++i)
        {
            buckets[i].clear();
        }
        size = 0;
    }

    // Disable copy constructor and assignment operator
    HashMap(const HashMap &) = delete;
    HashMap &operator=(const HashMap &) = delete;

    // Inserts (NEW)
    bool add(const K &key, V value)
    {
        if ((float)size / capacity >= 0.75f)
        {
            rehash();
        }

        size_t index = Hasher<K>::hash(key, capacity);
        Node<HashEntry<K, V>> *current = buckets[index].getHead();

        // Check if key already exists
        while (current != nullptr)
        {
            if (current->data.key == key)
            {
                return false;
            }
            current = current->next;
        }

        buckets[index].insertAtHead(HashEntry<K, V>(key, value));
        size++;
        return true;
    }

    // Updates (EXISTING)
    bool update(const K &key, V value)
    {
        size_t index = Hasher<K>::hash(key, capacity);
        Node<HashEntry<K, V>> *current = buckets[index].getHead();

        // Search for the existing key to update
        while (current != nullptr)
        {
            if (current->data.key == key)
            {
                current->data.value = value;
                return true;
            }
            current = current->next;
        }

        return false;
    }

    // Removes (by key)
    bool remove(const K &key)
    {
        size_t index = Hasher<K>::hash(key, capacity);
        if (buckets[index].remove(HashEntry<K, V>(key, V())))
        {
            size--;
            return true;
        }
        return false;
    }

    // Gets value by key
    V get(const K &key) const
    {
        size_t index = Hasher<K>::hash(key, capacity);
        Node<HashEntry<K, V>> *current = buckets[index].getHead();

        while (current != nullptr)
        {
            if (current->data.key == key)
            {
                return current->data.value;
            }
            current = current->next;
        }
        return V();
    }

    size_t getSize() const
    {
        return size;
    }

    size_t getCapacity() const
    {
        return capacity;
    }

    bool isEmpty() const
    {
        return size == 0;
    }
};

#endif // HASHMAP_H