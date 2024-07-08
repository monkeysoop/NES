#include "cpu.h"

#define MEM_SIZE (64 * 1024)

uint8_t memory[MEM_SIZE] = {0};

static inline uint8_t GetCarryFlag(Registers* registers) {
    return registers->status_flags & CARRY;
}

static inline uint8_t GetZeroFlag(Registers* registers) {
    return registers->status_flags & ZERO;
}

static inline uint8_t GetIrqDisableFlag(Registers* registers) {
    return registers->status_flags & IRQ_DISABLE;
}

static inline uint8_t GetDecimalModeFlag(Registers* registers) {
    return registers->status_flags & DECIMAL_MODE;
}

static inline uint8_t GetBrkCommandFlag(Registers* registers) {
    return registers->status_flags & BRK_COMMAND;
}

static inline uint8_t GetUnusedFlag(Registers* registers) {
    return registers->status_flags & UNUSED;
}

static inline uint8_t GetOverflowFlag(Registers* registers) {
    return registers->status_flags & OVERFLOW;
}

static inline uint8_t GetNegativeFlag(Registers* registers) {
    return registers->status_flags & NEGATIVE;
}



static inline uint8_t GetCarryFlagValue(Registers* registers) {
    return ((registers->status_flags & CARRY) ? 1 : 0);
}

static inline uint8_t GetZeroFlagValue(Registers* registers) {
    return ((registers->status_flags & ZERO) ? 1 : 0);
}

static inline uint8_t GetIrqDisableFlagValue(Registers* registers) {
    return ((registers->status_flags & IRQ_DISABLE) ? 1 : 0);
}

static inline uint8_t GetDecimalModeFlagValue(Registers* registers) {
    return ((registers->status_flags & DECIMAL_MODE) ? 1 : 0);
}

static inline uint8_t GetBrkCommandFlagValue(Registers* registers) {
    return ((registers->status_flags & BRK_COMMAND) ? 1 : 0);
}

static inline uint8_t GetUnusedFlagValue(Registers* registers) {
    return ((registers->status_flags & UNUSED) ? 1 : 0);
}

static inline uint8_t GetOverflowFlagValue(Registers* registers) {
    return ((registers->status_flags & OVERFLOW) ? 1 : 0);
}

static inline uint8_t GetNegativeFlagValue(Registers* registers) {
    return ((registers->status_flags & NEGATIVE) ? 1 : 0);
}



static inline void SetCarryFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= CARRY) : (registers->status_flags &= (~CARRY));    
    return;
}

static inline void SetZeroFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= ZERO) : (registers->status_flags &= (~ZERO));    
    return;
}

static inline void SetIrgDisableFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= IRQ_DISABLE) : (registers->status_flags &= (~IRQ_DISABLE));    
    return;
}

static inline void SetDecimalModeFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= DECIMAL_MODE) : (registers->status_flags &= (~DECIMAL_MODE));    
    return;
}

static inline void SetBrkCommandFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= BRK_COMMAND) : (registers->status_flags &= (~BRK_COMMAND));    
    return;
}

static inline void SetUnusedFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= UNUSED) : (registers->status_flags &= (~UNUSED));    
    return;
}

static inline void SetOverflowFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= OVERFLOW) : (registers->status_flags &= (~OVERFLOW));    
    return;
}

static inline void SetNegativeFlagValue(Registers* registers, const uint8_t value) {
    (value) ? (registers->status_flags |= NEGATIVE) : (registers->status_flags &= (~NEGATIVE));    
    return;
}


static inline void SetAllFlags(Registers* registers) {
    registers->status_flags = 0xFF;
}

static inline void ClearAllFlags(Registers* registers) {
    registers->status_flags = 0x00;
}



static inline uint8_t ReadByte(const uint16_t address) {
    return memory[address];
}

static inline uint16_t ReadBigEndianWord(const uint16_t address_start) {
    return (ReadByte(address_start) << 8) | ReadByte(address_start + 1);
} 

static inline uint16_t ReadLittleEndianWord(const uint16_t address_start) {
    return ReadByte(address_start) | (ReadByte(address_start + 1) << 8);
} 



