#ifndef POOL_HPP
#define POOL_HPP

#include <vector>
#include <iostream>

class IPool {
    public:
        virtual ~IPool() = default;
};

inline std::vector<IPool*> pools;

template <typename TComponent>
class Pool : public IPool {
    private:
        std::vector<TComponent> data;
    
    public:
        Pool(int size = 1000) {
            data.resize(size); // resize data vector to "size"
        };
    
    virtual ~Pool() = default;

    bool isEmpty() const {
        return data.empty();
    };

    int getSize() const {
        return static_cast<int>(data.size());
    }

    void resize(int new_size) {
        data.resize(new_size);
    }

    void clear() {
        data.clear();
    }

    void add(TComponent object) {
        data.push_back(object);
    }

    void set(unsigned int index, TComponent&& object) {
        data[index] = std::move(object);
    }

    TComponent& get(unsigned int index) {
        return static_cast<TComponent&>(data[index]);
    }

    TComponent& operator[](unsigned int index) {
       return get(index);
    }
};


#endif // POOL_HPP