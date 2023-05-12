#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#include "shormods.h"
#include "module.h"

using namespace std;

int main(int argc, char *argv[]) {
    std::vector<int> procs({1});
    auto num_bits = atoi(argv[1]);
    // auto num_bits = 8;
    if (num_bits == 0) {
        num_bits = 8;
    }
    Module* shors = new Shors(procs, num_bits);
    shors->make_children();

    // cout << "Num mods: " << shors->get_num_nodes() << endl;
    cout << shors->bytesize() << endl;
    cout << sizeof(Module) << endl;

    delete shors;
    return 0;
}