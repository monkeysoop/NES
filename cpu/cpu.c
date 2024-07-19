#include "cpu.h"


//uint8_t memory[65536];




static inline uint8_t GetCarryFlag(CPU* cpu) {
    return cpu->registers.status_flags & CARRY;
}
static inline uint8_t GetZeroFlag(CPU* cpu) {
    return cpu->registers.status_flags & ZERO;
}
static inline uint8_t GetIrqDisableFlag(CPU* cpu) {
    return cpu->registers.status_flags & IRQ_DISABLE;
}
static inline uint8_t GetDecimalModeFlag(CPU* cpu) {
    return cpu->registers.status_flags & DECIMAL_MODE;
}
static inline uint8_t GetBrkCommandFlag(CPU* cpu) {
    return cpu->registers.status_flags & BRK_COMMAND;
}
static inline uint8_t GetUnusedFlag(CPU* cpu) {
    return cpu->registers.status_flags & UNUSED;
}
static inline uint8_t GetOverflowFlag(CPU* cpu) {
    return cpu->registers.status_flags & OVERFLOW;
}
static inline uint8_t GetNegativeFlag(CPU* cpu) {
    return cpu->registers.status_flags & NEGATIVE;
}



static inline uint8_t GetCarryFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & CARRY) ? 1 : 0);
}
static inline uint8_t GetZeroFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & ZERO) ? 1 : 0);
}
static inline uint8_t GetIrqDisableFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & IRQ_DISABLE) ? 1 : 0);
}
static inline uint8_t GetDecimalModeFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & DECIMAL_MODE) ? 1 : 0);
}
static inline uint8_t GetBrkCommandFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & BRK_COMMAND) ? 1 : 0);
}
static inline uint8_t GetUnusedFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & UNUSED) ? 1 : 0);
}
static inline uint8_t GetOverflowFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & OVERFLOW) ? 1 : 0);
}
static inline uint8_t GetNegativeFlagValue(CPU* cpu) {
    return ((cpu->registers.status_flags & NEGATIVE) ? 1 : 0);
}



static inline void SetCarryFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= CARRY) : (cpu->registers.status_flags &= (~CARRY));    
    return;
}



static inline void SetZeroFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= ZERO) : (cpu->registers.status_flags &= (~ZERO));    
    return;
}
static inline void SetIrgDisableFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= IRQ_DISABLE) : (cpu->registers.status_flags &= (~IRQ_DISABLE));    
    return;
}
static inline void SetDecimalModeFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= DECIMAL_MODE) : (cpu->registers.status_flags &= (~DECIMAL_MODE));    
    return;
}
static inline void SetBrkCommandFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= BRK_COMMAND) : (cpu->registers.status_flags &= (~BRK_COMMAND));    
    return;
}
static inline void SetUnusedFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= UNUSED) : (cpu->registers.status_flags &= (~UNUSED));    
    return;
}
static inline void SetOverflowFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= OVERFLOW) : (cpu->registers.status_flags &= (~OVERFLOW));    
    return;
}
static inline void SetNegativeFlagValue(CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= NEGATIVE) : (cpu->registers.status_flags &= (~NEGATIVE));    
    return;
}



static inline void SetAllFlags(CPU* cpu) {
    cpu->registers.status_flags = 0xFF;
}
static inline void ClearAllFlags(CPU* cpu) {
    cpu->registers.status_flags = 0x00;
}



static inline uint8_t ReadByte(CPU* cpu, const uint16_t address) {
    return ReadCPUBus(cpu->cpu_bus, address);
}
static inline void WriteByte(CPU* cpu, const uint16_t address, const uint8_t data) {
    WriteCPUBus(cpu->cpu_bus, address, data);
}

static inline uint16_t ReadBigEndianWord(CPU* cpu, const uint16_t address_start) {
    return (ReadByte(cpu, address_start) << 8) | ReadByte(cpu, (address_start + 1));
} 
static inline uint16_t ReadLittleEndianWord(CPU* cpu, const uint16_t address_start) {
    return ReadByte(cpu, address_start) | (ReadByte(cpu, (address_start + 1)) << 8);
}
static inline void WriteBigEndianWord(CPU* cpu, const uint16_t address, const uint16_t data) {
    WriteByte(cpu, address, (data >> 8));
    WriteByte(cpu, (address + 1), (data & 0xFF));
}
static inline void WriteLittleEndianWord(CPU* cpu, const uint16_t address, const uint16_t data) {
    WriteByte(cpu, address, (data & 0xFF));
    WriteByte(cpu, (address + 1), (data >> 8));
}



