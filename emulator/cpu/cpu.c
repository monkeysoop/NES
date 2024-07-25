#include "cpu.h"
#include <string.h>

//uint8_t memory[65536];




static inline uint8_t GetCarryFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & CARRY;
}
static inline uint8_t GetZeroFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & ZERO;
}
static inline uint8_t GetIrqDisableFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & IRQ_DISABLE;
}
static inline uint8_t GetDecimalModeFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & DECIMAL_MODE;
}
static inline uint8_t GetBrkCommandFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & BRK_COMMAND;
}
static inline uint8_t GetUnusedFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & UNUSED;
}
static inline uint8_t GetOverflowFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & OVERFLOW;
}
static inline uint8_t GetNegativeFlag(struct CPU* cpu) {
    return cpu->registers.status_flags & NEGATIVE;
}



static inline uint8_t GetCarryFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & CARRY) ? 1 : 0);
}
static inline uint8_t GetZeroFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & ZERO) ? 1 : 0);
}
static inline uint8_t GetIrqDisableFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & IRQ_DISABLE) ? 1 : 0);
}
static inline uint8_t GetDecimalModeFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & DECIMAL_MODE) ? 1 : 0);
}
static inline uint8_t GetBrkCommandFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & BRK_COMMAND) ? 1 : 0);
}
static inline uint8_t GetUnusedFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & UNUSED) ? 1 : 0);
}
static inline uint8_t GetOverflowFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & OVERFLOW) ? 1 : 0);
}
static inline uint8_t GetNegativeFlagValue(struct CPU* cpu) {
    return ((cpu->registers.status_flags & NEGATIVE) ? 1 : 0);
}



static inline void SetCarryFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= CARRY) : (cpu->registers.status_flags &= (~CARRY));    
    return;
}
static inline void SetZeroFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= ZERO) : (cpu->registers.status_flags &= (~ZERO));    
    return;
}
static inline void SetIrgDisableFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= IRQ_DISABLE) : (cpu->registers.status_flags &= (~IRQ_DISABLE));    
    return;
}
static inline void SetDecimalModeFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= DECIMAL_MODE) : (cpu->registers.status_flags &= (~DECIMAL_MODE));    
    return;
}
static inline void SetBrkCommandFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= BRK_COMMAND) : (cpu->registers.status_flags &= (~BRK_COMMAND));    
    return;
}
static inline void SetUnusedFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= UNUSED) : (cpu->registers.status_flags &= (~UNUSED));    
    return;
}
static inline void SetOverflowFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= OVERFLOW) : (cpu->registers.status_flags &= (~OVERFLOW));    
    return;
}
static inline void SetNegativeFlagValue(struct CPU* cpu, const uint8_t value) {
    (value) ? (cpu->registers.status_flags |= NEGATIVE) : (cpu->registers.status_flags &= (~NEGATIVE));    
    return;
}



static inline void SetAllFlags(struct CPU* cpu) {
    cpu->registers.status_flags = 0xFF;
}
static inline void ClearAllFlags(struct CPU* cpu) {
    cpu->registers.status_flags = 0x00;
}



static inline uint8_t ReadByte(struct CPU* cpu, const uint16_t address) {
    //return memory[address];
    return CPUBusRead(cpu->cpu_bus, address);
}
static inline void WriteByte(struct CPU* cpu, const uint16_t address, const uint8_t data) {
    //memory[address] = data;
    CPUBusWrite(cpu->cpu_bus, address, data);
}

static inline uint16_t ReadBigEndianWord(struct CPU* cpu, const uint16_t address_start) {
    return (ReadByte(cpu, address_start) << 8) | ReadByte(cpu, (address_start + 1));
} 
static inline uint16_t ReadLittleEndianWord(struct CPU* cpu, const uint16_t address_start) {
    return ReadByte(cpu, address_start) | (ReadByte(cpu, (address_start + 1)) << 8);
}
static inline void WriteBigEndianWord(struct CPU* cpu, const uint16_t address, const uint16_t data) {
    WriteByte(cpu, address, (data >> 8));
    WriteByte(cpu, (address + 1), (data & 0xFF));
}
static inline void WriteLittleEndianWord(struct CPU* cpu, const uint16_t address, const uint16_t data) {
    WriteByte(cpu, address, (data & 0xFF));
    WriteByte(cpu, (address + 1), (data >> 8));
}



