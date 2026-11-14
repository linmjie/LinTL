#include <iostream>
#include "Lifetime.cpp"
#include "../vector/List.cpp"

int main(int argc, char *argv[]) {
    List<Lifetime> thing;
    for (int i = 0; i < 100; i++) {
        std::cout << "trying to append: " << i << '\n';
        thing.empend(i, i, i);
    }

    for (int i = 0; i < 100; i++) {
        std::cout << "output: " << thing[i] << '\n';
    }

    std::cout << thing << '\n';

    return 0;
}
