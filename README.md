# FPGA-Developer
Opcode Enum: Defines various operations like ADD, INC, CALL, etc., using unique byte codes.
Register File: Stores CPU registers, allowing get and set operations.
Memory Class: Manages memory with functions to store and retrieve values by address.
ALU (Arithmetic Logic Unit): Performs arithmetic operations like add, sub, mul, inc, etc.
CPU Class:
Integrates registers, memory, ALU, and call stack.
Fetches and executes instructions from memory.
Supports function calls with CALL and RET instructions.
Main Function:
Loads instructions into memory.
Runs the CPU and displays register values after execution.
