#include <memory>
#include <ostream>

template <typename T, typename Allocater = std::allocator<T>>
class List {
public:
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

    T &back() { return m_data[m_size - 1]; }

    const T &back() const { return m_data[m_size - 1]; }

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

        size_t oldCapacity = m_capacity;
        if (m_capacity == 0) {
            m_capacity = DEFAULT_INITIAL_CAPACITY;
        }
        m_capacity *= CAPACITY_GROWTH_MULTIPLIER;
        T *new_data = alloc::allocate(m_allocator, m_capacity);
        for (size_t i = 0; i < m_size; ++i) {
            new_data[i] = std::move(m_data[i]);
        }
        alloc::deallocate(m_allocator, m_data, oldCapacity);
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
