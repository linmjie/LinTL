#include "../vector/List.cpp"
#include "../optional/Optional.cpp"
#include "Lifetime.cpp"
#include <iostream>

void list_test() {
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
}

void optional_test() {
    auto thing = Some(3);
    if(thing.has()) {
        std::cout << *thing << '\n';
    } else {
        std::cout << "nothing" << '\n';
    }
}

int main(int argc, char *argv[]) {
    optional_test();

    return 0;
}