static inline void WriteByte(const uint16_t address, const uint8_t data) {
    memory[address] = data;
}

static inline void WriteBigEndianWord(const uint16_t address, const uint16_t data) {
    WriteByte(address, (data >> 8));
    WriteByte(address + 1, (data & 0xFF));
}

static inline void WriteLittleEndianWord(const uint16_t address, const uint16_t data) {
    WriteByte(address, (data & 0xFF));
    WriteByte(address + 1, (data >> 8));
}



static inline uint8_t StackPullByte(Registers* registers) {
    registers->stack_pointer++;
    return ReadByte(STACK_OFFSET + registers->stack_pointer);
}

static inline uint16_t StackPullBigEndianWord(Registers* registers) {
    return (StackPullByte(registers) << 8) | StackPullByte(registers);
} 

static inline uint16_t StackPullLittleEndianWord(Registers* registers) {
    return StackPullByte(registers) | (StackPullByte(registers) << 8);
} 



static inline void StackPushByte(Registers* registers, const uint8_t data) {
    WriteByte(STACK_OFFSET + registers->stack_pointer, data);
    registers->stack_pointer--;
}

static inline void StackPushBigEndianWord(Registers* registers, uint16_t data) {
    // 6502 stack goes from higher address to lower (right to left)
    StackPushByte(registers, (data & 0xFF));
    StackPushByte(registers, (data >> 8));
}

static inline void StackPushLittleEndianWord(Registers* registers, uint16_t data) {
    // 6502 stack goes from higher address to lower (right to left)
    StackPushByte(registers, (data >> 8));
    StackPushByte(registers, (data & 0xFF));
}



static void Reset(Registers* registers, uint8_t* remaining_cycles) {
    registers->program_counter = ReadLittleEndianWord(RESET_OFFSET);
    
    registers->a_register = 0x00;
    registers->x_register = 0x00;
    registers->y_register = 0x00;
    registers->stack_pointer = 0xFD;

    ClearAllFlags(registers);
    SetUnusedFlagValue(registers, 1);

    (*remaining_cycles) = 8;
}



static void InterruptRequest(Registers* registers, uint8_t* remaining_cycles) {
    if (GetIrqDisableFlagValue(registers) == 0) {
        StackPushLittleEndianWord(registers, registers->program_counter);

        SetBrkCommandFlagValue(registers, 0);
        SetIrgDisableFlagValue(registers, 1);
        SetUnusedFlagValue(registers, 1);

        StackPushByte(registers, registers->status_flags);

        registers->program_counter = ReadLittleEndianWord(BREAK_INTERRUPT_OFFSET);

        (*remaining_cycles) = 7;
    }
}

static void NonMaskableInterrupt(Registers* registers, uint8_t* remaining_cycles) {
    StackPushLittleEndianWord(registers, registers->program_counter);

    SetBrkCommandFlagValue(registers, 0);
    SetIrgDisableFlagValue(registers, 1);
    SetUnusedFlagValue(registers, 1);

    StackPushByte(registers, registers->status_flags);

    registers->program_counter = ReadLittleEndianWord(NON_MASKABLE_INTERRUPT_OFFSET);

    (*remaining_cycles) = 8;
}

static void tick(Instruction instructions[256], Registers* registers, uint8_t* remaining_cycles, uint64_t* tick_counter, FILE* log_file) {
    if ((*remaining_cycles) == 0) {
        uint8_t op_code = ReadByte(registers->program_counter);
        registers->program_counter++;
        

        Instruction instruction = instructions[op_code];
        
        fprintf(log_file, 
                "clock: %lu  -  mnemonic: %s  -  opcode: 0x%0x  -  A: 0x%02x  -  X: 0x%02x  -  Y: 0x%02x  -  Status: 0x%02x  -  Stack pointer: 0x%02x  -  Program counter: 0x%04x\n", 
                (*tick_counter), instruction.mnemonic, op_code, registers->a_register, registers->x_register, registers->y_register, registers->status_flags, registers->stack_pointer, registers->program_counter);
        
        (*remaining_cycles) = instruction.cycles;
        uint16_t absolute_address = instruction.address_mode(registers, remaining_cycles);
        printf("mnemonic: %s\n", instruction.mnemonic);
        printf("abs addr: %04x\n", absolute_address);
        instruction.operator(registers, remaining_cycles, absolute_address);

    }

    (*tick_counter)++;
    (*remaining_cycles)--;
}



