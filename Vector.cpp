#include <cassert>
#include <iostream>

template<typename T>
class Vector {
private:
    std::size_t m_size{};
    std::size_t m_capacity{ 1 };
    T* m_data{};
    
    static constexpr double MAX_LOAD_FACTOR { 0.75 };
    static constexpr double MIN_LOAD_FACTOR { 0.25 };
    static constexpr double RESIZE_FACTOR { 2.0 };

    double getLoadFactor() const { return m_size / static_cast<double>(m_capacity); }
    bool passedMaxLoadFactor() const { return getLoadFactor() >= MAX_LOAD_FACTOR; }

    void allocMemory() {
        m_data = new T[m_capacity];
    }

    void deallocMemory() {
        if (m_data)
        {
            delete[] m_data;
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
        allocMemory();
    }
    
    explicit Vector(std::size_t size) : m_capacity{ static_cast<size_t>(std::max(size * 2, 1ul)) } {
        assert(size >= 0);
        allocMemory();
    }
    
    Vector(const Vector& other) {
        for (auto elem : other) {
            push_back(elem);
        }
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        deallocMemory();
        m_capacity = other.m_capacity;
        allocMemory();
        m_size = 0;
        for (auto elem : other) {
            push_back(elem);
        }
        return *this;
    }
    
    ~Vector() {
        deallocMemory();
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
        assert(index >= 0);
        assert(index <= m_size);
        return m_data[index];
    }

    const T& operator[](std::size_t index) const {
        assert(index >= 0);
        assert(index <= m_size);
        return m_data[index];
    }
    
    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, const Vector<U>& vector);
    
    std::size_t size() const { return m_size; }
    std::size_t capacity() const { return m_capacity; }
    bool empty() const { return m_size == 0; }

    // void nullify() {
    //     for (T& item : *this) {
    //         item = T();
    //     }
    // }

    void push_back(T item) {
        assert(m_size < m_capacity);
        ++m_size;
        if (passedMaxLoadFactor()) {
            m_capacity = m_capacity * RESIZE_FACTOR ;
            T* newArr = new T[m_capacity];
            for (std::size_t ii{ 0 }; ii < m_size - 1; ++ii) {
                newArr[ii] = m_data[ii];
            }
            deallocMemory();
            m_data = newArr;
        }
        m_data[m_size - 1] = item;
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