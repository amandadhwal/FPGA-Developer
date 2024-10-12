#include <iostream>
#include <vector>
#include <stack>
#include <cstdint>

// Enum to define opcodes
enum class opcode : uint8_t {
    add = 0x01, sub = 0x02, mul = 0x03, div = 0x04,
    inc = 0x05, dec = 0x06, and_op = 0x07, or_op = 0x08,
    xor_op = 0x09, not_op = 0x0A, jmp = 0x0B, beq = 0x0C,
    bne = 0x0D, call = 0x0E, ret = 0x0F, ld = 0x10,
    st = 0x11, fft = 0x12, enc = 0x13, decrypt = 0x15
};

// Register file to manage CPU registers
class register_file {
private:
    std::vector<uint32_t> registers;

public:
    register_file(size_t size = 16) : registers(size, 0) {}

    void set(uint8_t index, uint32_t value) {
        if (index < registers.size())
            registers[index] = value;
    }

    uint32_t get(uint8_t index) const {
        return (index < registers.size()) ? registers[index] : 0;
    }
};

// Memory class to handle memory operations
class memory {
private:
    std::vector<uint32_t> mem;

public:
    memory(size_t size = 1024) : mem(size, 0) {}

    void set(uint16_t address, uint32_t value) {
        if (address < mem.size())
            mem[address] = value;
    }

    uint32_t get(uint16_t address) const {
        return (address < mem.size()) ? mem[address] : 0;
    }

    size_t size() const { return mem.size(); }
};

// ALU class to perform operations
class alu {
public:
    uint32_t add(uint32_t a, uint32_t b) { return a + b; }
    uint32_t sub(uint32_t a, uint32_t b) { return a - b; }
    uint32_t mul(uint32_t a, uint32_t b) { return a * b; }
    uint32_t div(uint32_t a, uint32_t b) { return (b != 0) ? a / b : 0; }
    uint32_t inc(uint32_t a) { return a + 1; }
    uint32_t dec(uint32_t a) { return a - 1; }
};

// CPU class to integrate all components
class cpu {
private:
    register_file reg_file;
    memory mem;
    alu alu_unit;
    uint32_t pc = 0;
    std::stack<uint32_t> call_stack;

public:
    cpu(size_t mem_size = 1024) : mem(mem_size) {}

    memory& get_memory() { return mem; }
    register_file& get_register_file() { return reg_file; }

    void fetch_and_execute() {
        uint32_t instr = mem.get(pc++);
        uint8_t raw_opcode = instr >> 24;
        opcode op = static_cast<opcode>(raw_opcode);
        uint8_t r1 = (instr >> 16) & 0xFF;
        uint8_t r2 = (instr >> 8) & 0xFF;
        uint8_t r3 = instr & 0xFF;

        std::cout << "Instruction: " << instr
                  << ", Raw Opcode: " << static_cast<uint32_t>(raw_opcode) << std::endl;

        switch (op) {
            case opcode::add:
                reg_file.set(r1, alu_unit.add(reg_file.get(r2), reg_file.get(r3)));
                break;
            case opcode::inc:
                reg_file.set(r1, alu_unit.inc(reg_file.get(r1)));
                break;
            case opcode::st:
                mem.set(r1, reg_file.get(r2));
                break;
            case opcode::call:
                call_stack.push(pc);
                pc = r1;
                break;
            case opcode::ret:
                if (!call_stack.empty()) {
                    pc = call_stack.top();
                    call_stack.pop();
                }
                break;
            case opcode::ld:
                reg_file.set(r1, mem.get(r2));
                break;
            default:
                std::cerr << "Unknown opcode: " << static_cast<uint32_t>(raw_opcode) << std::endl;
                exit(1);
        }
    }

    void run() {
        while (pc < mem.size()) {
            fetch_and_execute();
        }
    }
};

int main() {
    cpu cpu_unit(1024);

    // Load instructions into memory
    cpu_unit.get_memory().set(0, (static_cast<uint32_t>(opcode::ld) << 24) | (1 << 16) | 100);  // LD r1, [100]
    cpu_unit.get_memory().set(1, (static_cast<uint32_t>(opcode::inc) << 24) | (1 << 16));        // INC r1
    cpu_unit.get_memory().set(2, (static_cast<uint32_t>(opcode::st) << 24) | (1 << 16) | 101);   // ST [101], r1
    cpu_unit.get_memory().set(3, (static_cast<uint32_t>(opcode::add) << 24) | (2 << 16) | (1 << 8) | 1);  // ADD r2, r1, r1
    cpu_unit.get_memory().set(4, (static_cast<uint32_t>(opcode::call) << 24) | 10);              // CALL 10
    cpu_unit.get_memory().set(10, (static_cast<uint32_t>(opcode::ret) << 24));                   // RET

    // Run the CPU
    cpu_unit.run();

    // Output register values
    std::cout << "Register r1: " << cpu_unit.get_register_file().get(1) << std::endl;
    std::cout << "Register r2: " << cpu_unit.get_register_file().get(2) << std::endl;

    return 0;
}