uint16_t IlligalMode(Registers* registers, uint8_t* remaining_cycles) {
    printf("illigal addressing mode\n");
    exit(1);
    return 0;
}
uint16_t Accumulator(Registers* registers, uint8_t* remaining_cycles) {
    return 0;   // not used because Accumulator addressing takes no value from memory 
}
uint16_t Implied(Registers* registers, uint8_t* remaining_cycles) {
	return 0;   // not used because Implied addressing takes no value from memory 
}
uint16_t Immediate(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t absolute_address = registers->program_counter;
    registers->program_counter++;

	return absolute_address;
}
uint16_t ZeroPage(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t absolute_address = (uint16_t)ReadByte(registers->program_counter);	
	registers->program_counter++;

	return absolute_address;
}
uint16_t ZeroPageX(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t absolute_address = ((uint16_t)ReadByte(registers->program_counter) + (uint16_t)registers->x_register) & 0x00FF;
	registers->program_counter++;

	return absolute_address;
}
uint16_t ZeroPageY(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t absolute_address = ((uint16_t)ReadByte(registers->program_counter) + (uint16_t)registers->y_register) & 0x00FF;
	registers->program_counter++;

	return absolute_address;
}
uint16_t Relative(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t relative_address = (uint16_t)ReadByte(registers->program_counter);
	registers->program_counter++;

	if (relative_address & 0x0080) {
		relative_address |= 0xFF00;
    }

    uint16_t absolute_address = registers->program_counter + relative_address;
	
    return absolute_address;
}
uint16_t Absolute(Registers* registers, uint8_t* remaining_cycles) {
    uint16_t absolute_address = ReadLittleEndianWord(registers->program_counter);
    registers->program_counter += 2;

	return absolute_address;
}
uint16_t AbsoluteX(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t temp_address = ReadLittleEndianWord(registers->program_counter);
    registers->program_counter += 2;
    
    uint16_t absolute_address = temp_address + (uint16_t)registers->x_register;

    if ((temp_address ^ absolute_address) >> 8) {
        (*remaining_cycles)++;
    }

    return absolute_address;
}
uint16_t AbsoluteY(Registers* registers, uint8_t* remaining_cycles) {
	uint16_t temp_address = ReadLittleEndianWord(registers->program_counter);
    registers->program_counter += 2;
	
    uint16_t absolute_address = temp_address + (uint16_t)registers->y_register;

	if ((temp_address ^ absolute_address) >> 8) {
        (*remaining_cycles)++;
    }
 
    return absolute_address;
}
uint16_t Indirect(Registers* registers, uint8_t* remaining_cycles) {
    uint16_t ptr = ReadLittleEndianWord(registers->program_counter);
    registers->program_counter += 2;

    uint16_t absolute_address;
	if ((ptr & 0x00FF) == 0x00FF) {
		absolute_address = (uint16_t)(ReadByte(ptr & 0xFF00) << 8) | (uint16_t)ReadByte(ptr);   // known hardwer bug in 6502
	} else {
		absolute_address = ReadLittleEndianWord(ptr);
	}
	
	return absolute_address;
}
uint16_t IndirectX(Registers* registers, uint8_t* remaining_cycles) {
	uint8_t ptr = ReadByte(registers->program_counter);
	registers->program_counter++;
    
    uint16_t absolute_address = (uint16_t)(ReadByte(((uint16_t)ptr + (uint16_t)registers->x_register + 1) & 0x00FF) << 8) 
                              | (uint16_t)ReadByte(((uint16_t)ptr + (uint16_t)registers->x_register) & 0x00FF);
	
    return absolute_address;
}
uint16_t IndirectY(Registers* registers, uint8_t* remaining_cycles) {
    uint8_t ptr = ReadByte(registers->program_counter);
    registers->program_counter++;

    uint16_t temp_address = (uint16_t)(ReadByte(((uint16_t)ptr + 1) & 0x00FF) << 8) | (uint16_t)ReadByte(ptr);

    uint16_t absolute_address = temp_address + registers->y_register;

	
	if ((temp_address ^ absolute_address) >> 8) {
        (*remaining_cycles)++;
    }

    return absolute_address;
}







