#include <iostream>
#include <stack>
#include <vector>

enum Opcode {
    LOAD, STORE, MOV, PUSH, POP,      // Data Management
    ADD, SUB, MUL, DIV, INC, DEC,     // Arithmetic Operations
    JMP, JZ, JNZ, CALL, RET,          // Control Flow
    CMP, AND, OR, XOR, NOT,           // Logical Operations
    INT, HLT, NOP,                    // System Management
    IN, OUT                           // I/O
};

struct VM {
    int memory[65536];   // 64K memory
    int registers[8];    // 8 registers: R0 to R7
    int ip;              // Instruction Pointer
    int sp;              // Stack Pointer
    bool running;        // Status of VM
    std::stack<int> stack; // Stack

    VM() : ip(0), sp(0), running(true) {
        std::fill_n(memory, 65536, 0);
        std::fill_n(registers, 8, 0);
    }
};

int fetch_next(VM &vm) {
    return vm.memory[vm.ip++];
}

void execute(VM &vm, int instruction) {
    switch (instruction) {
        case LOAD: {
            int reg = fetch_next(vm);  // Register
            int addr = fetch_next(vm); // Memory address
            vm.registers[reg] = vm.memory[addr];
            break;
        }
        case STORE: {
            int reg = fetch_next(vm);
            int addr = fetch_next(vm);
            vm.memory[addr] = vm.registers[reg];
            break;
        }
        case MOV: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] = vm.registers[reg2];
            break;
        }
        case PUSH: {
            int reg = fetch_next(vm);
            vm.stack.push(vm.registers[reg]);
            break;
        }
        case POP: {
            int reg = fetch_next(vm);
            if (!vm.stack.empty()) {
                vm.registers[reg] = vm.stack.top();
                vm.stack.pop();
            }
            break;
        }
        case ADD: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] += vm.registers[reg2];
            break;
        }
        case SUB: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] -= vm.registers[reg2];
            break;
        }
        case MUL: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] *= vm.registers[reg2];
            break;
        }
        case DIV: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            if (vm.registers[reg2] != 0)
                vm.registers[reg1] /= vm.registers[reg2];
            break;
        }
        case INC: {
            int reg = fetch_next(vm);
            vm.registers[reg]++;
            break;
        }
        case DEC: {
            int reg = fetch_next(vm);
            vm.registers[reg]--;
            break;
        }
        case JMP: {
            int addr = fetch_next(vm);
            vm.ip = addr;
            break;
        }
        case JZ: {
            int addr = fetch_next(vm);
            if (vm.registers[0] == 0)
                vm.ip = addr;
            break;
        }
        case JNZ: {
            int addr = fetch_next(vm);
            if (vm.registers[0] != 0)
                vm.ip = addr;
            break;
        }
        case CALL: {
            int addr = fetch_next(vm);
            vm.stack.push(vm.ip); // Save current ip
            vm.ip = addr;
            break;
        }
        case RET: {
            if (!vm.stack.empty()) {
                vm.ip = vm.stack.top();
                vm.stack.pop();
            }
            break;
        }
        case CMP: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[0] = (vm.registers[reg1] == vm.registers[reg2]) ? 1 : 0;
            break;
        }
        case AND: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] &= vm.registers[reg2];
            break;
        }
        case OR: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] |= vm.registers[reg2];
            break;
        }
        case XOR: {
            int reg1 = fetch_next(vm);
            int reg2 = fetch_next(vm);
            vm.registers[reg1] ^= vm.registers[reg2];
            break;
        }
        case NOT: {
            int reg = fetch_next(vm);
            vm.registers[reg] = ~vm.registers[reg];
            break;
        }
        case INT: {
            int interrupt = fetch_next(vm);
            std::cout << "Interrupt: " << interrupt << std::endl;
            break;
        }
        case HLT: {
            vm.running = false;
            break;
        }
        case NOP: {
            break;
        }
        case IN: {
            int reg = fetch_next(vm);
            int port = fetch_next(vm);
            std::cout << "Input from port " << port << ": ";
            std::cin >> vm.registers[reg];
            break;
        }
        case OUT: {
            int port = fetch_next(vm);
            int reg = fetch_next(vm);
            std::cout << "Output to port " << port << ": " << vm.registers[reg] << std::endl;
            break;
        }
    }
}

void run(VM &vm) {
    while (vm.running) {
        int instruction = fetch_next(vm);
        execute(vm, instruction);
    }
}

int main() {
    VM vm;
    
    // Example program: load value into register 0, increment it, and output it
    int program[] = {
        LOAD, 5, 5,
        INC, 5,
        STORE, 5, 5,
        LOAD, 0, 10,    // Load memory[10] into R0
        LOAD, 5, 5,     // Load memory[5] into R5
        ADD, 5, 0,       // Add RO+R5
        INC, 0,         // Increment R0
        OUT, 1, 0,      // Output R0 to port 1
        OUT, 2, 5,
        HLT             // Halt
    };
    // Load the program into memory
    for (int i = 0; i < sizeof(program) / sizeof(program[0]); i++) {
        vm.memory[i] = program[i];
    }

    // Initialize memory location 10
    vm.memory[10] = 5;  // Let's assume memory[10] holds the value 5
    vm.memory[5] = 4;
    run(vm);

    return 0;
}
