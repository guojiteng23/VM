#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

constexpr int reglen = 32;

struct Instruction {
  enum {
    MOV,
    ADD,
    SUB,
    MUL,
    DIV,
    LD,
    ST,
    AND,
    OR,
    XOR,
    NOT,
    SHL,
    SHR,
  } InstructionType;

  enum {
    s8,
    s16,
    s32,
    u8,
    u16,
    u32,
    f16,
    f32,
  } DataType;

  // index of register
  int dst;
  int src1;
  int src2;

  int src1_imm;
  int src2_imm;

  bool src1_is_imm;
  bool src2_is_imm;

  std::array<bool, reglen> mask;
};

constexpr int regNum = 32;

class VM {
  private:
    std::array<std::array<int, reglen>, regNum> registers = {};
    std::array<std::array<bool, reglen>, regNum> masks = {};

    std::vector<Instruction> instructions;

    // Memory
    char *memory;

  public:
    VM() { memory = new char[1024 * 1024 * 1024]; }

    ~VM() { delete[] memory; }

    std::array<int, reglen> getRegister(int regNum) {
      if (regNum >= reglen || regNum < 0) {
        std::cout << "Invalid register number: " << regNum << std::endl;
        exit(1);
      }
      return registers[regNum];
    }

    void checkMemoryAccess(int address) {
      if (address < 0 || address >= 1024 * 1024 * 1024 * sizeof(char)) {
        std::cout << "Invalid memory address" << std::endl;
        exit(1);
      }
    }

    void set_masks(Instruction inst) {
        for (int i = 0; i < reglen; i++) {
            // 将 mask 赋值给 masks 中对应的行
            masks[inst.dst % reglen][i] = inst.mask[i];
        }
    }

    void emitInst(const Instruction inst) {
        instructions.push_back(inst);
    }

    void execute() {
      for (auto &inst : instructions) {
        // parse instruction
        int dst = inst.dst;
        bool is_src1_imm = inst.src1_is_imm;
        bool is_src2_imm = inst.src2_is_imm;
        int src1 = inst.src1;
        int src2 = inst.src2;

        auto &dst_reg = registers[dst];
        auto &src1_reg = registers[src1];
        auto &src2_reg = registers[src2];
        auto &mask = masks[dst];

        auto op = inst.InstructionType;

        // Check register, memory and immediate validity
        if (dst >= regNum || dst < 0) {
          std::cout << "Invalid destination register: " << dst << std::endl;
          exit(1);
        }
        if (!is_src1_imm && (src1 >= regNum || src1 < 0)) {
          std::cout << "Invalid source register 1: " << src1 << std::endl;
          exit(1);
        }
        if (!is_src2_imm && (src2 >= regNum || src2 < 0)) {
          std::cout << "Invalid source register 2: " << src2 << std::endl;
          exit(1);
        }
        if (is_src1_imm && (src1 < 0 || src1 >= regNum)) {
          std::cout << "Invalid immediate source 1: " << src1 << std::endl;
          exit(1);
        }
        if (is_src2_imm && (src2 < 0 || src2 >= regNum)) {
          std::cout << "Invalid immediate source 2: " << src2 << std::endl;
          exit(1);
        }

        for (int i = 0; i < reglen; i++) {
          switch (op) {
              case Instruction::MOV:
                if (mask[i]) {
                  dst_reg[i] = is_src1_imm ? inst.src1_imm : src1_reg[i];
                }
                break;
              case Instruction::ADD:
                if (mask[i]) {
                  dst_reg[i] = ((is_src1_imm ? inst.src1_imm : src1_reg[i]) + (is_src2_imm ? inst.src2_imm : src2_reg[i]));
                }
                break;
              case Instruction::SUB:
                if (mask[i]) {
                  dst_reg[i] = ((is_src1_imm ? inst.src1_imm : src1_reg[i]) - (is_src2_imm ? inst.src2_imm : src2_reg[i]));
                }
                break;
              case Instruction::MUL:
                if (mask[i]) {
                  dst_reg[i] = ((is_src1_imm ? inst.src1_imm : src1_reg[i]) * (is_src2_imm ? inst.src2_imm : src2_reg[i]));
                }
                break;
              case Instruction::DIV:
                if (is_src2_imm ? inst.src2_imm : src2_reg[i] == 0) {
                  std::cout << "Divide by zero" << std::endl;
                  exit(1);
                }
                if (mask[i]) {
                  dst_reg[i] = ((is_src1_imm ? inst.src1_imm : src1_reg[i]) / (is_src2_imm ? inst.src2_imm : src2_reg[i]));
                }
                break;
              case Instruction::LD:
                checkMemoryAccess(is_src1_imm ? inst.src1_imm : registers[src1][i]);
                dst_reg[i] = memory[is_src1_imm ? inst.src1_imm : src1_reg[i]];
                break;
              case Instruction::ST:
                checkMemoryAccess(is_src1_imm ? inst.src1_imm : registers[src1][i]);
                memory[is_src1_imm ? inst.src1_imm : src1_reg[i]] = dst_reg[i];
                break;
              default:
                std::cout << "Invalid instruction" << std::endl;
                exit(1);
          }
        }
      }
    }
};

int main() {
  VM vm;

  Instruction inst = {
    .InstructionType = Instruction::ADD,
    .DataType = Instruction::s32,
    .dst = 1,   // dst
    .src1 = 2,   // src1
    .src2 = 3,   // src2
    .src1_imm = 9,   // src1_imm
    .src2_imm = 8,   // src2_imm
    .src1_is_imm = true, // src1_is_imm
    .src2_is_imm = true,  // src2_is_imm
    .mask = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
             1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} // mask
  };

  vm.set_masks(inst);

  vm.emitInst(inst);

  vm.execute();

  // print the final result
  for (int i = 0; i < reglen; i++) {
    std::cout << vm.getRegister(inst.dst)[i] << std::endl;
  }
  return 0;
}