static inline uint8_t StackPullByte(struct CPU* cpu) {
    cpu->registers.stack_pointer++;
    return ReadByte(cpu, (STACK_OFFSET + cpu->registers.stack_pointer));
}
static inline void StackPushByte(struct CPU* cpu, const uint8_t data) {
    WriteByte(cpu, (STACK_OFFSET + cpu->registers.stack_pointer), data);
    cpu->registers.stack_pointer--;
}

static inline uint16_t StackPullBigEndianWord(struct CPU* cpu) {
    return (StackPullByte(cpu) << 8) | StackPullByte(cpu);
} 
static inline uint16_t StackPullLittleEndianWord(struct CPU* cpu) {
    return StackPullByte(cpu) | (StackPullByte(cpu) << 8);
}
static inline void StackPushBigEndianWord(struct CPU* cpu, uint16_t data) {
    // 6502 stack goes from higher address to lower (right to left)
    StackPushByte(cpu, (data & 0xFF));
    StackPushByte(cpu, (data >> 8));
}
static inline void StackPushLittleEndianWord(struct CPU* cpu, uint16_t data) {
    // 6502 stack goes from higher address to lower (right to left)
    StackPushByte(cpu, (data >> 8));
    StackPushByte(cpu, (data & 0xFF));
}






static uint16_t IlligalMode(struct CPU* cpu) {
    printf("illigal addressing mode\n");
    exit(1);
    return 0;
}
static uint16_t Accumulator(struct CPU* cpu) {
    return 0;   // not used because Accumulator addressing takes no value from memory 
}
static uint16_t Implied(struct CPU* cpu) {
	return 0;   // not used because Implied addressing takes no value from memory 
}
static uint16_t Immediate(struct CPU* cpu) {
	uint16_t absolute_address = cpu->registers.program_counter;
    cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t ZeroPage(struct CPU* cpu) {
	uint16_t absolute_address = (uint16_t)ReadByte(cpu, cpu->registers.program_counter);	
	cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t ZeroPageX(struct CPU* cpu) {
	uint16_t absolute_address = ((uint16_t)ReadByte(cpu, cpu->registers.program_counter) + (uint16_t)cpu->registers.x_register) & 0x00FF;
	cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t ZeroPageY(struct CPU* cpu) {
	uint16_t absolute_address = ((uint16_t)ReadByte(cpu, cpu->registers.program_counter) + (uint16_t)cpu->registers.y_register) & 0x00FF;
	cpu->registers.program_counter++;

	return absolute_address;
}
static uint16_t Relative(struct CPU* cpu) {
	uint16_t relative_address = (uint16_t)ReadByte(cpu, cpu->registers.program_counter);
	cpu->registers.program_counter++;

	if (relative_address & 0x0080) {
		relative_address |= 0xFF00;
    }

    uint16_t absolute_address = cpu->registers.program_counter + relative_address;
	
    return absolute_address;
}
static uint16_t Absolute(struct CPU* cpu) {
    uint16_t absolute_address = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;

	return absolute_address;
}
static uint16_t AbsoluteX(struct CPU* cpu) {
	uint16_t temp_address = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;
    
    uint16_t absolute_address = temp_address + (uint16_t)cpu->registers.x_register;

    if ((temp_address ^ absolute_address) >> 8) {
        cpu->remaining_cycles++;
    }

    return absolute_address;
}
static uint16_t AbsoluteY(struct CPU* cpu) {
	uint16_t temp_address = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;
	
    uint16_t absolute_address = temp_address + (uint16_t)cpu->registers.y_register;

	if ((temp_address ^ absolute_address) >> 8) {
        cpu->remaining_cycles++;
    }
 
    return absolute_address;
}
static uint16_t Indirect(struct CPU* cpu) {
    uint16_t ptr = ReadLittleEndianWord(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter += 2;

    uint16_t absolute_address;
	absolute_address = (uint16_t)(ReadByte(cpu, ((ptr & 0xFF00) | ((ptr + 1) & 0x00FF))) << 8) | (uint16_t)ReadByte(cpu, ptr);   // known hardwer bug in 6502
	
	return absolute_address;
}
static uint16_t IndirectX(struct CPU* cpu) {
	uint8_t ptr = ReadByte(cpu, cpu->registers.program_counter);
	cpu->registers.program_counter++;
    
    uint16_t absolute_address = (uint16_t)(ReadByte(cpu, (((uint16_t)ptr + (uint16_t)cpu->registers.x_register + 1) & 0x00FF)) << 8) 
                              | (uint16_t)ReadByte(cpu, (((uint16_t)ptr + (uint16_t)cpu->registers.x_register) & 0x00FF));
	
    return absolute_address;
}
static uint16_t IndirectY(struct CPU* cpu) {
    uint8_t ptr = ReadByte(cpu, cpu->registers.program_counter);
    cpu->registers.program_counter++;

    uint16_t temp_address = (uint16_t)(ReadByte(cpu, (((uint16_t)ptr + 1) & 0x00FF)) << 8) | (uint16_t)ReadByte(cpu, ptr);

    uint16_t absolute_address = temp_address + cpu->registers.y_register;

	
	if ((temp_address ^ absolute_address) >> 8) {
        cpu->remaining_cycles++;
    }

    return absolute_address;
}



static void ILL(struct CPU* cpu, const uint16_t absolute_address) {
    printf("illigal opcode\n");
    exit(1);
    return;
}

static void ASL_ACC(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = cpu->registers.a_register;
    SetCarryFlagValue(cpu, (temp & 0x80));
    temp <<= 1;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;    
    return;    
}
static void ROL_ACC(struct CPU* cpu, const uint16_t absolute_address) {
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
static void LSR_ACC(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = cpu->registers.a_register;
    SetCarryFlagValue(cpu, (temp & 0x01));
    temp >>= 1;
    SetNegativeFlagValue(cpu, 0);
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;
    return;
}
static void ROR_ACC(struct CPU* cpu, const uint16_t absolute_address) {
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

static void BRK(struct CPU* cpu, const uint16_t absolute_address) {
    /**/
    StackPushLittleEndianWord(cpu, cpu->registers.program_counter);
    
    SetBrkCommandFlagValue(cpu, 1);
    StackPushByte(cpu, cpu->registers.status_flags);
    SetBrkCommandFlagValue(cpu, 0);
    
    SetIrgDisableFlagValue(cpu, 1);

    cpu->registers.program_counter = ReadLittleEndianWord(cpu, BREAK_INTERRUPT_OFFSET);
    return;
}
static void ORA(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp |= cpu->registers.a_register;

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));

    cpu->registers.a_register = temp;
    return;
}
static void ASL(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    SetCarryFlagValue(cpu, (temp & 0x80));
    temp <<= 1;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return; 
}
static void PHP(struct CPU* cpu, const uint16_t absolute_address) {
    /**/
    StackPushByte(cpu, cpu->registers.status_flags | UNUSED | BRK_COMMAND);
    return;
}
static void BPL(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetNegativeFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLC(struct CPU* cpu, const uint16_t absolute_address) {
    SetCarryFlagValue(cpu, 0);
    return;
}
static void JSR(struct CPU* cpu, const uint16_t absolute_address) {
    StackPushLittleEndianWord(cpu, (cpu->registers.program_counter - 1));
    cpu->registers.program_counter = absolute_address;
    return;
}
static void AND(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp &= cpu->registers.a_register;

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));

    cpu->registers.a_register = temp;
    return;
}
static void BIT(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp &= cpu->registers.a_register;

    SetOverflowFlagValue(cpu, (temp & 0x40));
    
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    return;
}
static void ROL(struct CPU* cpu, const uint16_t absolute_address) {
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
static void PLP(struct CPU* cpu, const uint16_t absolute_address) {
    /**/
    cpu->registers.status_flags = StackPullByte(cpu);
    SetUnusedFlagValue(cpu, 1);
    SetBrkCommandFlagValue(cpu, 0); // on the actual 6502 cpu there is no place for this flag so it's ignored
    return;
}
static void BMI(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetNegativeFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void SEC(struct CPU* cpu, const uint16_t absolute_address) {
    SetCarryFlagValue(cpu, 1);
    return;
}
static void RTI(struct CPU* cpu, const uint16_t absolute_address) {
    /**/
    cpu->registers.status_flags = StackPullByte(cpu);
    SetUnusedFlagValue(cpu, 1);
    SetBrkCommandFlagValue(cpu, 0); // on the actual 6502 cpu there is no place for this flag so it's ignored
    
    cpu->registers.program_counter = StackPullLittleEndianWord(cpu);
    return;
}
static void EOR(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);

    temp ^= cpu->registers.a_register;

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));

    cpu->registers.a_register = temp;
    return;
}
static void LSR(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    SetCarryFlagValue(cpu, (temp & 0x01));
    temp >>= 1;
    SetNegativeFlagValue(cpu, 0);
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void PHA(struct CPU* cpu, const uint16_t absolute_address) {
    StackPushByte(cpu, cpu->registers.a_register);
    return;
}
static void JMP(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.program_counter = absolute_address;
    return;
}
static void BVC(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetOverflowFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLI(struct CPU* cpu, const uint16_t absolute_address) {
    SetIrgDisableFlagValue(cpu, 0);
    return;
}
static void RTS(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.program_counter = StackPullLittleEndianWord(cpu) + 1;
    return;
}
static void ADC(struct CPU* cpu, const uint16_t absolute_address) {
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
static void ROR(struct CPU* cpu, const uint16_t absolute_address) {
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
static void PLA(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = StackPullByte(cpu);
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    cpu->registers.a_register = temp;
    return;
}
static void BVS(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetOverflowFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void SEI(struct CPU* cpu, const uint16_t absolute_address) {
    SetIrgDisableFlagValue(cpu, 1);
    return;
}
static void STA(struct CPU* cpu, const uint16_t absolute_address) {
    WriteByte(cpu, absolute_address, cpu->registers.a_register);
    return;
}
static void STY(struct CPU* cpu, const uint16_t absolute_address) {
    WriteByte(cpu, absolute_address, cpu->registers.y_register);
    return;
}
static void STX(struct CPU* cpu, const uint16_t absolute_address) {
    WriteByte(cpu, absolute_address, cpu->registers.x_register);
    return;
}
static void DEY(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register--;
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void TXA(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.a_register = cpu->registers.x_register;
    SetNegativeFlagValue(cpu, (cpu->registers.a_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.a_register));
    return;
}
static void BCC(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetCarryFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void TYA(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.a_register = cpu->registers.y_register;
    SetNegativeFlagValue(cpu, (cpu->registers.a_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.a_register));
    return;
}
static void TXS(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.stack_pointer = cpu->registers.x_register;
    return;
}
static void LDY(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register = ReadByte(cpu, absolute_address);
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void LDA(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.a_register = ReadByte(cpu, absolute_address);
    SetNegativeFlagValue(cpu, (cpu->registers.a_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.a_register));
    return;
}
static void LDX(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register = ReadByte(cpu, absolute_address);
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void TAY(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register = cpu->registers.a_register;
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void TAX(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register = cpu->registers.a_register;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void BCS(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetCarryFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLV(struct CPU* cpu, const uint16_t absolute_address) {
    SetOverflowFlagValue(cpu, 0);
}
static void TSX(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register = cpu->registers.stack_pointer;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void CPY(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t cmp = (uint16_t)cpu->registers.y_register - (uint16_t)temp;
    SetCarryFlagValue(cpu, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(cpu, (cmp & 0x0080));
    SetZeroFlagValue(cpu, (!(cmp & 0x00FF)));
    return;
}
static void CMP(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t cmp = (uint16_t)cpu->registers.a_register - (uint16_t)temp;
    SetCarryFlagValue(cpu, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(cpu, (cmp & 0x0080));
    SetZeroFlagValue(cpu, (!(cmp & 0x00FF)));
    return;
}
static void DEC(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    temp--;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void INY(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.y_register++;
    SetNegativeFlagValue(cpu, (cpu->registers.y_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.y_register));
    return;
}
static void DEX(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register--;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void BNE(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetZeroFlagValue(cpu) == 0) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void CLD(struct CPU* cpu, const uint16_t absolute_address) {
    SetDecimalModeFlagValue(cpu, 0);
    return;
}
static void CPX(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    uint16_t cmp = (uint16_t)cpu->registers.x_register - (uint16_t)temp;
    SetCarryFlagValue(cpu, (!(cmp & 0xFF00)));
    SetNegativeFlagValue(cpu, (cmp & 0x0080));
    SetZeroFlagValue(cpu, (!(cmp & 0x00FF)));
    return;
}
static void SBC(struct CPU* cpu, const uint16_t absolute_address) {
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
static void INC(struct CPU* cpu, const uint16_t absolute_address) {
    uint8_t temp = ReadByte(cpu, absolute_address);
    temp++;
    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetZeroFlagValue(cpu, (!temp));
    WriteByte(cpu, absolute_address, temp);
    return;
}
static void INX(struct CPU* cpu, const uint16_t absolute_address) {
    cpu->registers.x_register++;
    SetNegativeFlagValue(cpu, (cpu->registers.x_register & 0x80));
    SetZeroFlagValue(cpu, (!cpu->registers.x_register));
    return;
}
static void NOP(struct CPU* cpu, const uint16_t absolute_address) {
    return;
}
static void BEQ(struct CPU* cpu, const uint16_t absolute_address) {
    if (GetZeroFlagValue(cpu) == 1) {
        cpu->remaining_cycles++;

        if ((cpu->registers.program_counter & 0xFF00) != (absolute_address & 0xFF00)) {
            cpu->remaining_cycles++;
        }

        cpu->registers.program_counter = absolute_address;
    }
    return;
}
static void SED(struct CPU* cpu, const uint16_t absolute_address) {
    SetDecimalModeFlagValue(cpu, 1);
    return;
}



typedef struct Instruction {
    char mnemonic[4];
    void (*operator)(struct CPU*, const uint16_t);
    uint16_t (*address_mode)(struct CPU*);
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




void CPUInit(struct CPU* cpu) {
    // registers
    cpu->registers.a_register = 0;
	cpu->registers.x_register = 0; 
	cpu->registers.y_register = 0;   
    cpu->registers.status_flags = 0;
    cpu->registers.stack_pointer = STACK_POINTER_OFFSET; 

	//cpu->registers.program_counter = ReadLittleEndianWord(cpu, RESET_OFFSET);
	cpu->registers.program_counter = ReadByte(cpu, RESET_OFFSET);

    cpu->remaining_cycles = 0;
    cpu->tick_counter = 0;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);

    //memset(memory, 0, 65536 * sizeof(uint8_t));
    //for (int i = 0; i < 65536; i++) {
    //    memory[i] = rand() & 0xFF;
    //} 
    //
    //memory[0x8000] = 0xa2;
    //memory[0x8001] = 0x0a;
    //memory[0x8002] = 0x8e;
    //memory[0x8003] = 0x00;
    //memory[0x8004] = 0x00;
    //memory[0x8005] = 0xa2;
    //memory[0x8006] = 0x03;
    //memory[0x8007] = 0x8e;
    //
    //memory[0x8008] = 0x01;
    //memory[0x8009] = 0x00;
    //memory[0x800a] = 0xac;
    //memory[0x800b] = 0x00;
    //memory[0x800c] = 0x00;
    //memory[0x800d] = 0xa9;
    //memory[0x800e] = 0x00;
    //memory[0x800f] = 0x18;
    //
    //memory[0x8010] = 0x6d;
    //memory[0x8011] = 0x01;
    //memory[0x8012] = 0x00;
    //memory[0x8013] = 0x88;
    //memory[0x8014] = 0xd0;
    //memory[0x8015] = 0xfa;
    //memory[0x8016] = 0x8d;
    //memory[0x8017] = 0x02;
    //
    //memory[0x8018] = 0x00;
    //memory[0x8019] = 0xea;
    //memory[0x801a] = 0xea;
    //memory[0x801b] = 0xea;


}


void CPUReset(struct CPU* cpu) {
    cpu->registers.program_counter = ReadLittleEndianWord(cpu, RESET_OFFSET);
    
    cpu->registers.stack_pointer -= 3;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);

    cpu->remaining_cycles = 8;
}



void CPUInterruptRequest(struct CPU* cpu) {
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

void CPUNonMaskableInterrupt(struct CPU* cpu) {
    StackPushLittleEndianWord(cpu, cpu->registers.program_counter);

    SetBrkCommandFlagValue(cpu, 0);
    SetIrgDisableFlagValue(cpu, 1);
    SetUnusedFlagValue(cpu, 1);

    StackPushByte(cpu, cpu->registers.status_flags);

    cpu->registers.program_counter = ReadLittleEndianWord(cpu, NON_MASKABLE_INTERRUPT_OFFSET);

    cpu->remaining_cycles = 8;
}

void CPUClock(struct CPU* cpu) {
    if (cpu->remaining_cycles == 0) {
        uint8_t op_code = ReadByte(cpu, cpu->registers.program_counter);
        cpu->registers.program_counter++;
        

        Instruction instruction = instructions[op_code];
        printf("mnemonic: %s\n", instruction.mnemonic);
        
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


uint8_t CPUDisassemble(struct CPU* cpu, uint16_t start_address, uint16_t count, char** debug_char_buffer, uint8_t w, uint8_t h) {
    // note that for addressing modes that require a register (x/y) it can't fully be disassembled 
    // because registers are a runtime thing and this function only disassembles the code doesn't execute it 

    uint16_t dummy_address = start_address;
    uint16_t prev_dummy_address = start_address;
    //uint16_t dummy_program_counter = cpu->registers.program_counter;
    uint8_t active_row = h;
    char* row_buffer = malloc(w * sizeof(char));

    for (int y = 0; y < count && y < h; y++) {
        memset(row_buffer, ' ', w * sizeof(char));

        

        int x = 0;
        
        if (dummy_address < 0x2000 || dummy_address >= 0x4020) { // with certain mappers this still could cause side effects but minimizes the chance
            uint8_t op_code = ReadByte(cpu, dummy_address);
            Instruction instruction = instructions[op_code];

            if ((x + 12) < w) {
                snprintf((row_buffer + x), 12, "0x%04X: %s", dummy_address, instruction.mnemonic);
            }
            x += 11;

            dummy_address++;

            if (instruction.address_mode == &IlligalMode) {
                if ((x + 5) < w) {
                    snprintf((row_buffer + x), 5, " ???");
                }    
                x += 4;
            }
            else if (instruction.address_mode == &Accumulator) {
                if ((x + 5) < w) {
                    snprintf((row_buffer + x), 5, " ACC");
                }    
                x += 4;
            }
            else if (instruction.address_mode == &Implied) {
                if ((x + 5) < w) {
                    snprintf((row_buffer + x), 5, " IMP");
                } 
                x += 4;
            }
            else if (instruction.address_mode == &Immediate) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < w) {
                        snprintf((row_buffer + x), 11, " IMM  0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &ZeroPage) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < w) {
                        snprintf((row_buffer + x), 11, " ZP   0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &ZeroPageX) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < w) {
                        snprintf((row_buffer + x), 11, " ZPX  0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &ZeroPageY) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < w) {
                        snprintf((row_buffer + x), 11, " ZPY  0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &Relative) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < w) {
                        snprintf((row_buffer + x), 11, " REL %c0x%02X", (((int8_t)temp_address < 0) ? '-' : '+'), (((int8_t)temp_address < 0) ? (-1 * (int8_t)temp_address) : temp_address));
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &Absolute) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < w) {
                        snprintf((row_buffer + x), 13, " ABS  0x%04X", temp_address);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &AbsoluteX) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < w) {
                        snprintf((row_buffer + x), 13, " ABX  0x%04X", temp_address);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &AbsoluteY) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < w) {
                        snprintf((row_buffer + x), 13, " ABY  0x%04X", temp_address);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &Indirect) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;

                    if ((x + 13) < w) {
                        snprintf((row_buffer + x), 13, " IND  0x%04X", temp_address_ptr);
                    } 
                    x += 12;

                    uint16_t temp_address_ptr_to_low = ((temp_address_ptr & 0xFF00) | ((temp_address_ptr + 1) & 0x00FF));
                    if ((temp_address_ptr < 0x2000 || temp_address_ptr >= 0x4020) && (temp_address_ptr_to_low < 0x2000 || temp_address_ptr_to_low >= 0x4020)) {
                        uint16_t temp_address = (uint16_t)(ReadByte(cpu, temp_address_ptr_to_low) << 8) | (uint16_t)ReadByte(cpu, temp_address_ptr);
                      
                        if ((x + 9) < w) {
                            snprintf((row_buffer + x), 9, "  0x%04X", temp_address);
                        } 
                        x += 8;
                    }
                }
            }
            else if (instruction.address_mode == &IndirectX) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;

                    if ((x + 13) < w) {
                        snprintf((row_buffer + x), 13, " INX  0x%04X", temp_address_ptr);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &IndirectY) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;

                    if ((x + 13) < w) {
                        snprintf((row_buffer + x), 13, " INY  0x%04X", temp_address_ptr);
                    } 
                    x += 12;
                }
            } else {
                printf("unknown addressing mode\n");
                free(row_buffer);
                exit(1);
            }
        }

        if (x >= w) {
            // this means something didn't fit
            printf("character buffer too thin\n");
            free(row_buffer);
            exit(1);
        }
        
        row_buffer[x] = ' ';    // removes null terminator
        memcpy(debug_char_buffer[y], row_buffer, w * sizeof(char));

        if (cpu->registers.program_counter >= prev_dummy_address && cpu->registers.program_counter < dummy_address) {
            active_row = y;
        }

        prev_dummy_address = dummy_address;
    }
    free(row_buffer);

    return active_row;
}