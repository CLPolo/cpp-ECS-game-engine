/**
 * @file ComponentStorage.h
 * @brief The storage container used for each individual component type: The Registry.
 *
 * @date 2025-10-27
 */

#pragma once

#include <vector>
#include <cassert>

namespace ECSEngine {

template <typename T> class ComponentStorage {
public: // why are these public????
    std::vector<T> mStorage;
    std::vector<bool> mValid; // Quickly track if a component is valid
    std::vector<size_t> mFreeList; // Avoid scanning for free slots
    size_t mCount = 0;

    ComponentStorage() = default;
    ~ComponentStorage() = default;

    // We can do perfect forwarding here, but this is what I had

    // Store By Copy
    size_t store(const T& value)
    {

        size_t id;

        if (!mFreeList.empty()) {

            id = mFreeList.back();
            mFreeList.pop_back();

            mStorage[id] = value;
            mValid[id] = true;
        } else {

            id = mStorage.size();
            mStorage.push_back(value);
            mValid.push_back(true);
        }

        mCount += 1;

        return id;
    }

    // Store by Move
    size_t store(T&& value)
    {

        size_t id;

        if (!mFreeList.empty()) {

            id = mFreeList.back();
            mFreeList.pop_back();

            mStorage[id] = std::move(value);
            mValid[id] = true;
        } else {

            id = mStorage.size();
            mStorage.push_back(std::move(value));
            mValid.push_back(true);
        }

        mCount += 1;

        return id;
    }

    void remove(size_t id)
    {
        assert(valid(id));

        mValid[id] = false;
        mStorage[id] = T {};
        mFreeList.push_back(id);
        mCount -= 1;
    }

    T& operator[](size_t id)
    {

        assert(valid(id));
        return mStorage[id];
    }

    const T& operator[](size_t id) const { return mStorage[id]; }

    bool valid(size_t id) const { return id < mValid.size() && mValid[id]; }

    size_t size() const
    {

        return mCount; // or mStorage.size() - mFreeList.size()
    }

    void reserve(size_t capacity)
    {
        mStorage.reserve(capacity);
        mValid.reserve(capacity);
    }
};

}
