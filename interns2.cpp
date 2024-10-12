#include <iostream>
#include <vector>
#include <stack>
#include <cstdint>

// Enum to define opcodes
enum class Opcode : uint8_t {
    ADD = 0x01, SUB = 0x02, MUL = 0x03, DIV = 0x04,
    INC = 0x05, DEC = 0x06, AND = 0x07, OR = 0x08,
    XOR = 0x09, NOT = 0x0A, JMP = 0x0B, BEQ = 0x0C,
    BNE = 0x0D, CALL = 0x0E, RET = 0x0F, LD = 0x10,
    ST = 0x11, FFT = 0x12, ENC = 0x13, DECRYPT = 0x15
};

// Register File class to manage CPU registers
class RegisterFile {
private:
    std::vector<uint32_t> registers;

public:
    RegisterFile(size_t size = 16) : registers(size, 0) {}

    void set(uint8_t index, uint32_t value) {
        if (index < registers.size())
            registers[index] = value;
    }

    uint32_t get(uint8_t index) const {
        if (index < registers.size())
            return registers[index];
        return 0;
    }
};

// Memory class to handle memory operations
class Memory {
private:
    std::vector<uint32_t> memory;

public:
    Memory(size_t size = 1024) : memory(size, 0) {}

    void set(uint16_t address, uint32_t value) {
        if (address < memory.size())
            memory[address] = value;
    }

    uint32_t get(uint16_t address) const {
        if (address < memory.size())
            return memory[address];
        return 0;
    }

    size_t getSize() const {
        return memory.size();
    }
};

// Arithmetic Logic Unit (ALU) to perform operations
class ALU {
public:
    uint32_t add(uint32_t a, uint32_t b) { return a + b; }
    uint32_t sub(uint32_t a, uint32_t b) { return a - b; }
    uint32_t mul(uint32_t a, uint32_t b) { return a * b; }
    uint32_t div(uint32_t a, uint32_t b) { return (b != 0) ? a / b : 0; }
    uint32_t inc(uint32_t a) { return a + 1; }
    uint32_t dec(uint32_t a) { return a - 1; }
};

// CPU class to integrate all components
class CPU {
private:
    RegisterFile regFile;
    Memory memory;
    ALU alu;
    uint32_t pc = 0;
    std::stack<uint32_t> callStack;

public:
    CPU(size_t memorySize = 1024) : memory(memorySize) {}

    Memory& getMemory() { return memory; }
    RegisterFile& getRegisterFile() { return regFile; }

    void fetchAndExecute() {
        uint32_t instr = memory.get(pc++);
        uint8_t rawOpcode = instr >> 24;
        Opcode opcode = static_cast<Opcode>(rawOpcode);
        uint8_t r1 = (instr >> 16) & 0xFF;
        uint8_t r2 = (instr >> 8) & 0xFF;
        uint8_t r3 = instr & 0xFF;

        // Debugging: Print the raw opcode and instruction
        std::cout << "Instruction: " << instr << ", Raw Opcode: " << static_cast<uint32_t>(rawOpcode) << std::endl;

        switch (opcode) {
            case Opcode::ADD:
                regFile.set(r1, alu.add(regFile.get(r2), regFile.get(r3)));
                break;
            case Opcode::INC:
                regFile.set(r1, alu.inc(regFile.get(r1)));
                break;
            case Opcode::ST:
                memory.set(r1, regFile.get(r2));
                break;
            case Opcode::CALL:
                callStack.push(pc);
                pc = r1;
                break;
            case Opcode::RET:
                if (!callStack.empty()) {
                    pc = callStack.top();
                    callStack.pop();
                }
                break;
            case Opcode::LD:
                regFile.set(r1, memory.get(r2));
                break;
            default:
                std::cerr << "Unknown opcode: " << static_cast<uint32_t>(rawOpcode) << std::endl;
                exit(1);
        }
    }

    void run() {
        while (pc < memory.getSize()) {
            fetchAndExecute();
        }
    }
};

int main() {
    CPU cpu(1024);

    // Load instructions into memory
    cpu.getMemory().set(0, (static_cast<uint32_t>(Opcode::LD) << 24) | (1 << 16) | 100);  // LD r1, [100]
    cpu.getMemory().set(1, (static_cast<uint32_t>(Opcode::INC) << 24) | (1 << 16));        // INC r1
    cpu.getMemory().set(2, (static_cast<uint32_t>(Opcode::ST) << 24) | (1 << 16) | 101);   // ST [101], r1
    cpu.getMemory().set(3, (static_cast<uint32_t>(Opcode::ADD) << 24) | (2 << 16) | (1 << 8) | 1);  // ADD r2, r1, r1
    cpu.getMemory().set(4, (static_cast<uint32_t>(Opcode::CALL) << 24) | 10);              // CALL 10
    cpu.getMemory().set(10, (static_cast<uint32_t>(Opcode::RET) << 24));                   // RET

    // Run the CPU
    cpu.run();

    // Output register values
    std::cout << "Register r1: " << cpu.getRegisterFile().get(1) << std::endl;
    std::cout << "Register r2: " << cpu.getRegisterFile().get(2) << std::endl;

    return 0;
}

