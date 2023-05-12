#ifndef MODULE_H
#define MODULE_H

#include <vector>
#include <map>
#include <string>
#include "InfInt.h"

using namespace std;

struct QuantumCircuit {
    vector<pair<string, vector<int>>> insts;
};

class Children {
public:
    // vector<tuple<int, int, int>> edge_list;
    vector<int> in_order_list;

    Children(vector<int>& in_order_list) : in_order_list(in_order_list) {}

    int calculate_longest_path() {
        return 1;
    }

    int bytesize() {
        // return edge_list.capacity() * sizeof(tuple<int, int, int>) 
        return in_order_list.capacity() * sizeof(int);
    }
};



struct Signal {
    Signal(const string name, int value, bool is_qreg) {
        this->name = name;
        this->value = value;
        this->value_big = -1;
        this->is_qreg = is_qreg;
    }
    Signal(const string name, InfInt value, bool is_qreg) {
        this->name = name;
        this->value_big = value;
        this->value = -1;
        this->is_qreg = is_qreg;
    }
    string name = "";
    int value;
    InfInt value_big;
    bool is_qreg = true;
};

struct ModuleArgs {
    string name = "";
    QuantumCircuit* circ = nullptr;
    vector<int>* procs;
    int parent = -1;
    map<string, Signal>* input_signals = nullptr;
    map<string, Signal>* internal_signals = nullptr;
    map<string, Signal>* output_signals = nullptr;
    bool inverse = false;
    int num_control_bits = 0;
    map<tuple<string, int>, tuple<string, int>>* flatten_dict = nullptr;
    int mod_id = -1;
};


class Module {
    protected:
        int id;
        string name = "";
        string id_str = "";
        ModuleArgs mod_args;
        Children* children;
        int num_control;
    public:
        static map<int, Module*> all_nodes;
        static map<Module, int> cache;

        void init() {
            id = Module::all_nodes.size();
            Module::all_nodes[id] = this;
            // name = get_base_name();
            id_str = name + "_" + to_string(id);
            // op = nullptr;
            this->mod_args = mod_args;
            // qasm = nullptr;
            children = nullptr;
            num_control = 0;
            if (mod_args.num_control_bits > 0) {
                // add_control(mod_args.control_bits);
            }
        }

        Signal get_internal_signal(const string& name) {
            return this->mod_args.internal_signals->at(name);
        }

        Signal get_input_signal(const string& name) {
            return this->mod_args.input_signals->at(name);
        }

        Module() {
            init();
        }

        Module(const ModuleArgs& mod_args) {
            init();
            this->mod_args = mod_args;
        }

        // Children functions
        void make_children();
        virtual void make_network(vector<Module*>* children);
        virtual vector<Module*>* create_children() {
            return nullptr;
        }

        virtual int get_num_nodes() {
            int num_nodes = 0;
            std::vector<Module*> modules {this};
            while (modules.size() > 0) {
                num_nodes++;
                Module* mod = modules.back();
                modules.pop_back();
                if (mod->children != nullptr) {
                    for (auto &child : mod->children->in_order_list) {
                        modules.push_back(Module::all_nodes[child]);
                    }
                }
            }
            return num_nodes;
        }

        int bytesize() {
            auto args_size = sizeof(this->mod_args);
            auto procs_size = this->mod_args.procs->capacity() * sizeof(int);
            auto inp_signals = 0;
            auto int_signals = 0;
            if (this->mod_args.input_signals != nullptr) {
                inp_signals = this->mod_args.input_signals->size() * (128 + sizeof(Signal));
            }
            if (this->mod_args.internal_signals != nullptr) {
                int_signals = this->mod_args.internal_signals->size() * (128 + sizeof(Signal));
            }
            args_size += (procs_size + inp_signals + int_signals);
            auto child_size = this->children->bytesize();
            cout << child_size << endl;
            return child_size + args_size + sizeof(*this);
        }

        virtual ~Module() {
            delete this->mod_args.circ;
            delete this->mod_args.input_signals;
            delete this->mod_args.internal_signals;
            delete this->mod_args.output_signals;
            delete this->children;
            // delete this->op;
            // delete this->qasm;
        }
};
#endif