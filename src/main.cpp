#include "../vector/List.cpp"
#include "Lifetime.cpp"
#include <iostream>

int main(int argc, char *argv[]) {
    List<Lifetime> thing;
    // for (int i = 0; i < 100; i++) {
    //     std::cout << "trying to append: " << i << '\n';
    //     thing.empend(i, i, i);
    // }
    //
    // for (int i = 0; i < 100; i++) {
    //     std::cout << "output: " << thing[i] << '\n';
    // }
    //
    // std::cout << thing << '\n';

    List<int> second;
    for (int i = 0; i < 100; i++) {
        second.append(i);
    }

    second.for_each([](int i) {
        std::cout << "second output: " << i << '\n';
    });

    return 0;
}
