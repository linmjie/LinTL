#include <utility>

class None {
    None() = default;
};

template <typename T>
class Optional {
public:
    template <typename... Args>
    explicit Optional(Args &&...args)
        : m_value(std::forward<Args>(args)...), m_has(true) {}

    Optional(None none) : m_dummy(), m_has(false) {};

    bool has() const { return m_has; }

    T &operator*() { return get(); }

    const T &operator*() const { return get(); }

    T &get() { return m_value; }

    const T &get() const { return m_value; }

private:
    union {
        char m_dummy;
        T m_value;
    };
    bool m_has;
};

template <typename T>
Optional<T> Some(const T &arg) {
    return Optional<T>(arg);
}

template <typename T>
Optional<T> Some(T &&arg) {
    return Optional<T>(std::forward<T>(arg));
}

template <typename T, typename... Args>
Optional<T> Some(Args &&...args) {
    return Optional<T>(std::forward<Args>(args)...);
}