void ILL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    printf("illigal opcode\n");
    exit(1);
    return;
}

void ASL_ACC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = registers->a_register;
    SetCarryFlagValue(registers, (temp & 0x80));
    temp <<= 1;
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    registers->a_register = temp;    
    return;    
}
void ROL_ACC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = registers->a_register;
    uint8_t temp_carry = GetCarryFlag(registers);
    SetCarryFlagValue(registers, (temp & 0x80));
    temp <<= 1;
    temp |= temp_carry;
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    registers->a_register = temp;
    return;
}
void LSR_ACC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = registers->a_register;
    SetCarryFlagValue(registers, (temp & 0x01));
    temp >>= 1;
    SetNegativeFlagValue(registers, 0);
    SetZeroFlagValue(registers, (!temp));
    registers->a_register = temp;
    return;
}
void ROR_ACC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = registers->a_register;
    uint8_t temp_carry = GetCarryFlagValue(registers);
    SetCarryFlagValue(registers, (temp & 0x01));
    temp >>= 1;
    temp |= (temp_carry << 7);
    SetNegativeFlagValue(registers, temp_carry);
    SetZeroFlagValue(registers, (!temp));
    registers->a_register = temp;
    return;
}

void BRK(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    /**/
    StackPushLittleEndianWord(registers, registers->program_counter);
    
    SetBrkCommandFlagValue(registers, 1);
    StackPushByte(registers, registers->status_flags);
    SetBrkCommandFlagValue(registers, 0);
    
    SetIrgDisableFlagValue(registers, 1);

    registers->program_counter = ReadLittleEndianWord(BREAK_INTERRUPT_OFFSET);
    return;
}
void ORA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);

    temp |= registers->a_register;

    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));

    registers->a_register = temp;
    return;
}
void ASL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    SetCarryFlagValue(registers, (temp & 0x80));
    temp <<= 1;
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    WriteByte(absolute_address, temp);
    return; 
}
void PHP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    /**/
    StackPushByte(registers, registers->status_flags | UNUSED | BRK_COMMAND);
    return;
}
void BPL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetNegativeFlagValue(registers) == 0) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void CLC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetCarryFlagValue(registers, 0);
    return;
}
void JSR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    StackPushLittleEndianWord(registers, (registers->program_counter - 1));
    registers->program_counter = absolute_address;
    return;
}
void AND(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);

    temp &= registers->a_register;

    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));

    registers->a_register = temp;
    return;
}
void BIT(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);

    temp &= registers->a_register;

    SetOverflowFlagValue(registers, (temp & 0x40));
    
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    return;
}
void ROL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint8_t temp_carry = GetCarryFlag(registers);
    SetCarryFlagValue(registers, (temp & 0x80));
    temp <<= 1;
    temp |= temp_carry;
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    WriteByte(absolute_address, temp);
    return;
}
void PLP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    /**/
    registers->status_flags = StackPullByte(registers);
    SetUnusedFlagValue(registers, 1);
    SetBrkCommandFlagValue(registers, 0); // on the actual 6502 cpu there is no place for this flag so it's ignored
    return;
}
void BMI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetNegativeFlagValue(registers) == 1) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void SEC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetCarryFlagValue(registers, 1);
    return;
}
void RTI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    /**/
    registers->status_flags = StackPullByte(registers);
    SetUnusedFlagValue(registers, 1);
    SetBrkCommandFlagValue(registers, 0); // on the actual 6502 cpu there is no place for this flag so it's ignored
    
    registers->program_counter = StackPullLittleEndianWord(registers);
    return;
}
void EOR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);

    temp ^= registers->a_register;

    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));

    registers->a_register = temp;
    return;
}
void LSR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    SetCarryFlagValue(registers, (temp & 0x01));
    temp >>= 1;
    SetNegativeFlagValue(registers, 0);
    SetZeroFlagValue(registers, (!temp));
    WriteByte(absolute_address, temp);
    return;
}
void PHA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    StackPushByte(registers, registers->a_register);
    return;
}
void JMP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->program_counter = absolute_address;
    return;
}
void BVC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetOverflowFlagValue(registers) == 0) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void CLI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetIrgDisableFlagValue(registers, 0);
    return;
}
void RTS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->program_counter = StackPullLittleEndianWord(registers) + 1;
    return;
}
void ADC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint16_t res = (uint16_t)registers->a_register + (uint16_t)temp + (uint16_t)GetCarryFlagValue(registers);

    SetZeroFlagValue(registers, (res & 0x00FF));

    if (GetDecimalModeFlagValue(registers)) {
        if (((registers->a_register & 0x0F) + (temp & 0x0F) + GetCarryFlagValue(registers)) > 0x09) {
            res += 6;
        }

        SetNegativeFlagValue(registers, (res & 0x0080));
        SetOverflowFlagValue(registers, (!((registers->a_register ^ temp) & 0x80) && ((registers->a_register ^ (uint8_t)res) & 0x80)));
        
        if (res > 0x0099) {
            res += 96;
        }

        SetCarryFlagValue(registers, (res > 0x0099));

        (*remaining_cycles)++;
    } else {
        SetNegativeFlagValue(registers, (res & 0x80));
        SetOverflowFlagValue(registers, (!((registers->a_register ^ temp) & 0x80) && ((registers->a_register ^ (uint8_t)res) & 0x80)));
        SetCarryFlagValue(registers, (res > 0x00FF));
    }

    registers->a_register = (uint8_t)(res & 0x00FF);
    return;
}
void ROR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint8_t temp_carry = GetCarryFlagValue(registers);
    SetCarryFlagValue(registers, (temp & 0x01));
    temp >>= 1;
    temp |= (temp_carry << 7);
    SetNegativeFlagValue(registers, temp_carry);
    SetZeroFlagValue(registers, (!temp));
    WriteByte(absolute_address, temp);
    return;
}
void PLA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = StackPullByte(registers);
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    registers->a_register = temp;
    return;
}
void BVS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetOverflowFlagValue(registers) == 1) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void SEI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetIrgDisableFlagValue(registers, 1);
    return;
}
void STA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    WriteByte(absolute_address, registers->a_register);
    return;
}
void STY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    WriteByte(absolute_address, registers->y_register);
    return;
}
void STX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    WriteByte(absolute_address, registers->x_register);
    return;
}
void DEY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->y_register--;
    SetNegativeFlagValue(registers, (registers->y_register & 0x80));
    SetZeroFlagValue(registers, (!registers->y_register));
    return;
}
void TXA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->a_register = registers->x_register;
    SetNegativeFlagValue(registers, (registers->a_register & 0x80));
    SetZeroFlagValue(registers, (!registers->a_register));
    return;
}
void BCC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetCarryFlagValue(registers) == 0) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void TYA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->a_register = registers->y_register;
    SetNegativeFlagValue(registers, (registers->a_register & 0x80));
    SetZeroFlagValue(registers, (!registers->a_register));
    return;
}
void TXS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->stack_pointer = registers->x_register;
    return;
}
void LDY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->y_register = ReadByte(absolute_address);
    SetNegativeFlagValue(registers, (registers->y_register & 0x80));
    SetZeroFlagValue(registers, (!registers->y_register));
    return;
}
void LDA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->a_register = ReadByte(absolute_address);
    SetNegativeFlagValue(registers, (registers->a_register & 0x80));
    SetZeroFlagValue(registers, (!registers->a_register));
    return;
}
void LDX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->x_register = ReadByte(absolute_address);
    SetNegativeFlagValue(registers, (registers->x_register & 0x80));
    SetZeroFlagValue(registers, (!registers->x_register));
    return;
}
void TAY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->y_register = registers->a_register;
    SetNegativeFlagValue(registers, (registers->y_register & 0x80));
    SetZeroFlagValue(registers, (!registers->y_register));
    return;
}
void TAX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->x_register = registers->a_register;
    SetNegativeFlagValue(registers, (registers->x_register & 0x80));
    SetZeroFlagValue(registers, (!registers->x_register));
    return;
}
void BCS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetCarryFlagValue(registers) == 1) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void CLV(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetOverflowFlagValue(registers, 0);
}
void TSX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->x_register = registers->stack_pointer;
    SetNegativeFlagValue(registers, (registers->x_register & 0x80));
    SetZeroFlagValue(registers, (!registers->x_register));
    return;
}
void CPY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint16_t cmp = (uint16_t)registers->y_register - (uint16_t)temp;
    SetCarryFlagValue(registers, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(registers, (cmp & 0x0080));
    SetZeroFlagValue(registers, (!(cmp & 0x00FF)));
    return;
}
void CMP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint16_t cmp = (uint16_t)registers->a_register - (uint16_t)temp;
    SetCarryFlagValue(registers, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(registers, (cmp & 0x0080));
    SetZeroFlagValue(registers, (!(cmp & 0x00FF)));
    return;
}
void DEC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    temp--;
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    WriteByte(absolute_address, temp);
    return;
}
void INY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->y_register++;
    SetNegativeFlagValue(registers, (registers->y_register & 0x80));
    SetZeroFlagValue(registers, (!registers->y_register));
    return;
}
void DEX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->x_register--;
    SetNegativeFlagValue(registers, (registers->x_register & 0x80));
    SetZeroFlagValue(registers, (!registers->x_register));
    return;
}
void BNE(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetZeroFlagValue(registers) == 0) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void CLD(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetDecimalModeFlagValue(registers, 0);
    return;
}
void CPX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint16_t cmp = (uint16_t)registers->x_register - (uint16_t)temp;
    SetCarryFlagValue(registers, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(registers, (cmp & 0x0080));
    SetZeroFlagValue(registers, (!(cmp & 0x00FF)));
    return;
}
void SBC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    uint16_t res = (uint16_t)registers->a_register - (uint16_t)temp + (uint16_t)GetCarryFlagValue(registers) - 1;

    SetNegativeFlagValue(registers, (res & 0x0080));
    SetZeroFlagValue(registers, (!(res & 0x00FF)));
    SetOverflowFlagValue(registers, (((registers->a_register ^ temp) & 0x80) && ((registers->a_register ^ (uint8_t)res) & 0x80)));

    if (GetDecimalModeFlagValue(registers)) {
        if (((registers->a_register & 0x0F) + GetCarryFlagValue(registers) - 1) < (temp & 0x0F)) {
            res -= 6;
        }

        
        if (res > 0x0099) {
            res -= 0x60;
        }

        (*remaining_cycles)++;
    }

    SetCarryFlagValue(registers, (!(res & 0xFF00)));
    registers->a_register = (uint8_t)(res & 0x00FF);
    return;
}
void INC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(absolute_address);
    temp++;
    SetNegativeFlagValue(registers, (temp & 0x80));
    SetZeroFlagValue(registers, (!temp));
    WriteByte(absolute_address, temp);
    return;
}
void INX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    registers->x_register++;
    SetNegativeFlagValue(registers, (registers->x_register & 0x80));
    SetZeroFlagValue(registers, (!registers->x_register));
    return;
}
void NOP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    return;
}
void BEQ(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    if (GetZeroFlagValue(registers) == 1) {
        (*remaining_cycles)++;

        if ((registers->program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            (*remaining_cycles)++;
        }

        registers->program_counter = absolute_address;
    }
    return;
}
void SED(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address) {
    SetDecimalModeFlagValue(registers, 1);
    return;
}










int main() {

    // registers
    Registers registers = {
        .a_register = 0x00,
	    .x_register = 0x00, 
	    .y_register = 0x00,   
        .status_flags = 0x00,
        .stack_pointer = 0x00, 
	    .program_counter = 0x0000
    };

    uint8_t remaining_cycles = 0;
    uint64_t tick_counter = 0;

    Instruction instructions[256] = {
        { .mnemonic="BRK", .operator=&BRK, .address_mode=&Immediate, .cycles=7 },   // ????????????????????????,
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//3 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="ASL", .operator=&ASL, .address_mode=&ZeroPage, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="PHP", .operator=&PHP, .address_mode=&Implied, .cycles=3 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="ASL", .operator=&ASL_ACC, .address_mode=&Accumulator, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="ASL", .operator=&ASL, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        
		{ .mnemonic="BPL", .operator=&BPL, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="ASL", .operator=&ASL, .address_mode=&ZeroPageX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="CLC", .operator=&CLC, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="ORA", .operator=&ORA, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="ASL", .operator=&ASL, .address_mode=&AbsoluteX, .cycles=7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
		
        { .mnemonic="JSR", .operator=&JSR, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="BIT", .operator=&BIT, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="ROL", .operator=&ROL, .address_mode=&ZeroPage, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="PLP", .operator=&PLP, .address_mode=&Implied, .cycles=4 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="ROL", .operator=&ROL_ACC, .address_mode=&Accumulator, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="BIT", .operator=&BIT, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="ROL", .operator=&ROL, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
		
        { .mnemonic="BMI", .operator=&BMI, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="ROL", .operator=&ROL, .address_mode=&ZeroPageX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="SEC", .operator=&SEC, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="AND", .operator=&AND, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="ROL", .operator=&ROL, .address_mode=&AbsoluteX, .cycles=7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
		
        { .mnemonic="RTI", .operator=&RTI, .address_mode=&Implied, .cycles=6 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//3 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="LSR", .operator=&LSR, .address_mode=&ZeroPage, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="PHA", .operator=&PHA, .address_mode=&Implied, .cycles=3 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="LSR", .operator=&LSR_ACC, .address_mode=&Accumulator, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="JMP", .operator=&JMP, .address_mode=&Absolute, .cycles=3 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="LSR", .operator=&LSR, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
		
        { .mnemonic="BVC", .operator=&BVC, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="LSR", .operator=&LSR, .address_mode=&ZeroPageX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="CLI", .operator=&CLI, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="EOR", .operator=&EOR, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="LSR", .operator=&LSR, .address_mode=&AbsoluteX, .cycles=7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
		
        { .mnemonic="RTS", .operator=&RTS, .address_mode=&Implied, .cycles=6 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//3 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="ROR", .operator=&ROR, .address_mode=&ZeroPage, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="PLA", .operator=&PLA, .address_mode=&Implied, .cycles=4 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="ROR", .operator=&ROR_ACC, .address_mode=&Accumulator, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="JMP", .operator=&JMP, .address_mode=&Indirect, .cycles=5 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="ROR", .operator=&ROR, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
		
        { .mnemonic="BVS", .operator=&BVS, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="ROR", .operator=&ROR, .address_mode=&ZeroPageX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="SEI", .operator=&SEI, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="ADC", .operator=&ADC, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="ROR", .operator=&ROR, .address_mode=&AbsoluteX, .cycles=7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
		
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="STY", .operator=&STY, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="STX", .operator=&STX, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//3 },
        { .mnemonic="DEY", .operator=&DEY, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="TXA", .operator=&TXA, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="STY", .operator=&STY, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="STX", .operator=&STX, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
		
        { .mnemonic="BCC", .operator=&BCC, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&IndirectY, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="STY", .operator=&STY, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="STX", .operator=&STX, .address_mode=&ZeroPageY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="TYA", .operator=&TYA, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&AbsoluteY, .cycles=5 },
        { .mnemonic="TXS", .operator=&TXS, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="STA", .operator=&STA, .address_mode=&AbsoluteX, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
		
        { .mnemonic="LDY", .operator=&LDY, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="LDX", .operator=&LDX, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="LDY", .operator=&LDY, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="LDX", .operator=&LDX, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//3 },
        { .mnemonic="TAY", .operator=&TAY, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="TAX", .operator=&TAX, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="LDY", .operator=&LDY, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="LDX", .operator=&LDX, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
		
        { .mnemonic="BCS", .operator=&BCS, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="LDY", .operator=&LDY, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="LDX", .operator=&LDX, .address_mode=&ZeroPageY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="CLV", .operator=&CLV, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="TSX", .operator=&TSX, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="LDY", .operator=&LDY, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="LDA", .operator=&LDA, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="LDX", .operator=&LDX, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
		
        { .mnemonic="CPY", .operator=&CPY, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="CPY", .operator=&CPY, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="DEC", .operator=&DEC, .address_mode=&ZeroPage, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="INY", .operator=&INY, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="DEX", .operator=&DEX, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="CPY", .operator=&CPY, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="DEC", .operator=&DEC, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
		
        { .mnemonic="BNE", .operator=&BNE, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="DEC", .operator=&DEC, .address_mode=&ZeroPageX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="CLD", .operator=&CLD, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="CMP", .operator=&CMP, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="DEC", .operator=&DEC, .address_mode=&AbsoluteX, .cycles=7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
		
        { .mnemonic="CPX", .operator=&CPX, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&IndirectX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="CPX", .operator=&CPX, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&ZeroPage, .cycles=3 },
        { .mnemonic="INC", .operator=&INC, .address_mode=&ZeroPage, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//5 },
        { .mnemonic="INX", .operator=&INX, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&Immediate, .cycles=2 },
        { .mnemonic="NOP", .operator=&NOP, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="CPX", .operator=&CPX, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&Absolute, .cycles=4 },
        { .mnemonic="INC", .operator=&INC, .address_mode=&Absolute, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
		
        { .mnemonic="BEQ", .operator=&BEQ, .address_mode=&Relative, .cycles=2 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&IndirectY, .cycles=5 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//8 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&ZeroPageX, .cycles=4 },
        { .mnemonic="INC", .operator=&INC, .address_mode=&ZeroPageX, .cycles=6 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//6 },
        { .mnemonic="SED", .operator=&SED, .address_mode=&Implied, .cycles=2 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&AbsoluteY, .cycles=4 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//2 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//4 },
        { .mnemonic="SBC", .operator=&SBC, .address_mode=&AbsoluteX, .cycles=4 },
        { .mnemonic="INC", .operator=&INC, .address_mode=&AbsoluteX, .cycles=7 },
        { .mnemonic="???", .operator=&ILL, .address_mode=&IlligalMode, .cycles=0 },//7 }
    };


    const char* nes_filename = "nestest.nes";
    const char* log_filename = "cpu_log.txt";

    FILE* nes_file = fopen(nes_filename, "r");

    if (nes_file == NULL) {
        perror("fopen");
        exit(1);
    }

    struct stat st;
    if (stat(nes_filename, &st) == -1) {
        perror("stat");
        exit(1);
    }

    int len = (st.st_size < MEM_SIZE) ? st.st_size : MEM_SIZE;
    //fread(memory, sizeof(uint8_t), len, nes_file);
    fclose(nes_file);

    memory[0x8000] = 0xa2;
    memory[0x8001] = 0x0a;
    memory[0x8002] = 0x8e;
    memory[0x8003] = 0x00;
    memory[0x8004] = 0x00;
    memory[0x8005] = 0xa2;
    memory[0x8006] = 0x03;
    memory[0x8007] = 0x8e;

    memory[0x8008] = 0x01;
    memory[0x8009] = 0x00;
    memory[0x800a] = 0xac;
    memory[0x800b] = 0x00;
    memory[0x800c] = 0x00;
    memory[0x800d] = 0xa9;
    memory[0x800e] = 0x00;
    memory[0x800f] = 0x18;

    memory[0x8010] = 0x6d;
    memory[0x8011] = 0x01;
    memory[0x8012] = 0x00;
    memory[0x8013] = 0x88;
    memory[0x8014] = 0xd0;
    memory[0x8015] = 0xfa;
    memory[0x8016] = 0x8d;
    memory[0x8017] = 0x02;

    memory[0x8018] = 0x00;
    memory[0x8019] = 0xea;
    memory[0x801a] = 0xea;
    memory[0x801b] = 0xea;


    WriteLittleEndianWord(RESET_OFFSET, 0x8000);

    Reset(&registers, &remaining_cycles);

    FILE* log_file = fopen(log_filename, "w");
    if (nes_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 1000; i++) {
        printf("tick: %d\n", i);
        tick(instructions, &registers, &remaining_cycles, &tick_counter, log_file);
    }


    fclose(log_file);
    return 0;
}