#include <functional>
#include <memory>
#include <ostream>

template <typename T, typename Allocater = std::allocator<T>>
class List {
private:
    // maybe do some more template shenanigans to not need ReferenceType passed
    // in and deduce a const T & automatically if PointerType is a T const *
    template <typename PointerType, typename ReferenceType, typename Vec>
    class VectorIterator {
        using ValueType = Vec::ValueType;

    public:
        VectorIterator(ValueType *data) : iter_data(data) {}

        VectorIterator &operator++() {
            ++iter_data;
            return *this;
        }

        VectorIterator operator++(int) {
            VectorIterator copy = *this;
            ++(*this);
            return copy;
        }

        VectorIterator &operator--() {
            --iter_data;
            return *this;
        }

        VectorIterator operator--(int) {
            VectorIterator copy = *this;
            --(*this);
            return copy;
        }

        VectorIterator operator+(size_t rhs) const {
            return VectorIterator(iter_data + rhs);
        }

        VectorIterator operator-(size_t rhs) const {
            return VectorIterator(iter_data - rhs);
        }

        bool operator==(const VectorIterator &other) const {
            return iter_data == other.iter_data;
        }

        bool operator!=(const VectorIterator &other) const {
            return iter_data != other.iter_data;
        }

        ReferenceType operator[](size_t idx) { return iter_data[idx]; }

        PointerType *operator->() { return iter_data; }

        ReferenceType operator*() { return *iter_data; }

    private:
        ValueType *iter_data;
    };

public:
    using Iterator = VectorIterator<T *, T &, List<T>>;
    using ConstIterator = VectorIterator<T const *, const T &, List<T>>;

    using ValueType = T;

    List() : m_size(0), m_capacity(0), m_data(nullptr) {}

    List(size_t initial_capacity)
        : m_size(0), m_capacity(initial_capacity),
          m_data(alloc::allocate(m_allocator, m_capacity)) {}

    ~List() {
        if (!m_data) {
            return;
        }
        delete_data();
    }

    List(const List &other)
        : m_size(other.m_size), m_capacity(other.m_capacity),
          m_data(alloc::allocate(m_allocator, m_size)) {
        for (size_t i = 0; i < m_size; ++i) {
            alloc::construct(m_allocator, m_data + i, other);
        }
    }

    List &operator=(const List &rhs) {
        if (m_capacity <= rhs.m_size) {
            delete_data();

            m_size = rhs.m_size;
            m_capacity = rhs.m_size;

            m_data = alloc::allocate(m_allocator, m_capacity);

            for (size_t i = 0; i < m_size; ++i) {
                alloc::construct(m_allocator, m_data + i, rhs);
            }
        } else {
            for (size_t i = 0; i < rhs.m_size; ++i) {
                m_data[i] = rhs.m_data[i];
            }
            if (m_size > rhs.m_size) {
                for (size_t i = rhs.m_size + 1; i < m_size; ++i) {
                    alloc::destroy(m_data[i]);
                }
            }
            m_size = rhs.m_size;
        }
        return *this;
    }

    List(List &&other) noexcept
        : m_size(other.m_size), m_capacity(other.m_capacity),
          m_data(other.m_data) {
        other.m_data = nullptr;
    }

    List &operator=(List &&rhs) noexcept {
        delete_data();
        m_size = rhs.m_size;
        m_capacity = rhs.m_capacity;
        m_data = rhs.m_data;
        rhs.m_data = nullptr;
        return *this;
    }

    T &operator[](size_t idx) { return this->get(idx); }

    const T &operator[](size_t idx) const { return this->get(idx); }

    T &get(size_t idx) {
        if (idx >= m_size) {
            throw "bad access";
        }
        return m_data[idx];
    }

    const T &get(size_t idx) const {
        if (idx >= m_size) {
            throw "bad access";
        }
        return m_data[idx];
    }

    void reserve(size_t new_capacity) {
        m_capacity = new_capacity;
        T *new_data = alloc::allocate(m_allocator, m_capacity);
        for (size_t i = 0; i < m_size; ++i) {
            new_data[i] = std::move(m_data[i]);
        }
        alloc::deallocate(m_allocator, m_data, m_capacity);
    }

    void append(const T &item) {
        expand_if_needed();
        alloc::construct(m_allocator, m_data + m_size++, item);
    }

    void append(T &&item) {
        expand_if_needed();
        alloc::construct(m_allocator, m_data + m_size++, item);
    }

    template <typename... Args>
    void empend(Args &&...args) {
        expand_if_needed();
        alloc::construct(m_allocator, m_data + m_size++,
                         std::forward<Args>(args)...);
    }

    size_t size() const { return m_size; }

    Iterator begin() { return Iterator(m_data); }

    Iterator end() { return Iterator(m_data + m_size); }

    ConstIterator begin() const { return ConstIterator(m_data); }

    ConstIterator end() const { return ConstIterator(m_data + m_size); }

    T &back() { return m_data[m_size - 1]; }

    const T &back() const { return m_data[m_size - 1]; }

    void for_each(std::function<void(const T &)> callback) const {
        for (const T &item : *this) {
            callback(item);
        }
    }

    void for_each(std::function<void(T)> callback) const {
        for (T item : *this) {
            callback(item);
        }
    }

    void for_each(std::function<void(T &)> callback) {
        for (T &item : *this) {
            callback(item);
        }
    }

    // change std::allocator<T> to Allocator once done
    // using current setup for easier lsp autocomplete
private:
    std::allocator<T> m_allocator;
    size_t m_size;
    size_t m_capacity;
    T *m_data;

private:
    using alloc = std::allocator_traits<std::allocator<T>>;

private:
    static constexpr size_t CAPACITY_GROWTH_MULTIPLIER = 2;
    static constexpr size_t DEFAULT_INITIAL_CAPACITY = 24;

private:
    void expand_if_needed() {
        if (m_size < m_capacity) {
            return;
        }

        size_t old_capacity = m_capacity;
        if (m_capacity == 0) {
            m_capacity = DEFAULT_INITIAL_CAPACITY;
        }

        m_capacity *= CAPACITY_GROWTH_MULTIPLIER;
        T *new_data = alloc::allocate(m_allocator, m_capacity);
        for (size_t i = 0; i < m_size; ++i) {
            new_data[i] = std::move(m_data[i]);
        }
        alloc::deallocate(m_allocator, m_data, old_capacity);
        m_data = new_data;
    }

    void delete_data() {
        for (size_t i = 0; i < m_size; ++i) {
            alloc::destroy(m_allocator, m_data + i);
        }
        alloc::deallocate(m_allocator, m_data, m_capacity);
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &stream, const List<T> &list) {
    if (list.size() == 0) {
        return stream;
    }

    stream << '[';
    for (size_t i = 0; i < list.size() - 1; i++) {
        stream << list[i] << ", ";
    }

    stream << list.back() << ']';
    return stream;
}