static inline uint8_t StackPullByte(CPU* cpu) {
    cpu->registers.stack_pointer++;
    return ReadByte(cpu, (STACK_OFFSET + cpu->registers.stack_pointer));
}
static inline void StackPushByte(CPU* cpu, const uint8_t data) {
    WriteByte(cpu, (STACK_OFFSET + cpu->registers.stack_pointer), data);
    cpu->registers.stack_pointer--;
}

static inline uint16_t StackPullBigEndianWord(CPU* cpu) {
    return (StackPullByte(cpu) << 8) | StackPullByte(cpu);
} 
static inline uint16_t StackPullLittleEndianWord(CPU* cpu) {
    return StackPullByte(cpu) | (StackPullByte(cpu) << 8);
}
static inline void StackPushBigEndianWord(CPU* cpu, uint16_t data) {
    // 6502 stack goes from higher address to lower (right to left)
    StackPushByte(cpu, (data & 0xFF));
    StackPushByte(cpu, (data >> 8));
}
static inline void StackPushLittleEndianWord(CPU* cpu, uint16_t data) {
    // 6502 stack goes from higher address to lower (right to left)
    StackPushByte(cpu, (data >> 8));
    StackPushByte(cpu, (data & 0xFF));
}






static uint16_t IlligalMode(CPU* cpu) {
    printf("illigal addressing mode\n");
    exit(1);
    return 0;
}
static uint16_t Accumulator(CPU* cpu) {
    return 0;   // not used because Accumulator addressing takes no value from memory 
}
static uint16_t Implied(CPU* cpu) {
	return 0;   // not used because Implied addressing takes no value from memory 
}
static uint16_t Immediate(CPU* cpu) {
	uint16_t absolute_address = cpu->registers.program_counter;
    cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t ZeroPage(CPU* cpu) {
	uint16_t absolute_address = (uint16_t)ReadByte(cpu, cpu->registers.program_counter);	
	cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t ZeroPageX(CPU* cpu) {
	uint16_t absolute_address = ((uint16_t)ReadByte(cpu, cpu->registers.program_counter) + (uint16_t)cpu->registers.x_register) & 0x00FF;
	cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t ZeroPageY(CPU* cpu) {
	uint16_t absolute_address = ((uint16_t)ReadByte(cpu, cpu->registers.program_counter) + (uint16_t)cpu->registers.y_register) & 0x00FF;
	cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t Relative(CPU* cpu) {
	uint16_t relative_address = (uint16_t)ReadByte(cpu, cpu->registers.program_counter);
	cpu->registers.program_counter++;

	if (relative_address & 0x0080) {
		relative_address |= 0xFF00;
    }

    uint16_t absolute_address = cpu->registers.program_counter + relative_address;
	
    return absolute_address;
}
static uint16_t Absolute(CPU* cpu) {
    uint16_t absolute_address = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;

	return absolute_address;
}
static uint16_t AbsoluteX(CPU* cpu) {
	uint16_t temp_address = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;
    
    uint16_t absolute_address = temp_address + (uint16_t)cpu->registers.x_register;

    if ((temp_address ^ absolute_address) >> 8) {
        cpu->remaining_cycles++;
    }

    return absolute_address;
}
static uint16_t AbsoluteY(CPU* cpu) {
	uint16_t temp_address = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;
	
    uint16_t absolute_address = temp_address + (uint16_t)cpu->registers.y_register;

	if ((temp_address ^ absolute_address) >> 8) {
        cpu->remaining_cycles++;
    }
 
    return absolute_address;
}
static uint16_t Indirect(CPU* cpu) {
    uint16_t ptr = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;

    uint16_t absolute_address;
	if ((ptr & 0x00FF) == 0x00FF) {
		absolute_address = (uint16_t)(ReadByte(cpu, (ptr & 0xFF00)) << 8) | (uint16_t)ReadByte(cpu, ptr);   // known hardwer bug in 6502
	} else {
		absolute_address = ReadLittleEndianWord(cpu, ptr);
	}
	
	return absolute_address;
}
static uint16_t IndirectX(CPU* cpu) {
	uint8_t ptr = ReadByte(cpu, cpu->registers.program_counter);
	cpu->registers.program_counter++;
    
    uint16_t absolute_address = (uint16_t)(ReadByte(cpu, (((uint16_t)ptr + (uint16_t)cpu->registers.x_register + 1) & 0x00FF)) << 8) 
                              | (uint16_t)ReadByte(cpu, (((uint16_t)ptr + (uint16_t)cpu->registers.x_register) & 0x00FF));
	
    return absolute_address;
}
static uint16_t IndirectY(CPU* cpu) {
    uint8_t ptr = ReadByte(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter++;

    uint16_t temp_address = (uint16_t)(ReadByte(cpu, (((uint16_t)ptr + 1) & 0x00FF)) << 8) | (uint16_t)ReadByte(cpu, ptr);

    uint16_t absolute_address = temp_address + cpu->registers.y_register;

	
	if ((temp_address ^ absolute_address) >> 8) {
        cpu->remaining_cycles++;
    }

    return absolute_address;
}



static void ILL(CPU* cpu, const uint16_t absolute_address) {
    printf("illigal opcode\n");
    exit(1);
    return;
}

static void ASL_ACC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = cpu->registers.a_register;
    SetCarryFlagValue(cpu, (temp & 0x80));
    temp <<= 1;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;    
    return;    
}
static void ROL_ACC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = cpu->registers.a_register;
    uint8_t temp_carry = GetCarryFlag(cpu);
    SetCarryFlagValue(cpu, (temp & 0x80));
    temp <<= 1;
    temp |= temp_carry;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;
    return;
}
static void LSR_ACC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = cpu->registers.a_register;
    SetCarryFlagValue(cpu, (temp & 0x01));
    temp >>= 1;
    SetNegativeFlagValue(cpu, 0);
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;
    return;
}
static void ROR_ACC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = cpu->registers.a_register;
    uint8_t temp_carry = GetCarryFlagValue(cpu);
    SetCarryFlagValue(cpu, (temp & 0x01));
    temp >>= 1;
    temp |= (temp_carry << 7);
    SetNegativeFlagValue(cpu, temp_carry);
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;
    return;
}

static void BRK(CPU* cpu, const uint16_t absolute_address) {
    /**/
    StackPushLittleEndianWord(cpu, cpu->registers.program_counter);
    
    SetBrkCommandFlagValue(cpu, 1);
    StackPushByte(cpu, cpu->registers.status_flags);
    SetBrkCommandFlagValue(cpu, 0);
    
    SetIrgDisableFlagValue(cpu, 1);

    cpu->registers.program_counter = ReadLittleEndianWord(cpu, BREAK_INTERRUPT_OFFSET);
    return;
}
static void ORA(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp |= cpu->registers.a_register;

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));

    cpu->registers.a_register = temp;
    return;
}
static void ASL(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    SetCarryFlagValue(cpu, (temp & 0x80));
    temp <<= 1;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return; 
}
static void PHP(CPU* cpu, const uint16_t absolute_address) {
    /**/
    StackPushByte(cpu, cpu->registers.status_flags | UNUSED | BRK_COMMAND);
    return;
}
static void BPL(CPU* cpu, const uint16_t absolute_address) {
    if (GetNegativeFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLC(CPU* cpu, const uint16_t absolute_address) {
    SetCarryFlagValue(cpu, 0);
    return;
}
static void JSR(CPU* cpu, const uint16_t absolute_address) {
    StackPushLittleEndianWord(cpu, (cpu->registers.program_counter - 1));
    cpu->registers.program_counter = absolute_address;
    return;
}
static void AND(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp &= cpu->registers.a_register;

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));

    cpu->registers.a_register = temp;
    return;
}
static void BIT(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp &= cpu->registers.a_register;

    SetOverflowFlagValue(cpu, (temp & 0x40));
    
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    return;
}
static void ROL(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint8_t temp_carry = GetCarryFlag(cpu);
    SetCarryFlagValue(cpu, (temp & 0x80));
    temp <<= 1;
    temp |= temp_carry;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void PLP(CPU* cpu, const uint16_t absolute_address) {
    /**/
    cpu->registers.status_flags = StackPullByte(cpu);
    SetUnusedFlagValue(cpu, 1);
    SetBrkCommandFlagValue(cpu, 0); // on the actual 6502 cpu there is no place for this flag so it's ignored
    return;
}
static void BMI(CPU* cpu, const uint16_t absolute_address) {
    if (GetNegativeFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void SEC(CPU* cpu, const uint16_t absolute_address) {
    SetCarryFlagValue(cpu, 1);
    return;
}
static void RTI(CPU* cpu, const uint16_t absolute_address) {
    /**/
    cpu->registers.status_flags = StackPullByte(cpu);
    SetUnusedFlagValue(cpu, 1);
    SetBrkCommandFlagValue(cpu, 0); // on the actual 6502 cpu there is no place for this flag so it's ignored
    
    cpu->registers.program_counter = StackPullLittleEndianWord(cpu);
    return;
}
static void EOR(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp ^= cpu->registers.a_register;

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));

    cpu->registers.a_register = temp;
    return;
}
static void LSR(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    SetCarryFlagValue(cpu, (temp & 0x01));
    temp >>= 1;
    SetNegativeFlagValue(cpu, 0);
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void PHA(CPU* cpu, const uint16_t absolute_address) {
    StackPushByte(cpu, cpu->registers.a_register);
    return;
}
static void JMP(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.program_counter = absolute_address;
    return;
}
static void BVC(CPU* cpu, const uint16_t absolute_address) {
    if (GetOverflowFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLI(CPU* cpu, const uint16_t absolute_address) {
    SetIrgDisableFlagValue(cpu, 0);
    return;
}
static void RTS(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.program_counter = StackPullLittleEndianWord(cpu) + 1;
    return;
}
static void ADC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t res = (uint16_t)cpu->registers.a_register + (uint16_t)temp + (uint16_t)GetCarryFlagValue(cpu);

    SetZeroFlagValue(cpu, (res & 0x00FF));

    if (GetDecimalModeFlagValue(cpu)) {
        if (((cpu->registers.a_register & 0x0F) + (temp & 0x0F) + GetCarryFlagValue(cpu)) > 0x09) {
            res += 6;
        }

        SetNegativeFlagValue(cpu, (res & 0x0080));
        SetOverflowFlagValue(cpu, (!((cpu->registers.a_register ^ temp) & 0x80) && ((cpu->registers.a_register ^ (uint8_t)res) & 0x80)));
        
        if (res > 0x0099) {
            res += 96;
        }

        SetCarryFlagValue(cpu, (res > 0x0099));

        cpu->remaining_cycles++;
    } else {
        SetNegativeFlagValue(cpu, (res & 0x80));
        SetOverflowFlagValue(cpu, (!((cpu->registers.a_register ^ temp) & 0x80) && ((cpu->registers.a_register ^ (uint8_t)res) & 0x80)));
        SetCarryFlagValue(cpu, (res > 0x00FF));
    }

    cpu->registers.a_register = (uint8_t)(res & 0x00FF);
    return;
}
static void ROR(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint8_t temp_carry = GetCarryFlagValue(cpu);
    SetCarryFlagValue(cpu, (temp & 0x01));
    temp >>= 1;
    temp |= (temp_carry << 7);
    SetNegativeFlagValue(cpu, temp_carry);
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void PLA(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = StackPullByte(cpu);
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;
    return;
}
static void BVS(CPU* cpu, const uint16_t absolute_address) {
    if (GetOverflowFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void SEI(CPU* cpu, const uint16_t absolute_address) {
    SetIrgDisableFlagValue(cpu, 1);
    return;
}
static void STA(CPU* cpu, const uint16_t absolute_address) {
    WriteByte(cpu, absolute_address, cpu->registers.a_register);
    return;
}
static void STY(CPU* cpu, const uint16_t absolute_address) {
    WriteByte(cpu, absolute_address, cpu->registers.y_register);
    return;
}
static void STX(CPU* cpu, const uint16_t absolute_address) {
    WriteByte(cpu, absolute_address, cpu->registers.x_register);
    return;
}
static void DEY(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register--;
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void TXA(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.a_register = cpu->registers.x_register;
    SetNegativeFlagValue(cpu, (cpu->registers.a_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.a_register));
    return;
}
static void BCC(CPU* cpu, const uint16_t absolute_address) {
    if (GetCarryFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void TYA(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.a_register = cpu->registers.y_register;
    SetNegativeFlagValue(cpu, (cpu->registers.a_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.a_register));
    return;
}
static void TXS(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.stack_pointer = cpu->registers.x_register;
    return;
}
static void LDY(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register = ReadByte(cpu, absolute_address);
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void LDA(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.a_register = ReadByte(cpu, absolute_address);
    SetNegativeFlagValue(cpu, (cpu->registers.a_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.a_register));
    return;
}
static void LDX(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register = ReadByte(cpu, absolute_address);
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void TAY(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register = cpu->registers.a_register;
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void TAX(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register = cpu->registers.a_register;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void BCS(CPU* cpu, const uint16_t absolute_address) {
    if (GetCarryFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLV(CPU* cpu, const uint16_t absolute_address) {
    SetOverflowFlagValue(cpu, 0);
}
static void TSX(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register = cpu->registers.stack_pointer;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void CPY(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t cmp = (uint16_t)cpu->registers.y_register - (uint16_t)temp;
    SetCarryFlagValue(cpu, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(cpu, (cmp & 0x0080));
    SetZeroFlagValue(cpu, (!(cmp & 0x00FF)));
    return;
}
static void CMP(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t cmp = (uint16_t)cpu->registers.a_register - (uint16_t)temp;
    SetCarryFlagValue(cpu, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(cpu, (cmp & 0x0080));
    SetZeroFlagValue(cpu, (!(cmp & 0x00FF)));
    return;
}
static void DEC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    temp--;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void INY(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register++;
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void DEX(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register--;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void BNE(CPU* cpu, const uint16_t absolute_address) {
    if (GetZeroFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLD(CPU* cpu, const uint16_t absolute_address) {
    SetDecimalModeFlagValue(cpu, 0);
    return;
}
static void CPX(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t cmp = (uint16_t)cpu->registers.x_register - (uint16_t)temp;
    SetCarryFlagValue(cpu, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(cpu, (cmp & 0x0080));
    SetZeroFlagValue(cpu, (!(cmp & 0x00FF)));
    return;
}
static void SBC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t res = (uint16_t)cpu->registers.a_register - (uint16_t)temp + (uint16_t)GetCarryFlagValue(cpu) - 1;

    SetNegativeFlagValue(cpu, (res & 0x0080));
    SetZeroFlagValue(cpu, (!(res & 0x00FF)));
    SetOverflowFlagValue(cpu, (((cpu->registers.a_register ^ temp) & 0x80) && ((cpu->registers.a_register ^ (uint8_t)res) & 0x80)));

    if (GetDecimalModeFlagValue(cpu)) {
        if (((cpu->registers.a_register & 0x0F) + GetCarryFlagValue(cpu) - 1) < (temp & 0x0F)) {
            res -= 6;
        }

        
        if (res > 0x0099) {
            res -= 0x60;
        }

        cpu->remaining_cycles++;
    }

    SetCarryFlagValue(cpu, (!(res & 0xFF00)));
    cpu->registers.a_register = (uint8_t)(res & 0x00FF);
    return;
}
static void INC(CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    temp++;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void INX(CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register++;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void NOP(CPU* cpu, const uint16_t absolute_address) {
    return;
}
static void BEQ(CPU* cpu, const uint16_t absolute_address) {
    if (GetZeroFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void SED(CPU* cpu, const uint16_t absolute_address) {
    SetDecimalModeFlagValue(cpu, 1);
    return;
}



typedef struct Instruction {
    char mnemonic[4];
    void (*operator)(CPU*, const uint16_t);
    uint16_t (*address_mode)(CPU*);
    uint8_t cycles;
} Instruction;

static const Instruction instructions[256] = {
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




void CPUInit(CPU* cpu) {
    // registers
    cpu->registers.a_register = 0;
	cpu->registers.x_register = 0; 
	cpu->registers.y_register = 0;   
    cpu->registers.status_flags = 0;
    cpu->registers.stack_pointer = RESET_OFFSET; 
	cpu->registers.program_counter = 0;

    cpu->remaining_cycles = 0;
    cpu->tick_counter = 0;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);
}


void CPUReset(CPU* cpu) {
    cpu->registers.program_counter = RESET_OFFSET;
    
    cpu->registers.stack_pointer -= 3;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);

    cpu->remaining_cycles = 8;
}



void CPUInterruptRequest(CPU* cpu) {
    if (GetIrqDisableFlagValue(cpu) == 0) {
        StackPushLittleEndianWord(cpu, cpu->registers.program_counter);

        SetBrkCommandFlagValue(cpu, 0);
        SetIrgDisableFlagValue(cpu, 1);
        SetUnusedFlagValue(cpu, 1);

        StackPushByte(cpu, cpu->registers.status_flags);

        cpu->registers.program_counter = ReadLittleEndianWord(cpu, BREAK_INTERRUPT_OFFSET);

        cpu->remaining_cycles = 7;
    }
}

void CPUNonMaskableInterrupt(CPU* cpu) {
    StackPushLittleEndianWord(cpu, cpu->registers.program_counter);

    SetBrkCommandFlagValue(cpu, 0);
    SetIrgDisableFlagValue(cpu, 1);
    SetUnusedFlagValue(cpu, 1);

    StackPushByte(cpu, cpu->registers.status_flags);

    cpu->registers.program_counter = ReadLittleEndianWord(cpu, NON_MASKABLE_INTERRUPT_OFFSET);

    cpu->remaining_cycles = 8;
}

//void tick(CPU* cpu, FILE* log_file) {
void CPUClock(CPU* cpu) {
    if (cpu->remaining_cycles == 0) {
        uint8_t op_code = ReadByte(cpu, cpu->registers.program_counter);
        cpu->registers.program_counter++;
        
        Instruction instruction = instructions[op_code];
        
        //fprintf(log_file, 
        //        "clock: %lu  -  mnemonic: %s  -  opcode: 0x%0x  -  A: 0x%02x  -  X: 0x%02x  -  Y: 0x%02x  -  Status: 0x%02x  -  Stack pointer: 0x%02x  -  Program counter: 0x%04x\n", 
        //        cpu->tick_counter, instruction.mnemonic, op_code, cpu->registers.a_register, cpu->registers.x_register, cpu->registers.y_register, cpu->registers.status_flags, cpu->registers.stack_pointer, cpu->registers.program_counter);
        
        cpu->remaining_cycles = instruction.cycles;
        uint16_t absolute_address = instruction.address_mode(cpu);
        printf("mnemonic: %s\n", instruction.mnemonic);
        printf("abs addr: %04x\n", absolute_address);
        instruction.operator(cpu, absolute_address);

    }

    cpu->tick_counter++;
    cpu->remaining_cycles--;
}




//int main() {
//
//    CPU cpu;
//    Init(&cpu);
//
//    memory[0x8000] = 0xa2;
//    memory[0x8001] = 0x0a;
//    memory[0x8002] = 0x8e;
//    memory[0x8003] = 0x00;
//    memory[0x8004] = 0x00;
//    memory[0x8005] = 0xa2;
//    memory[0x8006] = 0x03;
//    memory[0x8007] = 0x8e;
//
//    memory[0x8008] = 0x01;
//    memory[0x8009] = 0x00;
//    memory[0x800a] = 0xac;
//    memory[0x800b] = 0x00;
//    memory[0x800c] = 0x00;
//    memory[0x800d] = 0xa9;
//    memory[0x800e] = 0x00;
//    memory[0x800f] = 0x18;
//
//    memory[0x8010] = 0x6d;
//    memory[0x8011] = 0x01;
//    memory[0x8012] = 0x00;
//    memory[0x8013] = 0x88;
//    memory[0x8014] = 0xd0;
//    memory[0x8015] = 0xfa;
//    memory[0x8016] = 0x8d;
//    memory[0x8017] = 0x02;
//
//    memory[0x8018] = 0x00;
//    memory[0x8019] = 0xea;
//    memory[0x801a] = 0xea;
//    memory[0x801b] = 0xea;
//
//
//    WriteLittleEndianWord(RESET_OFFSET, 0x8000);
//
//    Reset(&cpu);
//
//    const char* log_filename = "cpu_log.txt";
//    FILE* log_file = fopen(log_filename, "w");
//    if (log_file == NULL) {
//        perror("fopen");
//        exit(1);
//    }
//
//    for (int i = 0; i < 1000; i++) {
//        printf("tick: %d\n", i);
//        tick(&cpu, log_file);
//    }
//
//
//    fclose(log_file);
//    return 0;
//}