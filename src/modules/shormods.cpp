#include <vector>
#include <cmath>
#include <iostream>
#include "module.h"
#include "shormods.h"
#include "stdmods.h"
#include "InfInt.h"

InfInt inv(InfInt i, InfInt m) {
  return i <= 1 ? i : m - (InfInt)(m/i) * inv(m % i, m) % m;
}

using namespace std;

vector<Module*>* Shors::create_children() {
    vector<Module*>* modules = new vector<Module*>();
    int counting_qubits = get_internal_signal("counting_qubits").value;
    int state_qubits = get_internal_signal("state_qubits").value;

    // Create Hadamard
    QuantumCircuit* had_child_op = new QuantumCircuit();
    for (int i = 0; i < counting_qubits; i++) {
        had_child_op->insts.push_back({"h", {i}});
    }

    ModuleArgs* had_args = new ModuleArgs();
    had_args->name = "hadn";
    had_args->circ = had_child_op;
    had_args->procs =  this->mod_args.procs;
    had_args->parent = this->id;

    // for (int i = 0; i < counting_qubits; i++) {
    //     had_args.add_flatten_dict({{"q", i}, {"counting_qubits", i}});
    // }

    Module* had = new Module(*had_args);
    modules->push_back(had);

    // // Create the ModularExponentiation classes
    // int exponent = 1;
    // map<string, vector<int>> controlled_u_flatten_dict;
    // for (int i = 0; i < state_qubits; i++) {
    //     controlled_u_flatten_dict[{"x_qubits", i}] = {"state_qubits", i};
    // }

    ModuleArgs* cont_u_mod_args = new ModuleArgs();
    cont_u_mod_args->name = "controlled_u";
    cont_u_mod_args->parent = this->id;
    cont_u_mod_args->procs = this->mod_args.procs;
    InfInt exponent = a;
    map<string, Signal> input_signals = map<string, Signal>({
            {"control_qubit", Signal("control_qubit", 1, true)},
            {"x_qubits", Signal("x_qubits", state_qubits, true)},
            {"a", Signal("a", exponent, true)},
            {"N", Signal("N", this->N, true)}
    });
    //TODO: Fix the exponent issue
    for (int i = 0; i < counting_qubits; i++) {
        // map<string, vector<int>> flat_dict = controlled_u_flatten_dict;
        // flat_dict[{"control_qubit", 0}] = {"counting_qubits", i};
        map<string, Signal>* input_signals_i = new map<string, Signal>(input_signals);
        input_signals_i->emplace("a", Signal("a", exponent, true));
        cont_u_mod_args->input_signals = input_signals_i;
        exponent *= 2;
        exponent = exponent % this->N;
        ControlledU_a* cont_u = new ControlledU_a(*cont_u_mod_args);
        modules->push_back(cont_u);
    }

    // // Create QFT inv class
    auto qft_mod = new QFT(QFT::QFT_args(counting_qubits, true, this->mod_args.procs));
    modules->push_back(qft_mod);
    // modules.push_back({QFT_mod, qft_args});
    // cout << "Shor children: " << modules->size() << endl;
    // cout << "Num mods so far: " << Module::all_nodes.size() << endl;
    return modules;
}


vector<Module*>* ControlledU_a::create_children() {
    vector<Module*>* modules = new vector<Module*>();
    int x_qubits = get_input_signal("x_qubits").value;
    int zero_qubits = get_internal_signal("zero_qubits").value;

    // Create Multiplier
    ModuleArgs* mult_args = new ModuleArgs();
    mult_args->name = "controlled_mult";
    mult_args->procs =  this->mod_args.procs;
    map<string, Signal>* input_signals = new map<string, Signal>({
        {"control_qubit", Signal("control_qubit", 1, true)},
        {"x_qubits", Signal("x_qubits", x_qubits, true)},
        {"b_qubits", Signal("b_qubits", zero_qubits, true)},
        {"a", Signal("a", this->a, false)},
        {"N", Signal("N", this->N, false)}
    });
    mult_args->input_signals = input_signals;
    mult_args->parent = this->id;

    Module* mult = new ControlledMultiplier(*mult_args);
    modules->push_back(mult);

    // Create Swap
    auto swap = new ControlledSwapN(ControlledSwapN::ControlledSwapArgs(x_qubits,this->mod_args.procs));
    modules->push_back(swap);

    // Inverse Multiplier

    ModuleArgs* mult_inv_args = new ModuleArgs(*mult_args);
    mult_inv_args->inverse = true;
    InfInt a_inv = inv(this->a, this->N);

    Module* mult_inv = new ControlledMultiplier(*mult_inv_args);
    modules->push_back(mult_inv);
    // modules.push_back({QFT_mod, qft_args});
    // cout << "Cont U children: " << modules->size() << endl;
    // cout << "Num mods so far: " << Module::all_nodes.size() << endl;
    return modules;
}
