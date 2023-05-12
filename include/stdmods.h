#include <vector>
#include <map>
#include <string>
#include "module.h"
#include "InfInt.h"

using namespace std;

class ControlledMultiplier : public Module {
    private:
        InfInt a;
        InfInt N;
    public:
        ControlledMultiplier (ModuleArgs& mod_args): Module(mod_args){
            this->a = get_input_signal("a").value_big;
            this->N = get_input_signal("N").value_big;
        }

        bool has_children() const {
            return true;
        }
        vector<Module*>* create_children();
};

class ControlledModAdder : public Module {
    private:
        InfInt a;
        InfInt N;

    public:
        ControlledModAdder (ModuleArgs& mod_args): Module(mod_args){
            this->a = get_input_signal("a").value_big;
            this->N = get_input_signal("N").value_big;
        }

        bool has_children() const {
            return true;
        }
        vector<Module*>* create_children();
};

class ControlledSwapN : public Module {
    public:
        static ModuleArgs& ControlledSwapArgs(int num_bits, vector<int>* procs) {
            ModuleArgs* swap_args = new ModuleArgs();
            swap_args->name = "controlled_swap";
            swap_args->procs = procs;
            auto qubit_signal = Signal("q", num_bits, true);
            swap_args->input_signals = new map<string, Signal>({{"q", qubit_signal}});
            return *swap_args;
        }

        ControlledSwapN (ModuleArgs& mod_args): Module(mod_args){}

        bool has_children() const {
            return false;
        }
        virtual vector<Module*>* create_children() {
            return nullptr;
        }
};

class Adder : public Module {
    public:
        static ModuleArgs& Adder_args(int num_bits, bool inverse, vector<int>* procs) {
            ModuleArgs* adder_args = new ModuleArgs();
            adder_args->name = "adder";
            adder_args->inverse = inverse;
            adder_args->procs = procs;
            auto qubit_signal = Signal("q", num_bits, true);
            adder_args->input_signals = new map<string, Signal>({{"q", qubit_signal}});
            return *adder_args;
        }

        Adder (ModuleArgs& mod_args): Module(mod_args){}

        bool has_children() const {
            return false;
        }
        virtual vector<Module*>* create_children() {
            return nullptr;
        }
};


class QFT : public Module {
    public:
        static ModuleArgs& QFT_args(int num_bits, bool inverse, vector<int>* procs) {
            ModuleArgs* qft_args = new ModuleArgs();
            qft_args->name = "qft";
            qft_args->inverse = inverse;
            qft_args->procs = procs;
            auto qubit_signal = Signal("q", num_bits, true);
            qft_args->input_signals = new map<string, Signal>({{"q", qubit_signal}});
            return *qft_args;
        }

        QFT (ModuleArgs& mod_args): Module(mod_args){}

        bool has_children() const {
            return false;
        }
        virtual vector<Module*>* create_children() {
            return nullptr;
        }
};
