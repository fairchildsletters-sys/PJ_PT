#ifndef HASHMAP_H
#define HASHMAP_H

#include "LinkedList.h"
#include "../utils/MathUtils.h"
#include "../utils/Hasher.h"
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

    // Internal hash gateway with compression
    size_t hash(const K &key, size_t targetCapacity = 0) const
    {
        size_t cap = (targetCapacity == 0) ? this->capacity : targetCapacity;
        return Hasher<K>::hash(key) % cap;
    }

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
                size_t newIndex = hash(current->data.key, newCapacity);
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

        size_t index = hash(key);
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

    // Removes (by key)
    bool remove(const K &key)
    {
        size_t index = hash(key);
        if (buckets[index].remove(HashEntry<K, V>(key, V())))
        {
            size--;
            return true;
        }
        return false;
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

    // =======================================================================
    // Inner Iterator class
    // =======================================================================
    class Iterator
    {
    private:
        const HashMap *map;
        size_t bucketIndex;
        Node<HashEntry<K, V>> *current;

        // Advances to the next non-empty bucket
        void advance()
        {
            while (current == nullptr && bucketIndex < map->capacity)
            {
                bucketIndex++;
                if (bucketIndex < map->capacity)
                {
                    current = map->buckets[bucketIndex].getHead();
                }
            }
        }

    public:
        Iterator(const HashMap *m, size_t index, Node<HashEntry<K, V>> *node)
            : map(m), bucketIndex(index), current(node)
        {
            if (current == nullptr)
            {
                advance();
            }
        }

        // Overloaded operators for iterator behavior
        HashEntry<K, V> &operator*() const
        {
            return current->data;
        }

        HashEntry<K, V> *operator->() const
        {
            return &(current->data);
        }

        Iterator &operator++()
        {
            if (current != nullptr)
            {
                current = current->next;
                if (current == nullptr)
                {
                    advance();
                }
            }
            return *this;
        }

        bool operator==(const Iterator &other) const
        {
            return current == other.current && bucketIndex == other.bucketIndex;
        }

        bool operator!=(const Iterator &other) const
        {
            return !(*this == other);
        }
    };

    // Returns the beginning iterator
    Iterator begin() const
    {
        return Iterator(this, 0, capacity > 0 ? buckets[0].getHead() : nullptr);
    }

    // Returns the ending iterator
    Iterator end() const
    {
        return Iterator(this, capacity, nullptr);
    }

    // Search by key (used for updating)
    Iterator find(const K &key) const
    {
        size_t index = hash(key);
        Node<HashEntry<K, V>> *current = buckets[index].getHead();

        while (current != nullptr)
        {
            if (current->data.key == key)
            {
                return Iterator(this, index, current);
            }
            current = current->next;
        }

        return end();
    }
};

#endif // HASHMAP_H