#include <cassert>
#include <iostream>

template<typename T>
class Vector {
private:
    std::size_t m_size{};
    std::size_t m_capacity{ 1 };
    T* m_data{};
    
    static constexpr double MAX_LOAD_FACTOR { 0.75 };
    static constexpr std::size_t RESIZE_FACTOR { 2ul };

    double getLoadFactor() const { return m_size / static_cast<double>(m_capacity); }
    bool passedMaxLoadFactor() const { return getLoadFactor() >= MAX_LOAD_FACTOR; }

    void init() {
        if (m_capacity) {
            m_data = reinterpret_cast<T*>(malloc( m_capacity * sizeof(T) ));  // This line allocates enough space for m_capaicty objects of type T to be initialize.
            new (m_data) T[m_size] {};  // This line initializes m_size objects of type T. Note that if m_size < m_capacity, we're not using all the allocated space from the line above.
        }
    }

    void destruct() {
        if (m_data)
        {
            // Since we constructed with two stages allocating memory and the initilizing the objects) we need two stages to destruct them
            for (std::size_t ii{ 0 }; ii < m_size; ++ii) { //std::destroy(m_data, m_data + m_size);  // First, we need to destroy all the objects in m_data using their destructor
                m_data[ii].~T();
            }
            free(m_data);  // Second, we need to free the allocated memory since we used malloc
            m_data = nullptr;
        }
    }

    class Iterator {
    private:
        Vector& m_vector;
        std::size_t m_curr_idx{};
    public:
        Iterator(Vector& vector) : Iterator::Iterator{ vector, 0 } {}
        Iterator(Vector& vector, std::size_t index) : m_vector{ vector }, m_curr_idx{ index } {}

        Iterator& operator++() {
            ++m_curr_idx;
            return *this;
        }

        T& operator*() const {
            return m_vector[m_curr_idx];
        }

        bool operator!=(Iterator& other) const {
            return ((&m_vector != &(other.m_vector)) || (m_curr_idx != other.m_curr_idx));
        }
    };

    class ConstIterator {
    private:
        const Vector& m_vector;
        std::size_t m_curr_idx{};
    public:
        ConstIterator(const Vector& vector) : ConstIterator::ConstIterator{ vector, 0 } {}
        ConstIterator(const Vector& vector, std::size_t index) : m_vector{ vector }, m_curr_idx{ index } {}

        ConstIterator& operator++() {
            ++m_curr_idx;
            return *this;
        }

        const T& operator*() const {
            return m_vector[m_curr_idx];
        }

        bool operator!=(const ConstIterator& other) const {
            return ((&m_vector != &(other.m_vector)) || (m_curr_idx != other.m_curr_idx));
        }
    };
    
public:
    Vector() {
        init();
    }
    
    explicit Vector(std::size_t size) : m_capacity{ size * RESIZE_FACTOR } {
        init();
    }
    
    Vector(const Vector& other) : m_capacity{ other.m_capacity } {
        init();
        for (auto elem : other) {
            push_back(elem);
        }
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        destruct();
        m_capacity = other.m_capacity;
        init();
        m_size = 0;
        for (auto elem : other) {
            push_back(elem);
        }
        return *this;
    }
    
    ~Vector() {
        destruct();
    }

    Iterator begin() {
        return { *this };
    }

    Iterator end() {
        return { *this, m_size };
    }

    ConstIterator begin() const {
        return { *this };
    }

    ConstIterator end() const {
        return { *this, m_size };
    }

    ConstIterator cbegin() const {
        return { *this };
    }

    ConstIterator cend() const {
        return { *this, m_size };
    }

    T& operator[](std::size_t index) {
        assert(index <= m_size);
        return m_data[index];
    }

    const T& operator[](std::size_t index) const {
        assert(index <= m_size);
        return m_data[index];
    }
    
    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, const Vector<U>& vector);
    
    std::size_t size() const { return m_size; }
    std::size_t capacity() const { return m_capacity; }
    bool empty() const { return m_size == 0; }

    void reserve(std::size_t requested_capacity) {
        if (m_capacity >= requested_capacity) {
            return;
        }
        std::cout << "increasing capacity\n";
        // allocate and init new array
        T* new_data = reinterpret_cast<T*>(malloc(requested_capacity * sizeof(T)));
        if (!new_data) {
            assert(false && "call for malloc() failed - new_data is a nullptr.");
        }
        std::size_t ii{ 0 };
        try {
            for (; ii < m_size; ++ii) {
                new (new_data + ii) T(m_data[ii]);
            }
        } catch (...) {
            // if we failed to init/copy/move any of the existiny object to the new array, we should destruct those that were initialized. 
            for (std::size_t j{ 0 }; j < ii; ++j) {
                new_data[j].~T();
            }
            // or std::destroy(new_arr, new_arr+i);
            free(new_data);
            new_data = nullptr;
            throw;
        }
        // delete old objects
        // std::destroy(m_data, m_data + m_size);
        for (std::size_t ii{ 0 }; ii < m_size; ++ii) {
            m_data[ii].~T();
        }
        free(m_data);
        m_data = new_data;
        m_capacity = requested_capacity;
    }

    void push_back(T item) {
        assert(m_size < m_capacity);
        ++m_size;
        if (passedMaxLoadFactor()) {
            std::cout << "Need to increase capacity. Calling reserve in push_back(T item).\n";
            reserve(m_capacity * RESIZE_FACTOR);
        }
        // place the new item
        new (m_data + m_size) T(item);
    }

    T pop_back() {
        assert(m_size > 0);
        return m_data[--m_size];
    }   
    
};


template<typename T>
std::ostream& operator<<(std::ostream& out, const Vector<T>& vector) {
    out << "capacity: " << vector.m_capacity << " size: " << vector.m_size;
    out << " vector: < ";
    for (int ii{ 0 }; ii < static_cast<int>(vector.m_size); ++ii) {
        out << vector.m_data[ii] << " ";
    }
    out << ">\n";
    return out;
}

int main() {
    Vector<int> vec(2);
    std::cout << vec;
    vec.push_back(0);
    std::cout << vec;
    vec.push_back(1);
    std::cout << vec;
    vec.push_back(2);
    std::cout << vec;
    vec.push_back(3);
    std::cout << "poped: " << vec.pop_back() << "\n";
    vec.push_back(4);
    std::cout << vec;
    vec.push_back(5);
    std::cout << vec;

    std::cout << "testing for-range:\n";
    for (auto item : vec) {
        std::cout << item << " ";
    }
    std::cout << "\n";

    // vec.nullify();
    // for (auto item : vec) {
    //     std::cout << item << " ";
    // }
    // std::cout << "\n";

    std::cout << " ==== DONE PUSHING ==== \n";
    
    std::cout << "vector copy\n";
    Vector<int> vectorCopy{ vec };
    std::cout << vectorCopy;

    std::cout << "assignment operator\n";
    Vector<int> assignmentVec(17);
    assignmentVec = vectorCopy;
    std::cout << assignmentVec;

    vec.pop_back();
    std::cout << vec;
    vec.pop_back();
    std::cout << vec;
    vec.pop_back();
    std::cout << vec;
    vec.pop_back();
    std::cout << vec;
    vec.pop_back();
    std::cout << vec;
    // vec.pop_back();
    // std::cout << vec;


    return 0;
}