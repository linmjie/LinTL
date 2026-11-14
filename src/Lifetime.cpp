#include <iostream>
#include <ostream>

class Lifetime {
public:
    Lifetime() { std::cout << "Lifetime() called\n"; }

    Lifetime(int dummy1, int dummy2, int dummy3) {
        std::cout << "Dummy Lifetime(...) called\n";
    }

    ~Lifetime() { std::cout << "~Lifetime() called\n"; }

    Lifetime(const Lifetime &) {
        std::cout << "Lifetime(const Lifetime&) called\n";
    }

    Lifetime(Lifetime &&) noexcept {
        std::cout << "Lifetime(Lifetime &&) called\n";
    }

    Lifetime &operator=(const Lifetime &) {
        std::cout << "operator=(const Lifetime &) called\n";
        return *this;
    }

    Lifetime &operator=(Lifetime &&) noexcept {
        std::cout << "operator=(Lifetime &&) called\n";
        return *this;
    }
};

std::ostream &operator<<(std::ostream &stream, const Lifetime &lifetime) {
    return stream;
}
