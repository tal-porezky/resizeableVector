#include <cassert>
#include <iostream>

template<typename T>
class Vector {
private:
    std::size_t m_size{};
    std::size_t m_capacity{ 1 };
    T* m_arr{};
    
    double MAX_LOAD_FACTOR { 0.75 };
    double MIN_LOAD_FACTOR { 0.25 };
    
    double getLoadFactor() const { return m_size / static_cast<double>(m_capacity); }
    bool passedMaxLoadFactor() const { return getLoadFactor() >= MAX_LOAD_FACTOR; }
    bool passedMinLoadFactor() const { return getLoadFactor() <= MIN_LOAD_FACTOR; }
    bool shouldResize() const { return passedMaxLoadFactor() || passedMinLoadFactor(); }

    void allocMemory() {
        m_arr = new T[m_capacity];
    }

    void deallocMemory() {
        delete[] m_arr;
        m_arr = nullptr;
    }
    
public:
    Vector() {
        allocMemory();
    }
    
    explicit Vector(std::size_t size) : m_capacity{ static_cast<size_t>(std::max(size * 2, 1ul)) } {
        assert(size >= 0);
        allocMemory();
    }
    
    Vector(const Vector& other) = delete;
    
    ~Vector() {
        deallocMemory();
    }

    T& operator[](std::size_t index) {
        assert(index >= 0);
        assert(index < m_size);
        return m_arr[index];
    }

    const T& operator[](std::size_t index) const {
        assert(index >= 0);
        assert(index < m_size);
        return m_arr[index];
    }
    
    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, const Vector<U>& vector);
    
    std::size_t size() const { return m_size; }
    std::size_t capacity() const { return m_capacity; }

    void nullify() {
        for (int ii{ 0 }; ii < m_size; ++ii) {
            m_arr[ii] = T();
        }
    }

    void push_back(T item) {
        ++m_size;
        if (passedMaxLoadFactor()){
            std::cout << "Increasing vector capacity. Current Capacity is: " << m_capacity << ". ";
            m_capacity = m_capacity * 2 ;
            std::cout << "New is " << m_capacity << "\n";
            T* newArr = new T[m_capacity];
            for (int ii{ 0 }; ii < m_size - 1; ++ii) {
                newArr[ii] = m_arr[ii];
            }
            deallocMemory();
            m_arr = newArr;
        }
        m_arr[m_size - 1] = item;
    }
    
};


template<typename T>
std::ostream& operator<<(std::ostream& out, const Vector<T>& vector) {
    out << "capacity: " << vector.m_capacity << " size: " << vector.m_size;
    out << " vector: < ";
    for (int ii{ 0 }; ii < static_cast<int>(vector.m_size); ++ii) {
        out << vector.m_arr[ii] << " ";
    }
    out << ">\n";
    return out;
}

int main() {
    Vector<int> vec(2);
    vec.push_back(0);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    std::cout << vec;
    return 0;
}