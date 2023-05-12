#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

#include "stdmods.h"
#include "module.h"

using namespace std;

vector<Module*>* ControlledMultiplier::create_children() {
    vector<Module*>* modules = new vector<Module*>();
    auto x_qubits = get_input_signal("x_qubits").value;
    auto b_qubits = get_input_signal("b_qubits").value;
    auto procs = this->mod_args.procs;

    auto qft = new QFT(QFT::QFT_args(x_qubits, false, procs));
    modules->push_back(qft);

    ModuleArgs* adder_args = new ModuleArgs();
    adder_args->name = "controlled_mod_adder";
    adder_args->parent = this->id;
    adder_args->inverse = this->mod_args.inverse;
    map<string, Signal> mod_adder_input_signals = {
        {"control_qubits", Signal("control_qubits", 2, true)},
        {"a", Signal("a", a, false)}, 
        {"b_qubits", Signal("b_qubits", b_qubits, true)},
        {"N", Signal("N", N, false)}};

    auto a_add = a;
    // TODO: If inverse, make sure to divide a and start from large a
    for (auto i = 0; i < x_qubits; i++) {
        auto mod_adder_input_signals_i = new map<string, Signal>(mod_adder_input_signals);
        mod_adder_input_signals_i->emplace("a", Signal("a", a_add, false));
        adder_args->input_signals = mod_adder_input_signals_i;
        auto mod_adder = new ControlledModAdder(*adder_args);
        modules->push_back(mod_adder);
        a_add *= 2;
    }

    auto qft_inv = new QFT(QFT::QFT_args(x_qubits, true, procs));
    modules->push_back(qft_inv);
    // cout << "Mult children: " << modules->size() << endl;
    // cout << "Num mods so far: " << Module::all_nodes.size() << endl;
    return modules;
}

vector<Module*>* ControlledModAdder::create_children() {
    auto procs = this->mod_args.procs;
    int b_qubits = get_input_signal("b_qubits").value;
    vector<Module*>* modules = new vector<Module*>();

    // This has the same circuit twice, so we can just repeat the steps
    for (int i = 0; i < 2; i++) {
        auto add_a_1 = new Adder(Adder::Adder_args(b_qubits, (i == 1) != this->mod_args.inverse, procs));
        modules->push_back(add_a_1);
        if (i == 0) {
            // Add inverse N the first go through
            auto add_N_1 = new Adder(Adder::Adder_args(b_qubits, this->mod_args.inverse, procs));
            modules->push_back(add_N_1);
        }
        
        auto qft_inv = new QFT(QFT::QFT_args(b_qubits, true, procs));
        modules->push_back(qft_inv);

        // Construct CNOT submodule
        QuantumCircuit cnot_qc;
        string name = "cnot_mod";
        if (i == 1) {
            cnot_qc.insts.push_back({"x", {0}});
        }
        cnot_qc.insts.push_back({"cnot", {0,1}});
        if (i == 1) {
            cnot_qc.insts.push_back({"x", {0}});
            name = "x_cnot_x_mod";
        }
        ModuleArgs* cnot_args = new ModuleArgs();
        cnot_args->name = name;
        cnot_args->parent = this->id;
        auto cnot_mod = new Module(*cnot_args);
        modules->push_back(cnot_mod);
        auto qft = new QFT(QFT::QFT_args(b_qubits, false, procs));
        modules->push_back(qft);
        if (i == 0) {
            // Add inverse N the first go through
            auto add_N_2 = new Adder(Adder::Adder_args(b_qubits, this->mod_args.inverse, procs));
            modules->push_back(add_N_2);
        } else {
            // Add inverse N the first go through
            auto add_a_2 = new Adder(Adder::Adder_args(b_qubits, this->mod_args.inverse, procs));
            modules->push_back(add_a_2);
        }
    }

    if (this->mod_args.inverse) {
        reverse(modules->begin(), modules->end());
    }
    // cout << "Mod Adder children: " << modules->size() << endl;
    // cout << "Num mods so far: " << Module::all_nodes.size() << endl;
    return modules;
}