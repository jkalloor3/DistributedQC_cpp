#include "module.h"
#include <vector>
#include <cmath>
#include "InfInt.h"

using namespace std;

class Shors : public Module {
    private:
        int a;
        int num_qubits;
        InfInt N;

    public:
        Shors(vector<int>& procs, int num_qubits = 32): Module(){
            ModuleArgs mod_args;
            mod_args.procs = &procs;
            this->a = 8;
            this->num_qubits = num_qubits;
            InfInt big2 = 12312351237891LL;
            this->N = big2;
            cout << N << endl;
            auto int_signals = new map<string, Signal>({
                {"counting_qubits", Signal("counting_qubits", num_qubits*2, true)},
                {"state_qubits", Signal("state_qubits", num_qubits, true)}
            });
            mod_args.internal_signals = int_signals;
            this->mod_args = mod_args;
        }
        
        bool has_children() const {
            return true;
        }

        vector<Module*>* create_children();
};

class ControlledU_a : public Module {
    private:
        InfInt a;
        InfInt N;
    public:
        ControlledU_a (ModuleArgs mod_args): Module(mod_args){
            this->a = get_input_signal("a").value_big;
            this->N = get_input_signal("N").value_big;
            auto num_qubits = get_input_signal("x_qubits").value;
            auto int_signals = new map<string, Signal>({
                {"zero_qubits", Signal("zero_qubits", num_qubits, true)}
            });
            this->mod_args.internal_signals = int_signals;
        }

        bool has_children() const {
            return true;
        }
        vector<Module*>* create_children();
};

