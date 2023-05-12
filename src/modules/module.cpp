#include "module.h"
#include <vector>

using namespace std;

map<int, Module*> Module::all_nodes = {};
map<Module, int> Module::cache = {};

void Module::make_children() {
    vector<Module*>* child_vector = this->create_children();
    this->make_network(child_vector);
    if (child_vector != nullptr) {
        for (auto&child : *child_vector) {
            child->make_children();
        }
    }
}

void Module::make_network(vector<Module*>* children) {
    if (children == nullptr) {
        return;
    }
    // vector<tuple<int, int, int>>* edge_list = new vector<tuple<int, int, int>>();
    auto in_order_list = new vector<int>();
    in_order_list->reserve(children->size());
    // in_order_list.reserve(children->size());
    auto prev_id = -1;
    for (auto &child : *children) {
        in_order_list->push_back(child->id);
        // if (prev_id > 0) {
        //     edge_list.push_back(make_tuple(prev_id, child->id, 1));
        // }
        prev_id = child->id;
    }
    this->children = new Children(*in_order_list);
}