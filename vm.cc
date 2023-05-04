#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>

class VirtualMachine {
public:
    void mov(const std::string& reg, int value) {
        registers[reg] = value;
    }

    void add(const std::string& reg1, const std::string& reg2, const std::string& reg3) {
        registers[reg1] = registers[reg2] + registers[reg3];
    }

    int getRegisterValue(const std::string& reg) {
        return registers[reg];
    }

private:
    std::map<std::string, int> registers;
};

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void executeInstruction(const std::string& instruction, VirtualMachine& vm) {
    std::vector<std::string> tokens = split(instruction, ' ');

    if (tokens[0] == "mov.vec.s32") {
        std::string reg = tokens[1].substr(0, tokens[1].size() - 1);
        int value = std::stoi(tokens[2]);
        vm.mov(reg, value);
    } else if (tokens[0] == "add.vec.s32") {
        std::string reg1 = tokens[1].substr(0, tokens[1].size() - 1);
        std::string reg2 = tokens[2].substr(0, tokens[2].size() - 1);
        std::string reg3 = tokens[3];
        vm.add(reg1, reg2, reg3);
    } else {
        std::cerr << "Unknown instruction: " << tokens[0] << std::endl;
    }
}

int main() {
    VirtualMachine vm;
    std::vector<std::string> instructions = {
        "mov.vec.s32 r1, 3",
        "mov.vec.s32 r2, 4",
        "add.vec.s32 r0, r1, r2"
    };

    for (const std::string& instruction : instructions) {
        executeInstruction(instruction, vm);
    }

    std::cout << "r0: " << vm.getRegisterValue("r0") << std::endl; // Output: r0: 7

    return 0;
}
