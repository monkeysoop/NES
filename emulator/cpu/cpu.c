#include "cpu.h"
#include <string.h>





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
    return CPUBusRead(cpu->cpu_bus, address);
}
static inline void WriteByte(struct CPU* cpu, const uint16_t address, const uint8_t data) {
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




void CPUInit(struct CPU* cpu, struct CPUBus* cpu_bus) {
    cpu->cpu_bus = cpu_bus;

    // registers
    cpu->registers.a_register = 0;
	cpu->registers.x_register = 0; 
	cpu->registers.y_register = 0;   
    cpu->registers.status_flags = 0;
    cpu->registers.stack_pointer = STACK_POINTER_OFFSET; 

	cpu->registers.program_counter = ReadLittleEndianWord(cpu, RESET_OFFSET);

    cpu->remaining_cycles = 0;
    cpu->tick_counter = 0;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);
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
        
        printf("mnemonic: %s  -  opcode: 0x%02x  -  A: 0x%02x  -  X: 0x%02x  -  Y: 0x%02x  -  Status: 0x%02x  -  Stack pointer: 0x%02x  -  Program counter: 0x%04x  -  tick: %lu\n", 
               instruction.mnemonic, op_code, cpu->registers.a_register, cpu->registers.x_register, cpu->registers.y_register, 
               cpu->registers.status_flags, cpu->registers.stack_pointer, cpu->registers.program_counter, cpu->tick_counter - 1);
        
        cpu->remaining_cycles = instruction.cycles;
        uint16_t absolute_address = instruction.address_mode(cpu);
        
        instruction.operator(cpu, absolute_address);
    }

    cpu->tick_counter++;
    cpu->remaining_cycles--;
}


uint8_t CPUDisassemble(
    struct CPU* cpu, uint16_t start_address, uint16_t count, 
    char disassembly_buffer[DISASSEMBLY_BUFFER_HEIGHT][DISASSEMBLY_BUFFER_WIDTH],
    char zero_page_buffer[ZERO_PAGE_BYTE_BUFFER_HEIGHT][ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH],
    char registers_buffer[REGISTERS_BUFFER_HEIGHT][REGISTER_WIDTH]
) {
    char registers_row_buffer[REGISTER_WIDTH + 1];
    for (int y = 0; y < REGISTERS_BUFFER_HEIGHT; y++) {
        for (int i = 0; i < (REGISTER_WIDTH + 1); i++) {
            registers_row_buffer[i] = ' ';
        }

        int x = REGISTER_WIDTH;
        switch (y) {
            case 0: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "A: 0x%02X", cpu->registers.a_register); registers_row_buffer[x] = ' '; break;
            case 1: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "X: 0x%02X", cpu->registers.x_register); registers_row_buffer[x] = ' '; break;
            case 2: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "Y: 0x%02X", cpu->registers.y_register); registers_row_buffer[x] = ' '; break;
            case 3: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "CARRY: %s", GetCarryFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 4: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "ZERO: %s", GetZeroFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 5: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "IRQ DISABLE: %s", GetIrqDisableFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 6: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "DECIMAL MODE: %s", GetDecimalModeFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 7: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "BRK COMMAND: %s", GetBrkCommandFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 8: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "UNUSED: %s", GetUnusedFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 9: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "OVERFLOW: %s", GetOverflowFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 10: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "NEGATIVE: %s", GetNegativeFlag(cpu) ? "True" : "False"); registers_row_buffer[x] = ' '; break;
            case 11: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "STACK POINTER: 0x%02X", cpu->registers.stack_pointer); registers_row_buffer[x] = ' '; break;
            case 12: x = snprintf(&registers_row_buffer[0], sizeof(registers_row_buffer), "PROGRAM COUNTER: 0x%04X", cpu->registers.program_counter); registers_row_buffer[x] = ' '; break;
        }

        memcpy(&registers_buffer[y], &registers_row_buffer[0], REGISTER_WIDTH * sizeof(char));
    }




    char zero_page_row_buffer[ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH + 1];

    for (int y = 0; y < ZERO_PAGE_BYTE_BUFFER_HEIGHT; y++) {
        for (int i = 0; i < (ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH + 1); i++) {
            zero_page_row_buffer[i] = ' ';
        } 
        for (int x = 0; x < ZERO_PAGE_BYTE_BUFFER_WIDTH; x++) {
            uint16_t temp_address = y * ZERO_PAGE_BYTE_BUFFER_WIDTH + x;
            if (temp_address > 0x00FF) {
                printf("indexing outside of zero page (possibly because incorrect buffer size/shape)\n");
                exit(1);
            }
            snprintf(&zero_page_row_buffer[x * ZERO_PAGE_BYTE_WIDTH], (ZERO_PAGE_BYTE_WIDTH + 1) * sizeof(char), "%02X ", ReadByte(cpu, temp_address));
        }
        memcpy(&zero_page_buffer[y], &zero_page_row_buffer[0], ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH * sizeof(char));
    }




    // note that for addressing modes that require a register can't fully be disassembled 
    // because registers are a runtime thing and this function only disassembles the code doesn't execute it 

    uint16_t dummy_address = start_address;
    uint16_t prev_dummy_address = start_address;

    uint8_t active_row = DISASSEMBLY_BUFFER_HEIGHT;
    char disassembly_row_buffer[DISASSEMBLY_BUFFER_WIDTH + 1];

    for (int y = 0; y < count && y < DISASSEMBLY_BUFFER_HEIGHT; y++) {
        for (int i = 0; i < (DISASSEMBLY_BUFFER_WIDTH + 1); i++) {
            disassembly_row_buffer[i] = ' ';
        }        

        int x = 0;
        
        if (dummy_address < 0x2000 || dummy_address >= 0x4020) { // with certain mappers this still could cause side effects but minimizes the chance
            uint8_t op_code = ReadByte(cpu, dummy_address);
            Instruction instruction = instructions[op_code];

            if ((x + 12) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                snprintf(&disassembly_row_buffer[x], 12 * sizeof(char), "0x%04X: %s", dummy_address, instruction.mnemonic);
            }
            x += 11;    // the reason for adding one less to x is because snprintf also prints a null terminator and this way the next snprintf overlaps

            dummy_address++;

            if (instruction.address_mode == &IlligalMode) {
                if ((x + 5) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                    snprintf(&disassembly_row_buffer[x], 5 * sizeof(char), " ???");
                }    
                x += 4;
            }
            else if (instruction.address_mode == &Accumulator) {
                if ((x + 5) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                    snprintf(&disassembly_row_buffer[x], 5 * sizeof(char), " ACC");
                }    
                x += 4;
            }
            else if (instruction.address_mode == &Implied) {
                if ((x + 5) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                    snprintf(&disassembly_row_buffer[x], 5 * sizeof(char), " IMP");
                } 
                x += 4;
            }
            else if (instruction.address_mode == &Immediate) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " IMM  0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &ZeroPage) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " ZP   0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &ZeroPageX) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " ZPX  0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &ZeroPageY) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " ZPY  0x%02X", temp_address);
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &Relative) {
                if (dummy_address < 0x2000 || dummy_address >= 0x4020) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " REL %c0x%02X", (((int8_t)temp_address < 0) ? '-' : '+'), (((int8_t)temp_address < 0) ? (-1 * (int8_t)temp_address) : temp_address));
                    } 
                    x += 10;
                }
            }
            else if (instruction.address_mode == &Absolute) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " ABS  0x%04X", temp_address);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &AbsoluteX) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " ABX  0x%04X", temp_address);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &AbsoluteY) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " ABY  0x%04X", temp_address);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &Indirect) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;

                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " IND  0x%04X", temp_address_ptr);
                    } 
                    x += 12;

                    uint16_t temp_address_ptr_to_low = ((temp_address_ptr & 0xFF00) | ((temp_address_ptr + 1) & 0x00FF));
                    if ((temp_address_ptr < 0x2000 || temp_address_ptr >= 0x4020) && (temp_address_ptr_to_low < 0x2000 || temp_address_ptr_to_low >= 0x4020)) {
                        uint16_t temp_address = (uint16_t)(ReadByte(cpu, temp_address_ptr_to_low) << 8) | (uint16_t)ReadByte(cpu, temp_address_ptr);
                      
                        if ((x + 9) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 9 * sizeof(char), "  0x%04X", temp_address);
                        } 
                        x += 8;
                    }
                }
            }
            else if (instruction.address_mode == &IndirectX) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;

                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " INX  0x%04X", temp_address_ptr);
                    } 
                    x += 12;
                }
            }
            else if (instruction.address_mode == &IndirectY) {
                if (dummy_address < 0x1FFF || dummy_address >= 0x4021) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;

                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " INY  0x%04X", temp_address_ptr);
                    } 
                    x += 12;
                }
            } else {
                printf("unknown addressing mode\n");
                exit(1);
            }
        }

        if (x >= DISASSEMBLY_BUFFER_WIDTH) {
            // this means something didn't fit
            printf("disassembly character buffer too thin\n");
            exit(1);
        }
        
        disassembly_row_buffer[x] = ' ';    // removes null terminator at the end
        memcpy(&disassembly_buffer[y], &disassembly_row_buffer[0], DISASSEMBLY_BUFFER_WIDTH * sizeof(char));

        if (cpu->registers.program_counter >= prev_dummy_address && cpu->registers.program_counter < dummy_address) {
            active_row = y;
        }

        prev_dummy_address = dummy_address;
    }

    return active_row;
}



//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc001
//mnemonic: LDX.LDX  -  opcode: 0xa2  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc5f6
//mnemonic: STX.STX  -  opcode: 0x86  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc5f8
//mnemonic: STX.STX  -  opcode: 0x86  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc5fa
//mnemonic: STX.STX  -  opcode: 0x86  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc5fc
//mnemonic: JSR.JSR  -  opcode: 0x20  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc5fe
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc72e
//mnemonic: SEC.SEC  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc72f
//mnemonic: BCS.BCS  -  opcode: 0xb0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc730
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc736
//mnemonic: CLC.CLC  -  opcode: 0x18  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc737
//mnemonic: BCS.BCS  -  opcode: 0xb0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc738
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc73a
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc741
//mnemonic: SEC.SEC  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc742
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc743
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc745
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc74c
//mnemonic: CLC.CLC  -  opcode: 0x18  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc74d
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc74e
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc754
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc755
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc757
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc75d
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc75e
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc760
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc762
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc769
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc76a
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc76c
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc772
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc773
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc775
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc777
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc77e
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc77f
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc781
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc783
//mnemonic: BVS.BVS  -  opcode: 0x70  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc785
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc78b
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc78c
//mnemonic: BVC.BVC  -  opcode: 0x50  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc78e
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc790
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc797
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc798
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x66.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc79a
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x66.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc79c
//mnemonic: BVC.BVC  -  opcode: 0x50  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc79e
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7a4
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7a5
//mnemonic: BVS.BVS  -  opcode: 0x70  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7a7
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7a9
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7b0
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7b1
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7b3
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7b9
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x26.26  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7ba
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7bc
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7be
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7da
//mnemonic: RTS.RTS  -  opcode: 0x60  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7db
//mnemonic: JSR.JSR  -  opcode: 0x20  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc601
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7dc
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7dd
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7df
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7e1
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe4.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7e3
//mnemonic: SEC.SEC  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x66.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7e5
//mnemonic: SEI.SEI  -  opcode: 0x78  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7e6
//mnemonic: SED.SED  -  opcode: 0xf8  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7e7
//mnemonic: PHP.PHP  -  opcode: 0x08  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7e8
//mnemonic: PLA.PLA  -  opcode: 0x68  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc7e9
//mnemonic: AND.AND  -  opcode: 0x29  -  A: 0x7f.7F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7ea
//mnemonic: CMP.CMP  -  opcode: 0xc9  -  A: 0x6f.6F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7ec
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x6f.6F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7ee
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x6f.6F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7f4
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x6f.6F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7f5
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7f7
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7f9
//mnemonic: CLD.CLD  -  opcode: 0xd8  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7fb
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x40.40  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x65.65  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7fc
//mnemonic: CLC.CLC  -  opcode: 0x18  -  A: 0x10.10  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x65.65  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7fe
//mnemonic: PHP.PHP  -  opcode: 0x08  -  A: 0x10.10  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x64.64  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc7ff
//mnemonic: PLA.PLA  -  opcode: 0x68  -  A: 0x10.10  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x64.64  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc800
//mnemonic: AND.AND  -  opcode: 0x29  -  A: 0x74.74  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x64.64  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc801
//mnemonic: CMP.CMP  -  opcode: 0xc9  -  A: 0x64.64  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x64.64  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc803
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x64.64  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc805
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x64.64  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc80b
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x64.64  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc80c
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe5.E5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc80e
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xe5.E5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc810
//mnemonic: SED.SED  -  opcode: 0xf8  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc812
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x80.80  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc813
//mnemonic: SEC.SEC  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc815
//mnemonic: PHP.PHP  -  opcode: 0x08  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc816
//mnemonic: PLA.PLA  -  opcode: 0x68  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc817
//mnemonic: AND.AND  -  opcode: 0x29  -  A: 0x3f.3F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2d.2D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc818
//mnemonic: CMP.CMP  -  opcode: 0xc9  -  A: 0x2f.2F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2d.2D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc81a
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x2f.2F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc81c
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x2f.2F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc822
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x2f.2F  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc823
//mnemonic: PHA.PHA  -  opcode: 0x48  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc825
//mnemonic: PLP.PLP  -  opcode: 0x28  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc826
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc827
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc829
//mnemonic: BVC.BVC  -  opcode: 0x50  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc82b
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc82d
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc82f
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc836
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xef.EF  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc837
//mnemonic: PHA.PHA  -  opcode: 0x48  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc839
//mnemonic: PLP.PLP  -  opcode: 0x28  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6d.6D  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc83a
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc83b
//mnemonic: BMI.BMI  -  opcode: 0x30  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc83d
//mnemonic: BVS.BVS  -  opcode: 0x70  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc83f
//mnemonic: BCS.BCS  -  opcode: 0xb0  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc841
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc843
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc84a
//mnemonic: SED.SED  -  opcode: 0xf8  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x24.24  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc84b
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x04.04  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2c.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc84c
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xac.AC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc84e
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xac.AC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc850
//mnemonic: CLC.CLC  -  opcode: 0x18  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc852
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc853
//mnemonic: PHA.PHA  -  opcode: 0x48  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc855
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc856
//mnemonic: PLA.PLA  -  opcode: 0x68  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc858
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc859
//mnemonic: BMI.BMI  -  opcode: 0x30  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc85b
//mnemonic: BVC.BVC  -  opcode: 0x50  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc85d
//mnemonic: BCS.BCS  -  opcode: 0xb0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc85f
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc861
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc868
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc869
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc86b
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc86d
//mnemonic: SEC.SEC  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2e.2E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc86f
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc870
//mnemonic: PHA.PHA  -  opcode: 0x48  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc872
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc873
//mnemonic: PLA.PLA  -  opcode: 0x68  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfa.FA  -  Program counter: 0xc875
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc876
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc878
//mnemonic: BVS.BVS  -  opcode: 0x70  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc87a
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc87c
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc87e
//mnemonic: RTS.RTS  -  opcode: 0x60  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc885
//mnemonic: JSR.JSR  -  opcode: 0x20  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfd.FD  -  Program counter: 0xc604
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc886
//mnemonic: CLC.CLC  -  opcode: 0x18  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc887
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xac.AC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc888
//mnemonic: STA.STA  -  opcode: 0x85  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xac.AC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc88a
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xac.AC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc88c
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc88e
//mnemonic: ORA.ORA  -  opcode: 0x09  -  A: 0x55.55  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6c.6C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc890
//mnemonic: BCS.BCS  -  opcode: 0xb0  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc892
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc894
//mnemonic: CMP.CMP  -  opcode: 0xc9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc896
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc898
//mnemonic: BVC.BVC  -  opcode: 0x50  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc89a
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc89c
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8a3
//mnemonic: SEC.SEC  -  opcode: 0x38  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8a4
//mnemonic: CLV.CLV  -  opcode: 0xb8  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8a5
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8a6
//mnemonic: ORA.ORA  -  opcode: 0x09  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8a8
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8aa
//mnemonic: BVS.BVS  -  opcode: 0x70  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ac
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ae
//mnemonic: BMI.BMI  -  opcode: 0x30  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8b0

//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8b2
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8b9
//mnemonic: CLC.CLC  -  opcode: 0x18  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ba
//mnemonic: BIT.BIT  -  opcode: 0x24  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2e.2E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8bb
//mnemonic: LDA.LDA  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2e.EE  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8bd
//mnemonic: AND.AND  -  opcode: 0x29  -  A: 0x55.55  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2c.6C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8bf
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8c1
//mnemonic: BVC.BVC  -  opcode: 0x50  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8c3
//mnemonic: LDX.BCS  -  opcode: 0xa2  -  A: 0x00.00  -  X: 0x00.00  -  Y: 0x00.00  -  Status: 0x2e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8cc
//mnemonic: STX.BMI  -  opcode: 0x86  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2c.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ce
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2c.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d0
//mnemonic: SEC.NOP  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2c.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d1
//mnemonic: CLV.SEC  -  opcode: 0xb8  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2d.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d2

//mnemonic: LDA.CLV  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2d.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d3
//mnemonic: AND.LDA  -  opcode: 0x29  -  A: 0xf8.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xad.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d5
//mnemonic: BCC.AND  -  opcode: 0x90  -  A: 0xe8.F8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d7
//mnemonic: BPL.BCC  -  opcode: 0x10  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8d9
//mnemonic: CMP.BPL  -  opcode: 0xc9  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xad.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8db
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.AD  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8dd
//mnemonic: BVS.BNE  -  opcode: 0x70  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8df
//mnemonic: JMP.BVS  -  opcode: 0x4c  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8e1
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8e8
//mnemonic: CLC.NOP  -  opcode: 0x18  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8e9
//mnemonic: BIT.CLC  -  opcode: 0x24  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ea
//mnemonic: LDA.BIT  -  opcode: 0xa9  -  A: 0xe8.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xec.2E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ec
//mnemonic: EOR.LDA  -  opcode: 0x49  -  A: 0x5f.E8  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6c.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8ee
//mnemonic: BCS.EOR  -  opcode: 0xb0  -  A: 0xf5.5F  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xec.6C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8f0
//mnemonic: BPL.BCS  -  opcode: 0x10  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8f2
//mnemonic: CMP.BPL  -  opcode: 0xc9  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0xec.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8f4
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6f.EC  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8f6
//mnemonic: BVC.BNE  -  opcode: 0x50  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8f8
//mnemonic: JMP.BVC  -  opcode: 0x4c  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc8fa
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc901
//mnemonic: SEC.NOP  -  opcode: 0x38  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc902
//mnemonic: CLV.SEC  -  opcode: 0xb8  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc903
//mnemonic: LDA.CLV  -  opcode: 0xa9  -  A: 0xf5.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.6F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc904
//mnemonic: EOR.LDA  -  opcode: 0x49  -  A: 0x70.F5  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2d.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc906
//mnemonic: BNE.EOR  -  opcode: 0xd0  -  A: 0x00.70  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc908
//mnemonic: BVS.BNE  -  opcode: 0x70  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc90a
//mnemonic: BCC.BVS  -  opcode: 0x90  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc90c
//mnemonic: BMI.BCC  -  opcode: 0x30  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc90e
//mnemonic: JMP.BMI  -  opcode: 0x4c  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc910
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc917
//mnemonic: CLC.NOP  -  opcode: 0x18  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc918
//mnemonic: BIT.CLC  -  opcode: 0x24  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc919
//mnemonic: LDA.BIT  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc91b
//mnemonic: ADC.LDA  -  opcode: 0x69  -  A: 0x00.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.EE  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc91d
//mnemonic: BMI.ADC  -  opcode: 0x30  -  A: 0x69.00  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.6E  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc91f
//mnemonic: BCS.BMI  -  opcode: 0xb0  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc921
//mnemonic: CMP.BCS  -  opcode: 0xc9  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc923
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc925
//mnemonic: BVS.BNE  -  opcode: 0x70  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc927
//mnemonic: JMP.BVS  -  opcode: 0x4c  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc929
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc930
//mnemonic: SEC.NOP  -  opcode: 0x38  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc931
//mnemonic: SED.SEC  -  opcode: 0xf8  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc932
//mnemonic: BIT.SED  -  opcode: 0x24  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2f.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc933
//mnemonic: LDA.BIT  -  opcode: 0xa9  -  A: 0x69.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6d.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc935
//mnemonic: ADC.LDA  -  opcode: 0x69  -  A: 0x01.69  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x6d.ED  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc937
//mnemonic: BMI.ADC  -  opcode: 0x30  -  A: 0x71.01  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.6D  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc939
//mnemonic: BCS.BMI  -  opcode: 0xb0  -  A: 0x71.6B  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc93b
//mnemonic: CMP.BCS  -  opcode: 0xc9  -  A: 0x71.6B  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2e.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc93d
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0x71.6B  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2d.2C  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc93f
//mnemonic: LDX.BNE  -  opcode: 0xa2  -  A: 0x71.6B  -  X: 0x1a.00  -  Y: 0x00.00  -  Status: 0x2d.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc946
//mnemonic: STX.BVS  -  opcode: 0x86  -  A: 0x71.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x2d.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc948
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x71.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x2d.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc94a
//mnemonic: CLD.NOP  -  opcode: 0xd8  -  A: 0x71.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x2d.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc94b
//mnemonic: SEC.CLD  -  opcode: 0x38  -  A: 0x71.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.2F  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc94c
//mnemonic: CLV.SEC  -  opcode: 0xb8  -  A: 0x71.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc94d
//mnemonic: LDA.CLV  -  opcode: 0xa9  -  A: 0x71.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc94e
//mnemonic: ADC.LDA  -  opcode: 0x69  -  A: 0x7f.6B  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc950
//mnemonic: BPL.ADC  -  opcode: 0x10  -  A: 0xff.7F  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xe6.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc952
//mnemonic: BCS.BPL  -  opcode: 0xb0  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xe6.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc954
//mnemonic: CMP.BCS  -  opcode: 0xc9  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xe6.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc956
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x67.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc958
//mnemonic: BVC.BNE  -  opcode: 0x50  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc95a
//mnemonic: JMP.BVC  -  opcode: 0x4c  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc95c
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc963
//mnemonic: CLC.NOP  -  opcode: 0x18  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x67.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc964
//mnemonic: BIT.CLC  -  opcode: 0x24  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x66.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc965
//mnemonic: LDA.BIT  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xe4.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc967
//mnemonic: ADC.LDA  -  opcode: 0x69  -  A: 0x7f.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x64.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc969
//mnemonic: BPL.ADC  -  opcode: 0x10  -  A: 0xff.7F  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xa6.64  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc96b
//mnemonic: BCS.BPL  -  opcode: 0xb0  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xa6.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc96d
//mnemonic: CMP.BCS  -  opcode: 0xc9  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0xa6.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc96f
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc971
//mnemonic: BVS.BNE  -  opcode: 0x70  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc973
//mnemonic: JMP.BVS  -  opcode: 0x4c  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc975
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc97c
//mnemonic: SEC.NOP  -  opcode: 0x38  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc97d
//mnemonic: CLV.SEC  -  opcode: 0xb8  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc97e
//mnemonic: LDA.CLV  -  opcode: 0xa9  -  A: 0xff.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc97f
//mnemonic: ADC.LDA  -  opcode: 0x69  -  A: 0x7f.FF  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc981
//mnemonic: BNE.ADC  -  opcode: 0xd0  -  A: 0x00.7F  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc983
//mnemonic: LDX.BNE  -  opcode: 0xa2  -  A: 0x00.00  -  X: 0x1f.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc98e
//mnemonic: STX.BMI  -  opcode: 0x86  -  A: 0x00.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc990
//mnemonic: NOP.BVS  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc992
//mnemonic: SEC.BCC  -  opcode: 0x38  -  A: 0x00.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc993
//mnemonic: CLV.JMP  -  opcode: 0xb8  -  A: 0x00.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc994
//mnemonic: LDA.NOP  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc995
//mnemonic: BEQ.SEC  -  opcode: 0xf0  -  A: 0x9f.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc997
//mnemonic: BPL.CLV  -  opcode: 0x10  -  A: 0x9f.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc999
//mnemonic: BVS.LDA  -  opcode: 0x70  -  A: 0x9f.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc99b
//mnemonic: BCC.BEQ  -  opcode: 0x90  -  A: 0x9f.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc99d
//mnemonic: JMP.BPL  -  opcode: 0x4c  -  A: 0x9f.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc99f
//mnemonic: NOP.BVS  -  opcode: 0xea  -  A: 0x9f.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9a6
//mnemonic: CLC.BCC  -  opcode: 0x18  -  A: 0x9f.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9a7
//mnemonic: BIT.JMP  -  opcode: 0x24  -  A: 0x9f.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa4.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9a8
//mnemonic: LDA.NOP  -  opcode: 0xa9  -  A: 0x9f.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0xa4.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9aa
//mnemonic: BNE.CLC  -  opcode: 0xd0  -  A: 0x00.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x26.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9ac
//mnemonic: BMI.BIT  -  opcode: 0x30  -  A: 0x00.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x26.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9ae
//mnemonic: BVC.LDA  -  opcode: 0x50  -  A: 0x00.9F  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x26.E4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9b0
//mnemonic: LDX.BNE  -  opcode: 0xa2  -  A: 0x00.00  -  X: 0x22.00  -  Y: 0x00.00  -  Status: 0x26.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9b7
//mnemonic: STX.BMI  -  opcode: 0x86  -  A: 0x00.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x24.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9b9
//mnemonic: NOP.BVC  -  opcode: 0xea  -  A: 0x00.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x24.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9bb
//mnemonic: BIT.BCS  -  opcode: 0x24  -  A: 0x00.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x24.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9bc
//mnemonic: LDA.JMP  -  opcode: 0xa9  -  A: 0x00.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x26.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9be
//mnemonic: CMP.NOP  -  opcode: 0xc9  -  A: 0x40.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x24.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9c0
//mnemonic: BMI.BIT  -  opcode: 0x30  -  A: 0x40.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x27.66  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9c2
//mnemonic: BCC.LDA  -  opcode: 0x90  -  A: 0x40.00  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x27.E6  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9c4
//mnemonic: BNE.CMP  -  opcode: 0xd0  -  A: 0x40.40  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x27.64  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9c6
//mnemonic: BVC.BMI  -  opcode: 0x50  -  A: 0x40.40  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9c8
//mnemonic: LDX.BCC  -  opcode: 0xa2  -  A: 0x40.40  -  X: 0x23.00  -  Y: 0x00.00  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9cd
//mnemonic: STX.BNE  -  opcode: 0x86  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9cf
//mnemonic: NOP.BVC  -  opcode: 0xea  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9d1
//mnemonic: CLV.JMP  -  opcode: 0xb8  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9d2
//mnemonic: CMP.NOP  -  opcode: 0xc9  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9d3
//mnemonic: BEQ.CLV  -  opcode: 0xf0  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9d5
//mnemonic: BMI.CMP  -  opcode: 0x30  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9d7
//mnemonic: BCC.BEQ  -  opcode: 0x90  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9d9
//mnemonic: BVS.BMI  -  opcode: 0x70  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9db
//mnemonic: JMP.BCC  -  opcode: 0x4c  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9dd
//mnemonic: NOP.BVS  -  opcode: 0xea  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9e4
//mnemonic: CMP.JMP  -  opcode: 0xc9  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9e5
//mnemonic: BEQ.NOP  -  opcode: 0xf0  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9e7
//mnemonic: BPL.CMP  -  opcode: 0x10  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9e9
//mnemonic: BPL.BEQ  -  opcode: 0x10  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9eb
//mnemonic: JMP.BPL  -  opcode: 0x4c  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9ed
//mnemonic: NOP.BPL  -  opcode: 0xea  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9f4
//mnemonic: LDA.JMP  -  opcode: 0xa9  -  A: 0x40.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9f5
//mnemonic: CMP.NOP  -  opcode: 0xc9  -  A: 0x80.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9f7
//mnemonic: BEQ.LDA  -  opcode: 0xf0  -  A: 0x80.40  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9f9
//mnemonic: BPL.CMP  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9fb
//mnemonic: BCC.BEQ  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9fd
//mnemonic: JMP.BPL  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xc9ff
//mnemonic: NOP.BCC  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca06
//mnemonic: CMP.JMP  -  opcode: 0xc9  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca07
//mnemonic: BNE.NOP  -  opcode: 0xd0  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x27.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca09
//mnemonic: BMI.CMP  -  opcode: 0x30  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x27.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca0b
//mnemonic: BCC.BNE  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca0d
//mnemonic: JMP.BMI  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca0f
//mnemonic: NOP.BCC  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca16
//mnemonic: CMP.JMP  -  opcode: 0xc9  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca17
//mnemonic: BCS.NOP  -  opcode: 0xb0  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca19
//mnemonic: BEQ.CMP  -  opcode: 0xf0  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca1b
//mnemonic: BPL.BCS  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca1d
//mnemonic: JMP.BEQ  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca1f
//mnemonic: NOP.BPL  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca26
//mnemonic: CMP.JMP  -  opcode: 0xc9  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca27
//mnemonic: BCC.NOP  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca29BIT
//mnemonic: NOP.BMI  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca36
//mnemonic: BIT.JMP  -  opcode: 0x24  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca37
//mnemonic: LDY.NOP  -  opcode: 0xa0  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x00.00  -  Status: 0xa5.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca39
//mnemonic: CPY.BIT  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x40.00  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca3b
//mnemonic: BNE.LDY  -  opcode: 0xd0  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x40.00  -  Status: 0x27.E5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca3d
//mnemonic: BMI.CPY  -  opcode: 0x30  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x40.40  -  Status: 0x27.65  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca3f
//mnemonic: BCC.BNE  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x40.40  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca41
//mnemonic: BVC.BMI  -  opcode: 0x50  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x40.40  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca43
//mnemonic: LDX.BCC  -  opcode: 0xa2  -  A: 0x80.80  -  X: 0x24.00  -  Y: 0x40.40  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca48
//mnemonic: STX.BVC  -  opcode: 0x86  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca4a
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca4c
//mnemonic: CLV.NOP  -  opcode: 0xb8  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca4d
//mnemonic: CPY.CLV  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca4e
//mnemonic: BEQ.CPY  -  opcode: 0xf0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca50
//mnemonic: BMI.BEQ  -  opcode: 0x30  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca52
//mnemonic: BCC.BMI  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca54
//mnemonic: BVS.BCC  -  opcode: 0x70  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca56
//mnemonic: JMP.BVS  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca58
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca5f
//mnemonic: CPY.NOP  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca60
//mnemonic: BEQ.CPY  -  opcode: 0xf0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0xa4.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca62
//mnemonic: BPL.BEQ  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca64
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca66
//mnemonic: JMP.BPL  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca68
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca6f
//mnemonic: LDY.NOP  -  opcode: 0xa0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x40.40  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca70
//mnemonic: CPY.LDY  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.40  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca72
//mnemonic: BEQ.CPY  -  opcode: 0xf0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca74
//mnemonic: BPL.BEQ  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca76
//mnemonic: BCC.BPL  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca78
//mnemonic: JMP.BCC  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca7a
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca81
//mnemonic: CPY.NOP  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca82
//mnemonic: BNE.CPY  -  opcode: 0xd0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x27.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca84
//mnemonic: BMI.BNE  -  opcode: 0x30  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca86
//mnemonic: BCC.BMI  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca88
//mnemonic: JMP.BCC  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca8a
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca91
//mnemonic: CPY.NOP  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca92
//mnemonic: BCS.CPY  -  opcode: 0xb0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa4.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca94
//mnemonic: BEQ.BCS  -  opcode: 0xf0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca96
//mnemonic: BPL.BEQ  -  opcode: 0x10  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca98
//mnemonic: JMP.BPL  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xca9a
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaa1
//mnemonic: CPY.NOP  -  opcode: 0xc0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaa2
//mnemonic: BCC.CPY  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x25.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaa4
//mnemonic: BEQ.BCC  -  opcode: 0xf0  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaa6
//mnemonic: BMI.BEQ  -  opcode: 0x30  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaa8
//mnemonic: JMP.BMI  -  opcode: 0x4c  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaaa
//mnemonic: NOP.JMP  -  opcode: 0xea  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcab1
//mnemonic: BIT.NOP  -  opcode: 0x24  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcab2
//mnemonic: LDX.BIT  -  opcode: 0xa2  -  A: 0x80.80  -  X: 0x2b.00  -  Y: 0x80.80  -  Status: 0xa5.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcab4
//mnemonic: CPX.LDX  -  opcode: 0xe0  -  A: 0x80.80  -  X: 0x40.00  -  Y: 0x80.80  -  Status: 0x25.E5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcab6
//mnemonic: BNE.CPX  -  opcode: 0xd0  -  A: 0x80.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x27.65  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcab8
//mnemonic: BMI.BNE  -  opcode: 0x30  -  A: 0x80.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaba
//mnemonic: BCC.BMI  -  opcode: 0x90  -  A: 0x80.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcabc
//mnemonic: BVC.BCC  -  opcode: 0x50  -  A: 0x80.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcabe
//mnemonic: LDA.BVC  -  opcode: 0xa9  -  A: 0x80.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x27.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcac3
//mnemonic: STA.JMP  -  opcode: 0x85  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcac5
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcac7
//mnemonic: CLV.CLV  -  opcode: 0xb8  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.67  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcac8
//mnemonic: CPX.CPX  -  opcode: 0xe0  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcac9
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcacb
//mnemonic: BMI.BMI  -  opcode: 0x30  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcacd
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcacf
//mnemonic: BVS.BVS  -  opcode: 0x70  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcad1
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcad3
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcada
//mnemonic: CPX.CPX  -  opcode: 0xe0  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0x25.25  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcadb
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcadd
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcadf
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcae1
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcae3
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaea
//mnemonic: LDX.LDX  -  opcode: 0xa2  -  A: 0x32.80  -  X: 0x40.40  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaeb
//mnemonic: CPX.CPX  -  opcode: 0xe0  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa4.A4  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaed
//mnemonic: BEQ.BEQ  -  opcode: 0xf0  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaef
//mnemonic: BPL.BPL  -  opcode: 0x10  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaf1
//mnemonic: BCC.BCC  -  opcode: 0x90  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaf3
//mnemonic: JMP.JMP  -  opcode: 0x4c  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaf5
//mnemonic: NOP.NOP  -  opcode: 0xea  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcafc
//mnemonic: CPX.CPX  -  opcode: 0xe0  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0xa5.A5  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcafd
//mnemonic: BNE.BNE  -  opcode: 0xd0  -  A: 0x32.80  -  X: 0x80.80  -  Y: 0x80.80  -  Status: 0x27.27  -  Stack pointer: 0xfb.FB  -  Program counter: 0xcaff



//mnemonic: BMI  -  opcode: 0x30  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb01
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb03
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb05
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb0c
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb0d
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb0f
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb11
//mnemonic: BPL  -  opcode: 0x10  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb13
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb15
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb1c
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb1d
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb1f
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb21
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb23
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb25
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb2c
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb2d
//mnemonic: CLV  -  opcode: 0xb8  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb2e
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x32  -  X: 0x80  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb2f
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb31
//mnemonic: BPL  -  opcode: 0x10  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb33
//mnemonic: BVS  -  opcode: 0x70  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb35
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb37
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb39
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb40
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb41
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb42
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x32  -  X: 0x9f  -  Y: 0x80  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb44
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x32  -  X: 0x00  -  Y: 0x80  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb46
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x32  -  X: 0x00  -  Y: 0x80  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb48
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x32  -  X: 0x00  -  Y: 0x80  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb4a
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x32  -  X: 0x00  -  Y: 0x80  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb51
//mnemonic: STX  -  opcode: 0x86  -  A: 0x32  -  X: 0x3a  -  Y: 0x80  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb53
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x3a  -  Y: 0x80  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb55
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x32  -  X: 0x3a  -  Y: 0x80  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb56
//mnemonic: CLV  -  opcode: 0xb8  -  A: 0x32  -  X: 0x3a  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb57
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x32  -  X: 0x3a  -  Y: 0x80  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcb58
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb5a
//mnemonic: BPL  -  opcode: 0x10  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb5c
//mnemonic: BVS  -  opcode: 0x70  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb5e
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb60
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb62
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb69
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcb6a
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb6b
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x32  -  X: 0x3a  -  Y: 0x9f  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb6d
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x32  -  X: 0x3a  -  Y: 0x00  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb6f
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x32  -  X: 0x3a  -  Y: 0x00  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb71
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x32  -  X: 0x3a  -  Y: 0x00  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb73
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x32  -  X: 0x3a  -  Y: 0x00  -  Status: 0x26  -  Stack pointer: 0xfb  -  Program counter: 0xcb7a
//mnemonic: STX  -  opcode: 0x86  -  A: 0x32  -  X: 0x3c  -  Y: 0x00  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb7c
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x32  -  X: 0x3c  -  Y: 0x00  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb7e
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x32  -  X: 0x3c  -  Y: 0x00  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb7f
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x55  -  X: 0x3c  -  Y: 0x00  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb81
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x55  -  X: 0xaa  -  Y: 0x00  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb83
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcb85
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb87
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb89
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb8b
//mnemonic: CPY  -  opcode: 0xc0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb8d
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb8f
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb91
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb93
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb95
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb97
//mnemonic: CPY  -  opcode: 0xc0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb99
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb9b
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcb9d
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcb9f
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcba1
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcba3
//mnemonic: CPY  -  opcode: 0xc0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcba5
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcba7
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcba9
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x55  -  X: 0xaa  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcbb0
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x55  -  X: 0xaa  -  Y: 0x71  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcbb2
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x55  -  X: 0xaa  -  Y: 0x71  -  Status: 0x24  -  Stack pointer: 0xf9  -  Program counter: 0xf932
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x55  -  X: 0xaa  -  Y: 0x71  -  Status: 0x64  -  Stack pointer: 0xf9  -  Program counter: 0xf934
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x40  -  X: 0xaa  -  Y: 0x71  -  Status: 0x64  -  Stack pointer: 0xf9  -  Program counter: 0xf936
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x40  -  X: 0xaa  -  Y: 0x71  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf937
//mnemonic: SBC  -  opcode: 0xe9  -  A: 0x40  -  X: 0xaa  -  Y: 0x71  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbb5
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcbb7
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf938
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf93a
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf93c
//mnemonic: BVS  -  opcode: 0x70  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf93e
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf940
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf942
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf944
//mnemonic: INY  -  opcode: 0xc8  -  A: 0x00  -  X: 0xaa  -  Y: 0x71  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcbba
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x00  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcbbb
//mnemonic: CLV  -  opcode: 0xb8  -  A: 0x00  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf948
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x00  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf949
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x00  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf94a
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x40  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf94c
//mnemonic: SBC  -  opcode: 0xe9  -  A: 0x40  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcbbe
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcbc0
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf94d
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf94f
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf951
//mnemonic: BVS  -  opcode: 0x70  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf953
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf955
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf957
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf959
//mnemonic: INY  -  opcode: 0xc8  -  A: 0x01  -  X: 0xaa  -  Y: 0x72  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcbc3
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x01  -  X: 0xaa  -  Y: 0x73  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcbc4
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x01  -  X: 0xaa  -  Y: 0x73  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf95d
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x40  -  X: 0xaa  -  Y: 0x73  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf95f
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x40  -  X: 0xaa  -  Y: 0x73  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf960
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x40  -  X: 0xaa  -  Y: 0x73  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf962
//mnemonic: SBC  -  opcode: 0xe9  -  A: 0x40  -  X: 0xaa  -  Y: 0x73  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbc7
//mnemonic: JSR  -  opcode: 0x20  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcbc9
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0xa4  -  Stack pointer: 0xf9  -  Program counter: 0xf963
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0xa4  -  Stack pointer: 0xf9  -  Program counter: 0xf965
//mnemonic: BPL  -  opcode: 0x10  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0xa4  -  Stack pointer: 0xf9  -  Program counter: 0xf967
//mnemonic: BVS  -  opcode: 0x70  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0xa4  -  Stack pointer: 0xf9  -  Program counter: 0xf969
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0xa4  -  Stack pointer: 0xf9  -  Program counter: 0xf96b
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf96d
//mnemonic: RTS  -  opcode: 0x60  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0x27  -  Stack pointer: 0xf9  -  Program counter: 0xf96f
//mnemonic: INY  -  opcode: 0xc8  -  A: 0xff  -  X: 0xaa  -  Y: 0x73  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcbcc
//mnemonic: JSR  -  opcode: 0x20  -  A: 0xff  -  X: 0xaa  -  Y: 0x74  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcbcd
//mnemonic: CLC  -  opcode: 0x18  -  A: 0xff  -  X: 0xaa  -  Y: 0x74  -  Status: 0x25  -  Stack pointer: 0xf9  -  Program counter: 0xf973
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0xff  -  X: 0xaa  -  Y: 0x74  -  Status: 0x24  -  Stack pointer: 0xf9  -  Program counter: 0xf974
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x80  -  X: 0xaa  -  Y: 0x74  -  Status: 0xa4  -  Stack pointer: 0xf9  -  Program counter: 0xf976
//mnemonic: SBC  -  opcode: 0xe9  -  A: 0x80  -  X: 0xaa  -  Y: 0x74  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcbd0
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x7f  -  X: 0xaa  -  Y: 0x74  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbd2
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x7f  -  X: 0xaa  -  Y: 0x74  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf977
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x7f  -  X: 0xaa  -  Y: 0x74  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf979
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x7f  -  X: 0xaa  -  Y: 0x74  -  Status: 0x67  -  Stack pointer: 0xf9  -  Program counter: 0xf97b
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x7f  -  X: 0xaa  -  Y: 0x74  -  Status: 0x67  -  Stack pointer: 0xf9  -  Program counter: 0xf97d
//mnemonic: INY  -  opcode: 0xc8  -  A: 0x7f  -  X: 0xaa  -  Y: 0x74  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbd5
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x7f  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbd6
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x7f  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf981
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x7f  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf982
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x81  -  X: 0xaa  -  Y: 0x75  -  Status: 0xe5  -  Stack pointer: 0xf9  -  Program counter: 0xf984
//mnemonic: SBC  -  opcode: 0xe9  -  A: 0x81  -  X: 0xaa  -  Y: 0x75  -  Status: 0xe5  -  Stack pointer: 0xfb  -  Program counter: 0xcbd9
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbdb
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf985
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf987
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xf9  -  Program counter: 0xf989
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x67  -  Stack pointer: 0xf9  -  Program counter: 0xf98b
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x67  -  Stack pointer: 0xf9  -  Program counter: 0xf98d
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbde
//mnemonic: JSR  -  opcode: 0x20  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x67  -  Stack pointer: 0xfd  -  Program counter: 0xc607
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbdf
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x02  -  X: 0xaa  -  Y: 0x75  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbe0
//mnemonic: STA  -  opcode: 0x85  -  A: 0xff  -  X: 0xaa  -  Y: 0x75  -  Status: 0xe5  -  Stack pointer: 0xfb  -  Program counter: 0xcbe2
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0xff  -  X: 0xaa  -  Y: 0x75  -  Status: 0xe5  -  Stack pointer: 0xfb  -  Program counter: 0xcbe4
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x44  -  X: 0xaa  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbe6
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x44  -  X: 0x55  -  Y: 0x75  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbe8
//mnemonic: INX  -  opcode: 0xe8  -  A: 0x44  -  X: 0x55  -  Y: 0x66  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbea
//mnemonic: DEY  -  opcode: 0x88  -  A: 0x44  -  X: 0x56  -  Y: 0x66  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbeb
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x44  -  X: 0x56  -  Y: 0x65  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbec
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x56  -  Y: 0x65  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbee
//mnemonic: CPY  -  opcode: 0xc0  -  A: 0x44  -  X: 0x56  -  Y: 0x65  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbf0
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x56  -  Y: 0x65  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbf2
//mnemonic: INX  -  opcode: 0xe8  -  A: 0x44  -  X: 0x56  -  Y: 0x65  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbf4
//mnemonic: INX  -  opcode: 0xe8  -  A: 0x44  -  X: 0x57  -  Y: 0x65  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbf5
//mnemonic: DEY  -  opcode: 0x88  -  A: 0x44  -  X: 0x58  -  Y: 0x65  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbf6
//mnemonic: DEY  -  opcode: 0x88  -  A: 0x44  -  X: 0x58  -  Y: 0x64  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbf7
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x44  -  X: 0x58  -  Y: 0x63  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcbf8
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x58  -  Y: 0x63  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbfa
//mnemonic: CPY  -  opcode: 0xc0  -  A: 0x44  -  X: 0x58  -  Y: 0x63  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbfc
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x58  -  Y: 0x63  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcbfe
//mnemonic: DEX  -  opcode: 0xca  -  A: 0x44  -  X: 0x58  -  Y: 0x63  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc00
//mnemonic: INY  -  opcode: 0xc8  -  A: 0x44  -  X: 0x57  -  Y: 0x63  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcc01
//mnemonic: CPX  -  opcode: 0xe0  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcc02
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc04
//mnemonic: CPY  -  opcode: 0xc0  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc06
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc08
//mnemonic: CMP  -  opcode: 0xc9  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc0a
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc0c
//mnemonic: JMP  -  opcode: 0x4c  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc0e
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc15
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc16
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x44  -  X: 0x57  -  Y: 0x64  -  Status: 0x67  -  Stack pointer: 0xfb  -  Program counter: 0xcc17
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x44  -  X: 0x69  -  Y: 0x64  -  Status: 0x65  -  Stack pointer: 0xfb  -  Program counter: 0xcc19
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x96  -  X: 0x69  -  Y: 0x64  -  Status: 0xe5  -  Stack pointer: 0xfb  -  Program counter: 0xcc1b
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x96  -  X: 0x69  -  Y: 0x64  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcc1d
//mnemonic: INY  -  opcode: 0xc8  -  A: 0x96  -  X: 0x69  -  Y: 0xff  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcc1f
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x96  -  X: 0x69  -  Y: 0x00  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc20
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x96  -  X: 0x69  -  Y: 0x00  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc22
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x96  -  X: 0x69  -  Y: 0x00  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc24
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x96  -  X: 0x69  -  Y: 0x00  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc26
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x96  -  X: 0x69  -  Y: 0x00  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc5f
//mnemonic: STX  -  opcode: 0x86  -  A: 0x96  -  X: 0x3f  -  Y: 0x00  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcc61
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x96  -  X: 0x3f  -  Y: 0x00  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcc63
//mnemonic: SEC  -  opcode: 0x38  -  A: 0x96  -  X: 0x3f  -  Y: 0x00  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcc64
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x96  -  X: 0x3f  -  Y: 0x00  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcc65
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x96  -  X: 0x3f  -  Y: 0x69  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xcc67
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x96  -  X: 0x3f  -  Y: 0x69  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcc69
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x96  -  X: 0x3f  -  Y: 0x69  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcc6b
//mnemonic: INX  -  opcode: 0xe8  -  A: 0x96  -  X: 0xff  -  Y: 0x69  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xcc6d
//mnemonic: BNE  -  opcode: 0xd0  -  A: 0x96  -  X: 0x00  -  Y: 0x69  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc6e
//mnemonic: BMI  -  opcode: 0x30  -  A: 0x96  -  X: 0x00  -  Y: 0x69  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc70
//mnemonic: BCC  -  opcode: 0x90  -  A: 0x96  -  X: 0x00  -  Y: 0x69  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc72
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x96  -  X: 0x00  -  Y: 0x69  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xcc74
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x96  -  X: 0x00  -  Y: 0x69  -  Status: 0x27  -  Stack pointer: 0xfb  -  Program counter: 0xccad
//mnemonic: STX  -  opcode: 0x86  -  A: 0x96  -  X: 0x40  -  Y: 0x69  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xccaf
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x96  -  X: 0x40  -  Y: 0x69  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xccb1
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x96  -  X: 0x40  -  Y: 0x69  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xccb2
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x85  -  X: 0x40  -  Y: 0x69  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xccb4
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x85  -  X: 0x34  -  Y: 0x69  -  Status: 0x25  -  Stack pointer: 0xfb  -  Program counter: 0xccb6
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa5  -  Stack pointer: 0xfb  -  Program counter: 0xccb8
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccb9
//mnemonic: TAY  -  opcode: 0xa8  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccbb
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x85  -  X: 0x34  -  Y: 0x85  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccbc
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0x85  -  X: 0x34  -  Y: 0x85  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccbe
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x85  -  X: 0x34  -  Y: 0x85  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccc0
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x85  -  X: 0x34  -  Y: 0x85  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccec
//mnemonic: STX  -  opcode: 0x86  -  A: 0x85  -  X: 0x41  -  Y: 0x85  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xccee
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x85  -  X: 0x41  -  Y: 0x85  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xccf0
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x85  -  X: 0x41  -  Y: 0x85  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xccf1
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x85  -  X: 0x41  -  Y: 0x85  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccf3
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x85  -  X: 0x34  -  Y: 0x85  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xccf5
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccf7
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccf8
//mnemonic: TAX  -  opcode: 0xaa  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccfa
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x85  -  X: 0x85  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccfb
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0x85  -  X: 0x85  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccfd
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x85  -  X: 0x85  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xccff
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x85  -  X: 0x85  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd2b
//mnemonic: STX  -  opcode: 0x86  -  A: 0x85  -  X: 0x42  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd2d
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x85  -  X: 0x42  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd2f
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x85  -  X: 0x42  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd30
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x85  -  X: 0x42  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd32
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd34
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd36
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd37
//mnemonic: TYA  -  opcode: 0x98  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd39
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x99  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd3a
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0x99  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd3c
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x99  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd3e
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x99  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd6a
//mnemonic: STX  -  opcode: 0x86  -  A: 0x99  -  X: 0x43  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd6c
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x99  -  X: 0x43  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd6e
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x99  -  X: 0x43  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd6f
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x85  -  X: 0x43  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd71
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd73
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd75
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd76
//mnemonic: TXA  -  opcode: 0x8a  -  A: 0x85  -  X: 0x34  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcd78
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x34  -  X: 0x34  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd79
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0x34  -  X: 0x34  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd7b
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x34  -  X: 0x34  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcd7d
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x34  -  X: 0x34  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcda9
//mnemonic: STX  -  opcode: 0x86  -  A: 0x34  -  X: 0x44  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcdab
//mnemonic: NOP  -  opcode: 0xea  -  A: 0x34  -  X: 0x44  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcdad
//mnemonic: TSX  -  opcode: 0xba  -  A: 0x34  -  X: 0x44  -  Y: 0x99  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcdae
//mnemonic: STX  -  opcode: 0x8e  -  A: 0x34  -  X: 0xfb  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcdaf
//mnemonic: LDY  -  opcode: 0xa0  -  A: 0x34  -  X: 0xfb  -  Y: 0x99  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcdb2
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x34  -  X: 0xfb  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcdb4
//mnemonic: LDA  -  opcode: 0xa9  -  A: 0x34  -  X: 0x69  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0xfb  -  Program counter: 0xcdb6
//mnemonic: CLC  -  opcode: 0x18  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcdb8
//mnemonic: BIT  -  opcode: 0x24  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcdb9
//mnemonic: TXS  -  opcode: 0x9a  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0xfb  -  Program counter: 0xcdbb
//mnemonic: BEQ  -  opcode: 0xf0  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0x69  -  Program counter: 0xcdbc
//mnemonic: BPL  -  opcode: 0x10  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0x69  -  Program counter: 0xcdbe
//mnemonic: BCS  -  opcode: 0xb0  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0x69  -  Program counter: 0xcdc0
//mnemonic: BVC  -  opcode: 0x50  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0x69  -  Program counter: 0xcdc2
//mnemonic: LDX  -  opcode: 0xa2  -  A: 0x84  -  X: 0x69  -  Y: 0x33  -  Status: 0xa4  -  Stack pointer: 0x69  -  Program counter: 0xcdf0
//mnemonic: STX  -  opcode: 0x86  -  A: 0x84  -  X: 0x45  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0x69  -  Program counter: 0xcdf2
//mnemonic: LDX  -  opcode: 0xae  -  A: 0x84  -  X: 0x45  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0x69  -  Program counter: 0xcdf4
//mnemonic: TXS  -  opcode: 0x9a  -  A: 0x84  -  X: 0x07  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0x69  -  Program counter: 0xcdf7
//mnemonic: RTS  -  opcode: 0x60  -  A: 0x84  -  X: 0x07  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0x07  -  Program counter: 0xcdf8
//mnemonic: ???  -  opcode: 0xff  -  A: 0x84  -  X: 0x07  -  Y: 0x33  -  Status: 0x24  -  Stack pointer: 0x09  -  Program counter: 0x0002






//C000  JMP  A:00 X:00 Y:00 P:24 SP:FD 
//C5F5  LDX  A:00 X:00 Y:00 P:24 SP:FD 
//C5F7  STX  A:00 X:00 Y:00 P:26 SP:FD 
//C5F9  STX  A:00 X:00 Y:00 P:26 SP:FD 
//C5FB  STX  A:00 X:00 Y:00 P:26 SP:FD 
//C5FD  JSR  A:00 X:00 Y:00 P:26 SP:FD 
//C72D  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C72E  SEC  A:00 X:00 Y:00 P:26 SP:FB 
//C72F  BCS  A:00 X:00 Y:00 P:27 SP:FB 
//C735  NOP  A:00 X:00 Y:00 P:27 SP:FB 
//C736  CLC  A:00 X:00 Y:00 P:27 SP:FB 
//C737  BCS  A:00 X:00 Y:00 P:26 SP:FB 
//C739  JMP  A:00 X:00 Y:00 P:26 SP:FB 
//C740  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C741  SEC  A:00 X:00 Y:00 P:26 SP:FB 
//C742  BCC  A:00 X:00 Y:00 P:27 SP:FB 
//C744  JMP  A:00 X:00 Y:00 P:27 SP:FB 
//C74B  NOP  A:00 X:00 Y:00 P:27 SP:FB 
//C74C  CLC  A:00 X:00 Y:00 P:27 SP:FB 
//C74D  BCC  A:00 X:00 Y:00 P:26 SP:FB 
//C753  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C754  LDA  A:00 X:00 Y:00 P:26 SP:FB 
//C756  BEQ  A:00 X:00 Y:00 P:26 SP:FB 
//C75C  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C75D  LDA  A:00 X:00 Y:00 P:26 SP:FB 
//C75F  BEQ  A:40 X:00 Y:00 P:24 SP:FB 
//C761  JMP  A:40 X:00 Y:00 P:24 SP:FB 
//C768  NOP  A:40 X:00 Y:00 P:24 SP:FB 
//C769  LDA  A:40 X:00 Y:00 P:24 SP:FB 
//C76B  BNE  A:40 X:00 Y:00 P:24 SP:FB 
//C771  NOP  A:40 X:00 Y:00 P:24 SP:FB 
//C772  LDA  A:40 X:00 Y:00 P:24 SP:FB 
//C774  BNE  A:00 X:00 Y:00 P:26 SP:FB 
//C776  JMP  A:00 X:00 Y:00 P:26 SP:FB 
//C77D  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C77E  LDA  A:00 X:00 Y:00 P:26 SP:FB 
//C780  STA  A:FF X:00 Y:00 P:A4 SP:FB 
//C782  BIT  A:FF X:00 Y:00 P:A4 SP:FB 
//C784  BVS  A:FF X:00 Y:00 P:E4 SP:FB 
//C78A  NOP  A:FF X:00 Y:00 P:E4 SP:FB 
//C78B  BIT  A:FF X:00 Y:00 P:E4 SP:FB 
//C78D  BVC  A:FF X:00 Y:00 P:E4 SP:FB 
//C78F  JMP  A:FF X:00 Y:00 P:E4 SP:FB 
//C796  NOP  A:FF X:00 Y:00 P:E4 SP:FB
//C797  LDA  A:FF X:00 Y:00 P:E4 SP:FB
//C799  STA  A:00 X:00 Y:00 P:66 SP:FB 
//C79B  BIT  A:00 X:00 Y:00 P:66 SP:FB 
//C79D  BVC  A:00 X:00 Y:00 P:26 SP:FB 
//C7A3  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C7A4  BIT  A:00 X:00 Y:00 P:26 SP:FB 
//C7A6  BVS  A:00 X:00 Y:00 P:26 SP:FB 
//C7A8  JMP  A:00 X:00 Y:00 P:26 SP:FB 
//C7AF  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C7B0  LDA  A:00 X:00 Y:00 P:26 SP:FB 
//C7B2  BPL  A:00 X:00 Y:00 P:26 SP:FB 
//C7B8  NOP  A:00 X:00 Y:00 P:26 SP:FB 
//C7B9  LDA  A:00 X:00 Y:00 P:26 SP:FB 
//C7BB  BPL  A:80 X:00 Y:00 P:A4 SP:FB 
//C7BD  JMP  A:80 X:00 Y:00 P:A4 SP:FB 
//C7D9  NOP  A:80 X:00 Y:00 P:A4 SP:FB 
//C7DA  RTS  A:80 X:00 Y:00 P:A4 SP:FB 
//C600  JSR  A:80 X:00 Y:00 P:A4 SP:FD 
//C7DB  NOP  A:80 X:00 Y:00 P:A4 SP:FB 
//C7DC  LDA  A:80 X:00 Y:00 P:A4 SP:FB 
//C7DE  STA  A:FF X:00 Y:00 P:A4 SP:FB 
//C7E0  BIT  A:FF X:00 Y:00 P:A4 SP:FB 
//C7E2  LDA  A:FF X:00 Y:00 P:E4 SP:FB 
//C7E4  SEC  A:00 X:00 Y:00 P:66 SP:FB 
//C7E5  SEI  A:00 X:00 Y:00 P:67 SP:FB 
//C7E6  SED  A:00 X:00 Y:00 P:67 SP:FB 
//C7E7  PHP  A:00 X:00 Y:00 P:6F SP:FB 
//C7E8  PLA  A:00 X:00 Y:00 P:6F SP:FA 
//C7E9  AND  A:7F X:00 Y:00 P:6D SP:FB 
//C7EB  CMP  A:6F X:00 Y:00 P:6D SP:FB 
//C7ED  BEQ  A:6F X:00 Y:00 P:6F SP:FB 
//C7F3  NOP  A:6F X:00 Y:00 P:6F SP:FB 
//C7F4  LDA  A:6F X:00 Y:00 P:6F SP:FB 
//C7F6  STA  A:40 X:00 Y:00 P:6D SP:FB 
//C7F8  BIT  A:40 X:00 Y:00 P:6D SP:FB 
//C7FA  CLD  A:40 X:00 Y:00 P:6D SP:FB 
//C7FB  LDA  A:40 X:00 Y:00 P:65 SP:FB 
//C7FD  CLC  A:10 X:00 Y:00 P:65 SP:FB 
//C7FE  PHP  A:10 X:00 Y:00 P:64 SP:FB 
//C7FF  PLA  A:10 X:00 Y:00 P:64 SP:FA 
//C800  AND  A:74 X:00 Y:00 P:64 SP:FB 
//C802  CMP  A:64 X:00 Y:00 P:64 SP:FB 
//C804  BEQ  A:64 X:00 Y:00 P:67 SP:FB 
//C80A  NOP  A:64 X:00 Y:00 P:67 SP:FB
//C80B  LDA  A:64 X:00 Y:00 P:67 SP:FB 
//C80D  STA  A:80 X:00 Y:00 P:E5 SP:FB 
//C80F  BIT  A:80 X:00 Y:00 P:E5 SP:FB 
//C811  SED  A:80 X:00 Y:00 P:A5 SP:FB 
//C812  LDA  A:80 X:00 Y:00 P:AD SP:FB 
//C814  SEC  A:00 X:00 Y:00 P:2F SP:FB 
//C815  PHP  A:00 X:00 Y:00 P:2F SP:FB 
//C816  PLA  A:00 X:00 Y:00 P:2F SP:FA 
//C817  AND  A:3F X:00 Y:00 P:2D SP:FB 
//C819  CMP  A:2F X:00 Y:00 P:2D SP:FB 
//C81B  BEQ  A:2F X:00 Y:00 P:2F SP:FB 
//C821  NOP  A:2F X:00 Y:00 P:2F SP:FB 
//C822  LDA  A:2F X:00 Y:00 P:2F SP:FB 
//C824  PHA  A:FF X:00 Y:00 P:AD SP:FB 
//C825  PLP  A:FF X:00 Y:00 P:AD SP:FA 
//C826  BNE  A:FF X:00 Y:00 P:EF SP:FB 
//C828  BPL  A:FF X:00 Y:00 P:EF SP:FB 
//C82A  BVC  A:FF X:00 Y:00 P:EF SP:FB 
//C82C  BCC  A:FF X:00 Y:00 P:EF SP:FB 
//C82E  JMP  A:FF X:00 Y:00 P:EF SP:FB 
//C835  NOP  A:FF X:00 Y:00 P:EF SP:FB 
//C836  LDA  A:FF X:00 Y:00 P:EF SP:FB 
//C838  PHA  A:04 X:00 Y:00 P:6D SP:FB 
//C839  PLP  A:04 X:00 Y:00 P:6D SP:FA 
//C83A  BEQ  A:04 X:00 Y:00 P:24 SP:FB 
//C83C  BMI  A:04 X:00 Y:00 P:24 SP:FB 
//C83E  BVS  A:04 X:00 Y:00 P:24 SP:FB 
//C840  BCS  A:04 X:00 Y:00 P:24 SP:FB 
//C842  JMP  A:04 X:00 Y:00 P:24 SP:FB 
//C849  NOP  A:04 X:00 Y:00 P:24 SP:FB 
//C84A  SED  A:04 X:00 Y:00 P:24 SP:FB 
//C84B  LDA  A:04 X:00 Y:00 P:2C SP:FB 
//C84D  STA  A:FF X:00 Y:00 P:AC SP:FB 
//C84F  BIT  A:FF X:00 Y:00 P:AC SP:FB 
//C851  CLC  A:FF X:00 Y:00 P:EC SP:FB 
//C852  LDA  A:FF X:00 Y:00 P:EC SP:FB 
//C854  PHA  A:00 X:00 Y:00 P:6E SP:FB 
//C855  LDA  A:00 X:00 Y:00 P:6E SP:FA 
//C857  PLA  A:FF X:00 Y:00 P:EC SP:FA 
//C858  BNE  A:00 X:00 Y:00 P:6E SP:FB 
//C85A  BMI  A:00 X:00 Y:00 P:6E SP:FB 
//C85C  BVC  A:00 X:00 Y:00 P:6E SP:FB 
//C85E  BCS  A:00 X:00 Y:00 P:6E SP:FB 
//C860  JMP  A:00 X:00 Y:00 P:6E SP:FB 
//C867  NOP  A:00 X:00 Y:00 P:6E SP:FB 
//C868  LDA  A:00 X:00 Y:00 P:6E SP:FB
//C86A  STA  A:00 X:00 Y:00 P:6E SP:FB
//C86C  BIT  A:00 X:00 Y:00 P:6E SP:FB 
//C86E  SEC  A:00 X:00 Y:00 P:2E SP:FB 
//C86F  LDA  A:00 X:00 Y:00 P:2F SP:FB 
//C871  PHA  A:FF X:00 Y:00 P:AD SP:FB 
//C872  LDA  A:FF X:00 Y:00 P:AD SP:FA 
//C874  PLA  A:00 X:00 Y:00 P:2F SP:FA 
//C875  BEQ  A:FF X:00 Y:00 P:AD SP:FB 
//C877  BPL  A:FF X:00 Y:00 P:AD SP:FB 
//C879  BVS  A:FF X:00 Y:00 P:AD SP:FB 
//C87B  BCC  A:FF X:00 Y:00 P:AD SP:FB 
//C87D  JMP  A:FF X:00 Y:00 P:AD SP:FB 
//C884  RTS  A:FF X:00 Y:00 P:AD SP:FB 
//C603  JSR  A:FF X:00 Y:00 P:AD SP:FD 
//C885  NOP  A:FF X:00 Y:00 P:AD SP:FB 
//C886  CLC  A:FF X:00 Y:00 P:AD SP:FB 
//C887  LDA  A:FF X:00 Y:00 P:AC SP:FB 
//C889  STA  A:FF X:00 Y:00 P:AC SP:FB 
//C88B  BIT  A:FF X:00 Y:00 P:AC SP:FB 
//C88D  LDA  A:FF X:00 Y:00 P:EC SP:FB 
//C88F  ORA  A:55 X:00 Y:00 P:6C SP:FB 
//C891  BCS  A:FF X:00 Y:00 P:EC SP:FB 
//C893  BPL  A:FF X:00 Y:00 P:EC SP:FB 
//C895  CMP  A:FF X:00 Y:00 P:EC SP:FB 
//C897  BNE  A:FF X:00 Y:00 P:6F SP:FB 
//C899  BVC  A:FF X:00 Y:00 P:6F SP:FB 
//C89B  JMP  A:FF X:00 Y:00 P:6F SP:FB 
//C8A2  NOP  A:FF X:00 Y:00 P:6F SP:FB 
//C8A3  SEC  A:FF X:00 Y:00 P:6F SP:FB 
//C8A4  CLV  A:FF X:00 Y:00 P:6F SP:FB 
//C8A5  LDA  A:FF X:00 Y:00 P:2F SP:FB 
//C8A7  ORA  A:00 X:00 Y:00 P:2F SP:FB 
//C8A9  BNE  A:00 X:00 Y:00 P:2F SP:FB 
//C8AB  BVS  A:00 X:00 Y:00 P:2F SP:FB 
//C8AD  BCC  A:00 X:00 Y:00 P:2F SP:FB 
//C8AF  BMI  A:00 X:00 Y:00 P:2F SP:FB 
//C8B1  JMP  A:00 X:00 Y:00 P:2F SP:FB 
//C8B8  NOP  A:00 X:00 Y:00 P:2F SP:FB 
//C8B9  CLC  A:00 X:00 Y:00 P:2F SP:FB 
//C8BA  BIT  A:00 X:00 Y:00 P:2E SP:FB 
//C8BC  LDA  A:00 X:00 Y:00 P:EE SP:FB 
//C8BE  AND  A:55 X:00 Y:00 P:6C SP:FB 
//C8C0  BNE  A:00 X:00 Y:00 P:6E SP:FB 
//C8C2  BVC  A:00 X:00 Y:00 P:6E SP:FB 
//C8C4  BCS  A:00 X:00 Y:00 P:6E SP:FB 
//C8C6  BMI  A:00 X:00 Y:00 P:6E SP:FB 
//C8C8  JMP  A:00 X:00 Y:00 P:6E SP:FB 
//C8CF  NOP  A:00 X:00 Y:00 P:6E SP:FB
//C8D0  SEC  A:00 X:00 Y:00 P:6E SP:FB 
//C8D1  CLV  A:00 X:00 Y:00 P:6F SP:FB 
//C8D2  LDA  A:00 X:00 Y:00 P:2F SP:FB 
//C8D4  AND  A:F8 X:00 Y:00 P:AD SP:FB 
//C8D6  BCC  A:E8 X:00 Y:00 P:AD SP:FB 
//C8D8  BPL  A:E8 X:00 Y:00 P:AD SP:FB 
//C8DA  CMP  A:E8 X:00 Y:00 P:AD SP:FB 
//C8DC  BNE  A:E8 X:00 Y:00 P:2F SP:FB 
//C8DE  BVS  A:E8 X:00 Y:00 P:2F SP:FB 
//C8E0  JMP  A:E8 X:00 Y:00 P:2F SP:FB 
//C8E7  NOP  A:E8 X:00 Y:00 P:2F SP:FB 
//C8E8  CLC  A:E8 X:00 Y:00 P:2F SP:FB 
//C8E9  BIT  A:E8 X:00 Y:00 P:2E SP:FB 
//C8EB  LDA  A:E8 X:00 Y:00 P:EC SP:FB 
//C8ED  EOR  A:5F X:00 Y:00 P:6C SP:FB 
//C8EF  BCS  A:F5 X:00 Y:00 P:EC SP:FB 
//C8F1  BPL  A:F5 X:00 Y:00 P:EC SP:FB 
//C8F3  CMP  A:F5 X:00 Y:00 P:EC SP:FB 
//C8F5  BNE  A:F5 X:00 Y:00 P:6F SP:FB 
//C8F7  BVC  A:F5 X:00 Y:00 P:6F SP:FB 
//C8F9  JMP  A:F5 X:00 Y:00 P:6F SP:FB 
//C900  NOP  A:F5 X:00 Y:00 P:6F SP:FB 
//C901  SEC  A:F5 X:00 Y:00 P:6F SP:FB 
//C902  CLV  A:F5 X:00 Y:00 P:6F SP:FB 
//C903  LDA  A:F5 X:00 Y:00 P:2F SP:FB 
//C905  EOR  A:70 X:00 Y:00 P:2D SP:FB 
//C907  BNE  A:00 X:00 Y:00 P:2F SP:FB 
//C909  BVS  A:00 X:00 Y:00 P:2F SP:FB 
//C90B  BCC  A:00 X:00 Y:00 P:2F SP:FB 
//C90D  BMI  A:00 X:00 Y:00 P:2F SP:FB 
//C90F  JMP  A:00 X:00 Y:00 P:2F SP:FB 
//C916  NOP  A:00 X:00 Y:00 P:2F SP:FB 
//C917  CLC  A:00 X:00 Y:00 P:2F SP:FB 
//C918  BIT  A:00 X:00 Y:00 P:2E SP:FB 
//C91A  LDA  A:00 X:00 Y:00 P:EE SP:FB 
//C91C  ADC  A:00 X:00 Y:00 P:6E SP:FB 
//C91E  BMI  A:69 X:00 Y:00 P:2C SP:FB 
//C920  BCS  A:69 X:00 Y:00 P:2C SP:FB 
//C922  CMP  A:69 X:00 Y:00 P:2C SP:FB 
//C924  BNE  A:69 X:00 Y:00 P:2F SP:FB 
//C926  BVS  A:69 X:00 Y:00 P:2F SP:FB 
//C928  JMP  A:69 X:00 Y:00 P:2F SP:FB 
//C92F  NOP  A:69 X:00 Y:00 P:2F SP:FB 
//C930  SEC  A:69 X:00 Y:00 P:2F SP:FB 
//C931  SED  A:69 X:00 Y:00 P:2F SP:FB 
//C932  BIT  A:69 X:00 Y:00 P:2F SP:FB 
//C934  LDA  A:69 X:00 Y:00 P:ED SP:FB 
//C936  ADC  A:01 X:00 Y:00 P:6D SP:FB 
//C938  BMI  A:6B X:00 Y:00 P:2C SP:FB 
//C93A  BCS  A:6B X:00 Y:00 P:2C SP:FB 
//C93C  CMP  A:6B X:00 Y:00 P:2C SP:FB 
//C93E  BNE  A:6B X:00 Y:00 P:2F SP:FB 
//C940  BVS  A:6B X:00 Y:00 P:2F SP:FB
//C942  JMP  A:6B X:00 Y:00 P:2F SP:FB 
//C949  NOP  A:6B X:00 Y:00 P:2F SP:FB
//C94A  CLD  A:6B X:00 Y:00 P:2F SP:FB
//C94B  SEC  A:6B X:00 Y:00 P:27 SP:FB
//C94C  CLV  A:6B X:00 Y:00 P:27 SP:FB
//C94D  LDA  A:6B X:00 Y:00 P:27 SP:FB
//C94F  ADC  A:7F X:00 Y:00 P:25 SP:FB
//C951  BPL  A:FF X:00 Y:00 P:E4 SP:FB
//C953  BCS  A:FF X:00 Y:00 P:E4 SP:FB
//C955  CMP  A:FF X:00 Y:00 P:E4 SP:FB
//C957  BNE  A:FF X:00 Y:00 P:67 SP:FB
//C959  BVC  A:FF X:00 Y:00 P:67 SP:FB
//C95B  JMP  A:FF X:00 Y:00 P:67 SP:FB
//C962  NOP  A:FF X:00 Y:00 P:67 SP:FB
//C963  CLC  A:FF X:00 Y:00 P:67 SP:FB
//C964  BIT  A:FF X:00 Y:00 P:66 SP:FB
//C966  LDA  A:FF X:00 Y:00 P:E4 SP:FB
//C968  ADC  A:7F X:00 Y:00 P:64 SP:FB
//C96A  BPL  A:FF X:00 Y:00 P:A4 SP:FB
//C96C  BCS  A:FF X:00 Y:00 P:A4 SP:FB
//C96E  CMP  A:FF X:00 Y:00 P:A4 SP:FB
//C970  BNE  A:FF X:00 Y:00 P:27 SP:FB
//C972  BVS  A:FF X:00 Y:00 P:27 SP:FB
//C974  JMP  A:FF X:00 Y:00 P:27 SP:FB
//C97B  NOP  A:FF X:00 Y:00 P:27 SP:FB
//C97C  SEC  A:FF X:00 Y:00 P:27 SP:FB
//C97D  CLV  A:FF X:00 Y:00 P:27 SP:FB
//C97E  LDA  A:FF X:00 Y:00 P:27 SP:FB
//C980  ADC  A:7F X:00 Y:00 P:25 SP:FB
//C982  BNE  A:00 X:00 Y:00 P:27 SP:FB
//C984  BMI  A:00 X:00 Y:00 P:27 SP:FB
//C986  BVS  A:00 X:00 Y:00 P:27 SP:FB
//C988  BCC  A:00 X:00 Y:00 P:27 SP:FB
//C98A  JMP  A:00 X:00 Y:00 P:27 SP:FB
//C991  NOP  A:00 X:00 Y:00 P:27 SP:FB
//C992  SEC  A:00 X:00 Y:00 P:27 SP:FB
//C993  CLV  A:00 X:00 Y:00 P:27 SP:FB
//C994  LDA  A:00 X:00 Y:00 P:27 SP:FB
//C996  BEQ  A:9F X:00 Y:00 P:A5 SP:FB
//C998  BPL  A:9F X:00 Y:00 P:A5 SP:FB
//C99A  BVS  A:9F X:00 Y:00 P:A5 SP:FB
//C99C  BCC  A:9F X:00 Y:00 P:A5 SP:FB
//C99E  JMP  A:9F X:00 Y:00 P:A5 SP:FB
//C9A5  NOP  A:9F X:00 Y:00 P:A5 SP:FB
//C9A6  CLC  A:9F X:00 Y:00 P:A5 SP:FB
//C9A7  BIT  A:9F X:00 Y:00 P:A4 SP:FB
//C9A9  LDA  A:9F X:00 Y:00 P:E4 SP:FB
//C9AB  BNE  A:00 X:00 Y:00 P:66 SP:FB
//C9AD  BMI  A:00 X:00 Y:00 P:66 SP:FB
//C9AF  BVC  A:00 X:00 Y:00 P:66 SP:FB
//C9B1  BCS  A:00 X:00 Y:00 P:66 SP:FB
//C9B3  JMP  A:00 X:00 Y:00 P:66 SP:FB
//C9BA  NOP  A:00 X:00 Y:00 P:66 SP:FB
//C9BB  BIT  A:00 X:00 Y:00 P:66 SP:FB
//C9BD  LDA  A:00 X:00 Y:00 P:E6 SP:FB
//C9BF  CMP  A:40 X:00 Y:00 P:64 SP:FB
//C9C1  BMI  A:40 X:00 Y:00 P:67 SP:FB
//C9C3  BCC  A:40 X:00 Y:00 P:67 SP:FB
//C9C5  BNE  A:40 X:00 Y:00 P:67 SP:FB
//C9C7  BVC  A:40 X:00 Y:00 P:67 SP:FB
//C9C9  JMP  A:40 X:00 Y:00 P:67 SP:FB
//C9D0  NOP  A:40 X:00 Y:00 P:67 SP:FB
//C9D1  CLV  A:40 X:00 Y:00 P:67 SP:FB
//C9D2  CMP  A:40 X:00 Y:00 P:27 SP:FB
//C9D4  BEQ  A:40 X:00 Y:00 P:25 SP:FB
//C9D6  BMI  A:40 X:00 Y:00 P:25 SP:FB
//C9D8  BCC  A:40 X:00 Y:00 P:25 SP:FB
//C9DA  BVS  A:40 X:00 Y:00 P:25 SP:FB
//C9DC  JMP  A:40 X:00 Y:00 P:25 SP:FB
//C9E3  NOP  A:40 X:00 Y:00 P:25 SP:FB
//C9E4  CMP  A:40 X:00 Y:00 P:25 SP:FB
//C9E6  BEQ  A:40 X:00 Y:00 P:A4 SP:FB
//C9E8  BPL  A:40 X:00 Y:00 P:A4 SP:FB
//C9EA  BPL  A:40 X:00 Y:00 P:A4 SP:FB
//C9EC  JMP  A:40 X:00 Y:00 P:A4 SP:FB
//C9F3  NOP  A:40 X:00 Y:00 P:A4 SP:FB
//C9F4  LDA  A:40 X:00 Y:00 P:A4 SP:FB
//C9F6  CMP  A:80 X:00 Y:00 P:A4 SP:FB
//C9F8  BEQ  A:80 X:00 Y:00 P:A5 SP:FB
//C9FA  BPL  A:80 X:00 Y:00 P:A5 SP:FB
//C9FC  BCC  A:80 X:00 Y:00 P:A5 SP:FB
//C9FE  JMP  A:80 X:00 Y:00 P:A5 SP:FB
//CA05  NOP  A:80 X:00 Y:00 P:A5 SP:FB
//CA06  CMP  A:80 X:00 Y:00 P:A5 SP:FB
//CA08  BNE  A:80 X:00 Y:00 P:27 SP:FB
//CA0A  BMI  A:80 X:00 Y:00 P:27 SP:FB
//CA0C  BCC  A:80 X:00 Y:00 P:27 SP:FB
//CA0E  JMP  A:80 X:00 Y:00 P:27 SP:FB
//CA15  NOP  A:80 X:00 Y:00 P:27 SP:FB
//CA16  CMP  A:80 X:00 Y:00 P:27 SP:FB
//CA18  BCS  A:80 X:00 Y:00 P:A4 SP:FB
//CA1A  BEQ  A:80 X:00 Y:00 P:A4 SP:FB
//CA1C  BPL  A:80 X:00 Y:00 P:A4 SP:FB
//CA1E  JMP  A:80 X:00 Y:00 P:A4 SP:FB
//CA25  NOP  A:80 X:00 Y:00 P:A4 SP:FB
//CA26  CMP  A:80 X:00 Y:00 P:A4 SP:FB
//CA28  BCC  A:80 X:00 Y:00 P:25 SP:FB
//CA2A  BEQ  A:80 X:00 Y:00 P:25 SP:FB
//CA2C  BMI  A:80 X:00 Y:00 P:25 SP:FB
//CA2E  JMP  A:80 X:00 Y:00 P:25 SP:FB
//CA35  NOP  A:80 X:00 Y:00 P:25 SP:FB
//CA36  BIT  A:80 X:00 Y:00 P:25 SP:FB
//CA38  LDY  A:80 X:00 Y:00 P:E5 SP:FB
//CA3A  CPY  A:80 X:00 Y:40 P:65 SP:FB
//CA3C  BNE  A:80 X:00 Y:40 P:67 SP:FB
//CA3E  BMI  A:80 X:00 Y:40 P:67 SP:FB
//CA40  BCC  A:80 X:00 Y:40 P:67 SP:FB
//CA42  BVC  A:80 X:00 Y:40 P:67 SP:FB
//CA44  JMP  A:80 X:00 Y:40 P:67 SP:FB
//CA4B  NOP  A:80 X:00 Y:40 P:67 SP:FB
//CA4C  CLV  A:80 X:00 Y:40 P:67 SP:FB
//CA4D  CPY  A:80 X:00 Y:40 P:27 SP:FB
//CA4F  BEQ  A:80 X:00 Y:40 P:25 SP:FB
//CA51  BMI  A:80 X:00 Y:40 P:25 SP:FB
//CA53  BCC  A:80 X:00 Y:40 P:25 SP:FB
//CA55  BVS  A:80 X:00 Y:40 P:25 SP:FB
//CA57  JMP  A:80 X:00 Y:40 P:25 SP:FB
//CA5E  NOP  A:80 X:00 Y:40 P:25 SP:FB
//CA5F  CPY  A:80 X:00 Y:40 P:25 SP:FB
//CA61  BEQ  A:80 X:00 Y:40 P:A4 SP:FB
//CA63  BPL  A:80 X:00 Y:40 P:A4 SP:FB
//CA65  BPL  A:80 X:00 Y:40 P:A4 SP:FB
//CA67  JMP  A:80 X:00 Y:40 P:A4 SP:FB
//CA6E  NOP  A:80 X:00 Y:40 P:A4 SP:FB
//CA6F  LDY  A:80 X:00 Y:40 P:A4 SP:FB
//CA71  CPY  A:80 X:00 Y:80 P:A4 SP:FB
//CA73  BEQ  A:80 X:00 Y:80 P:A5 SP:FB
//CA75  BPL  A:80 X:00 Y:80 P:A5 SP:FB
//CA77  BCC  A:80 X:00 Y:80 P:A5 SP:FB
//CA79  JMP  A:80 X:00 Y:80 P:A5 SP:FB
//CA80  NOP  A:80 X:00 Y:80 P:A5 SP:FB
//CA81  CPY  A:80 X:00 Y:80 P:A5 SP:FB
//CA83  BNE  A:80 X:00 Y:80 P:27 SP:FB
//CA85  BMI  A:80 X:00 Y:80 P:27 SP:FB
//CA87  BCC  A:80 X:00 Y:80 P:27 SP:FB
//CA89  JMP  A:80 X:00 Y:80 P:27 SP:FB
//CA90  NOP  A:80 X:00 Y:80 P:27 SP:FB
//CA91  CPY  A:80 X:00 Y:80 P:27 SP:FB
//CA93  BCS  A:80 X:00 Y:80 P:A4 SP:FB
//CA95  BEQ  A:80 X:00 Y:80 P:A4 SP:FB
//CA97  BPL  A:80 X:00 Y:80 P:A4 SP:FB
//CA99  JMP  A:80 X:00 Y:80 P:A4 SP:FB
//CAA0  NOP  A:80 X:00 Y:80 P:A4 SP:FB
//CAA1  CPY  A:80 X:00 Y:80 P:A4 SP:FB
//CAA3  BCC  A:80 X:00 Y:80 P:25 SP:FB
//CAA5  BEQ  A:80 X:00 Y:80 P:25 SP:FB
//CAA7  BMI  A:80 X:00 Y:80 P:25 SP:FB
//CAA9  JMP  A:80 X:00 Y:80 P:25 SP:FB
//CAB0  NOP  A:80 X:00 Y:80 P:25 SP:FB
//CAB1  BIT  A:80 X:00 Y:80 P:25 SP:FB
//CAB3  LDX  A:80 X:00 Y:80 P:E5 SP:FB
//CAB5  CPX  A:80 X:40 Y:80 P:65 SP:FB
//CAB7  BNE  A:80 X:40 Y:80 P:67 SP:FB
//CAB9  BMI  A:80 X:40 Y:80 P:67 SP:FB
//CABB  BCC  A:80 X:40 Y:80 P:67 SP:FB
//CABD  BVC  A:80 X:40 Y:80 P:67 SP:FB
//CABF  JMP  A:80 X:40 Y:80 P:67 SP:FB
//CAC6  NOP  A:80 X:40 Y:80 P:67 SP:FB
//CAC7  CLV  A:80 X:40 Y:80 P:67 SP:FB
//CAC8  CPX  A:80 X:40 Y:80 P:27 SP:FB
//CACA  BEQ  A:80 X:40 Y:80 P:25 SP:FB
//CACC  BMI  A:80 X:40 Y:80 P:25 SP:FB
//CACE  BCC  A:80 X:40 Y:80 P:25 SP:FB
//CAD0  BVS  A:80 X:40 Y:80 P:25 SP:FB
//CAD2  JMP  A:80 X:40 Y:80 P:25 SP:FB
//CAD9  NOP  A:80 X:40 Y:80 P:25 SP:FB
//CADA  CPX  A:80 X:40 Y:80 P:25 SP:FB
//CADC  BEQ  A:80 X:40 Y:80 P:A4 SP:FB
//CADE  BPL  A:80 X:40 Y:80 P:A4 SP:FB
//CAE0  BPL  A:80 X:40 Y:80 P:A4 SP:FB
//CAE2  JMP  A:80 X:40 Y:80 P:A4 SP:FB
//CAE9  NOP  A:80 X:40 Y:80 P:A4 SP:FB
//CAEA  LDX  A:80 X:40 Y:80 P:A4 SP:FB
//CAEC  CPX  A:80 X:80 Y:80 P:A4 SP:FB
//CAEE  BEQ  A:80 X:80 Y:80 P:A5 SP:FB
//CAF0  BPL  A:80 X:80 Y:80 P:A5 SP:FB
//CAF2  BCC  A:80 X:80 Y:80 P:A5 SP:FB
//CAF4  JMP  A:80 X:80 Y:80 P:A5 SP:FB
//CAFB  NOP  A:80 X:80 Y:80 P:A5 SP:FB
//CAFC  CPX  A:80 X:80 Y:80 P:A5 SP:FB
//CAFE  BNE  A:80 X:80 Y:80 P:27 SP:FB





//CB00  BMI  A:80 X:80 Y:80 P:27 SP:FB
//CB02  BCC  A:80 X:80 Y:80 P:27 SP:FB
//CB04  JMP  A:80 X:80 Y:80 P:27 SP:FB
//CB0B  NOP  A:80 X:80 Y:80 P:27 SP:FB
//CB0C  CPX  A:80 X:80 Y:80 P:27 SP:FB
//CB0E  BCS  A:80 X:80 Y:80 P:A4 SP:FB
//CB10  BEQ  A:80 X:80 Y:80 P:A4 SP:FB
//CB12  BPL  A:80 X:80 Y:80 P:A4 SP:FB
//CB14  JMP  A:80 X:80 Y:80 P:A4 SP:FB
//CB1B  NOP  A:80 X:80 Y:80 P:A4 SP:FB
//CB1C  CPX  A:80 X:80 Y:80 P:A4 SP:FB
//CB1E  BCC  A:80 X:80 Y:80 P:25 SP:FB
//CB20  BEQ  A:80 X:80 Y:80 P:25 SP:FB
//CB22  BMI  A:80 X:80 Y:80 P:25 SP:FB
//CB24  JMP  A:80 X:80 Y:80 P:25 SP:FB
//CB2B  NOP  A:80 X:80 Y:80 P:25 SP:FB
//CB2C  SEC  A:80 X:80 Y:80 P:25 SP:FB
//CB2D  CLV  A:80 X:80 Y:80 P:25 SP:FB
//CB2E  LDX  A:80 X:80 Y:80 P:25 SP:FB
//CB30  BEQ  A:80 X:9F Y:80 P:A5 SP:FB
//CB32  BPL  A:80 X:9F Y:80 P:A5 SP:FB
//CB34  BVS  A:80 X:9F Y:80 P:A5 SP:FB
//CB36  BCC  A:80 X:9F Y:80 P:A5 SP:FB
//CB38  JMP  A:80 X:9F Y:80 P:A5 SP:FB
//CB3F  NOP  A:80 X:9F Y:80 P:A5 SP:FB
//CB40  CLC  A:80 X:9F Y:80 P:A5 SP:FB
//CB41  BIT  A:80 X:9F Y:80 P:A4 SP:FB
//CB43  LDX  A:80 X:9F Y:80 P:E4 SP:FB
//CB45  BNE  A:80 X:00 Y:80 P:66 SP:FB
//CB47  BMI  A:80 X:00 Y:80 P:66 SP:FB
//CB49  BVC  A:80 X:00 Y:80 P:66 SP:FB
//CB4B  BCS  A:80 X:00 Y:80 P:66 SP:FB
//CB4D  JMP  A:80 X:00 Y:80 P:66 SP:FB
//CB54  NOP  A:80 X:00 Y:80 P:66 SP:FB
//CB55  SEC  A:80 X:00 Y:80 P:66 SP:FB
//CB56  CLV  A:80 X:00 Y:80 P:67 SP:FB
//CB57  LDY  A:80 X:00 Y:80 P:27 SP:FB
//CB59  BEQ  A:80 X:00 Y:9F P:A5 SP:FB
//CB5B  BPL  A:80 X:00 Y:9F P:A5 SP:FB
//CB5D  BVS  A:80 X:00 Y:9F P:A5 SP:FB
//CB5F  BCC  A:80 X:00 Y:9F P:A5 SP:FB
//CB61  JMP  A:80 X:00 Y:9F P:A5 SP:FB
//CB68  NOP  A:80 X:00 Y:9F P:A5 SP:FB
//CB69  CLC  A:80 X:00 Y:9F P:A5 SP:FB
//CB6A  BIT  A:80 X:00 Y:9F P:A4 SP:FB
//CB6C  LDY  A:80 X:00 Y:9F P:E4 SP:FB
//CB6E  BNE  A:80 X:00 Y:00 P:66 SP:FB
//CB70  BMI  A:80 X:00 Y:00 P:66 SP:FB
//CB72  BVC  A:80 X:00 Y:00 P:66 SP:FB
//CB74  BCS  A:80 X:00 Y:00 P:66 SP:FB
//CB76  JMP  A:80 X:00 Y:00 P:66 SP:FB
//CB7D  NOP  A:80 X:00 Y:00 P:66 SP:FB
//CB7E  LDA  A:80 X:00 Y:00 P:66 SP:FB
//CB80  LDX  A:55 X:00 Y:00 P:64 SP:FB
//CB82  LDY  A:55 X:AA Y:00 P:E4 SP:FB
//CB84  CMP  A:55 X:AA Y:33 P:64 SP:FB
//CB86  BNE  A:55 X:AA Y:33 P:67 SP:FB
//CB88  CPX  A:55 X:AA Y:33 P:67 SP:FB
//CB8A  BNE  A:55 X:AA Y:33 P:67 SP:FB
//CB8C  CPY  A:55 X:AA Y:33 P:67 SP:FB
//CB8E  BNE  A:55 X:AA Y:33 P:67 SP:FB
//CB90  CMP  A:55 X:AA Y:33 P:67 SP:FB
//CB92  BNE  A:55 X:AA Y:33 P:67 SP:FB
//CB94  CPX  A:55 X:AA Y:33 P:67 SP:FB
//CB96  BNE  A:55 X:AA Y:33 P:67 SP:FB
//CB98  CPY  A:55 X:AA Y:33 P:67 SP:FB
//CB9A  BNE  A:55 X:AA Y:33 P:67 SP:FB
//CB9C  CMP  A:55 X:AA Y:33 P:67 SP:FB
//CB9E  BEQ  A:55 X:AA Y:33 P:E4 SP:FB
//CBA0  CPX  A:55 X:AA Y:33 P:E4 SP:FB
//CBA2  BEQ  A:55 X:AA Y:33 P:E4 SP:FB
//CBA4  CPY  A:55 X:AA Y:33 P:E4 SP:FB
//CBA6  BEQ  A:55 X:AA Y:33 P:E4 SP:FB
//CBA8  JMP  A:55 X:AA Y:33 P:E4 SP:FB
//CBAF  LDY  A:55 X:AA Y:33 P:E4 SP:FB
//CBB1  JSR  A:55 X:AA Y:71 P:64 SP:FB
//F931  BIT  A:55 X:AA Y:71 P:64 SP:F9
//F933  LDA  A:55 X:AA Y:71 P:E4 SP:F9
//F935  SEC  A:40 X:AA Y:71 P:64 SP:F9
//F936  RTS  A:40 X:AA Y:71 P:65 SP:F9
//CBB4  SBC  A:40 X:AA Y:71 P:65 SP:FB 
//CBB6  JSR  A:00 X:AA Y:71 P:27 SP:FB 
//F937  BMI  A:00 X:AA Y:71 P:27 SP:F9 
//F939  BCC  A:00 X:AA Y:71 P:27 SP:F9 
//F93B  BNE  A:00 X:AA Y:71 P:27 SP:F9 
//F93D  BVS  A:00 X:AA Y:71 P:27 SP:F9 
//F93F  CMP  A:00 X:AA Y:71 P:27 SP:F9 
//F941  BNE  A:00 X:AA Y:71 P:27 SP:F9 
//F943  RTS  A:00 X:AA Y:71 P:27 SP:F9 
//CBB9  INY  A:00 X:AA Y:71 P:27 SP:FB 
//CBBA  JSR  A:00 X:AA Y:72 P:25 SP:FB 
//F947  CLV  A:00 X:AA Y:72 P:25 SP:F9 
//F948  SEC  A:00 X:AA Y:72 P:25 SP:F9 
//F949  LDA  A:00 X:AA Y:72 P:25 SP:F9 
//F94B  RTS  A:40 X:AA Y:72 P:25 SP:F9 
//CBBD  SBC  A:40 X:AA Y:72 P:25 SP:FB 
//CBBF  JSR  A:01 X:AA Y:72 P:25 SP:FB 
//F94C  BEQ  A:01 X:AA Y:72 P:25 SP:F9 
//F94E  BMI  A:01 X:AA Y:72 P:25 SP:F9 
//F950  BCC  A:01 X:AA Y:72 P:25 SP:F9 
//F952  BVS  A:01 X:AA Y:72 P:25 SP:F9 
//F954  CMP  A:01 X:AA Y:72 P:25 SP:F9 
//F956  BNE  A:01 X:AA Y:72 P:27 SP:F9 
//F958  RTS  A:01 X:AA Y:72 P:27 SP:F9 
//CBC2  INY  A:01 X:AA Y:72 P:27 SP:FB 
//CBC3  JSR  A:01 X:AA Y:73 P:25 SP:FB 
//F95C  LDA  A:01 X:AA Y:73 P:25 SP:F9 
//F95E  SEC  A:40 X:AA Y:73 P:25 SP:F9 
//F95F  BIT  A:40 X:AA Y:73 P:25 SP:F9 
//F961  RTS  A:40 X:AA Y:73 P:E5 SP:F9 
//CBC6  SBC  A:40 X:AA Y:73 P:E5 SP:FB 
//CBC8  JSR  A:FF X:AA Y:73 P:A4 SP:FB 
//F962  BCS  A:FF X:AA Y:73 P:A4 SP:F9 
//F964  BEQ  A:FF X:AA Y:73 P:A4 SP:F9 
//F966  BPL  A:FF X:AA Y:73 P:A4 SP:F9 
//F968  BVS  A:FF X:AA Y:73 P:A4 SP:F9 
//F96A  CMP  A:FF X:AA Y:73 P:A4 SP:F9
//F96C  BNE  A:FF X:AA Y:73 P:27 SP:F9
//F96E  RTS  A:FF X:AA Y:73 P:27 SP:F9 
//CBCB  INY  A:FF X:AA Y:73 P:27 SP:FB 
//CBCC  JSR  A:FF X:AA Y:74 P:25 SP:FB 
//F972  CLC  A:FF X:AA Y:74 P:25 SP:F9 
//F973  LDA  A:FF X:AA Y:74 P:24 SP:F9 
//F975  RTS  A:80 X:AA Y:74 P:A4 SP:F9 
//CBCF  SBC  A:80 X:AA Y:74 P:A4 SP:FB 
//CBD1  JSR  A:7F X:AA Y:74 P:65 SP:FB 
//F976  BCC  A:7F X:AA Y:74 P:65 SP:F9 
//F978  CMP  A:7F X:AA Y:74 P:65 SP:F9 
//F97A  BNE  A:7F X:AA Y:74 P:67 SP:F9 
//F97C  RTS  A:7F X:AA Y:74 P:67 SP:F9
//CBD4  INY  A:7F X:AA Y:74 P:67 SP:FB
//F983  RTS  A:81 X:AA Y:75 P:E5 SP:F9
//CBD5  JSR  A:7F X:AA Y:75 P:65 SP:FB
//CBDA  JSR  A:02 X:AA Y:75 P:65 SP:FB
//F980  SEC  A:7F X:AA Y:75 P:65 SP:F9
//F986  BCC  A:02 X:AA Y:75 P:65 SP:F9
//F981  LDA  A:7F X:AA Y:75 P:65 SP:F9
//F98A  BNE  A:02 X:AA Y:75 P:67 SP:F9
//CBD8  SBC  A:81 X:AA Y:75 P:E5 SP:FB
//CBDD  RTS  A:02 X:AA Y:75 P:67 SP:FB
//F984  BVC  A:02 X:AA Y:75 P:65 SP:F9
//CBDE  NOP  A:02 X:AA Y:75 P:67 SP:FB
//F988  CMP  A:02 X:AA Y:75 P:65 SP:F9
//CBE1  STA  A:FF X:AA Y:75 P:E5 SP:FB
//F98C  RTS  A:02 X:AA Y:75 P:67 SP:F9
//CBE5  LDX  A:44 X:AA Y:75 P:65 SP:FB
//C606  JSR  A:02 X:AA Y:75 P:67 SP:FD
//CBE9  INX  A:44 X:55 Y:66 P:65 SP:FB
//CBDF  LDA  A:02 X:AA Y:75 P:67 SP:FB
//CBEB  CPX  A:44 X:56 Y:65 P:65 SP:FB
//CBE3  LDA  A:FF X:AA Y:75 P:E5 SP:FB
//CBEF  CPY  A:44 X:56 Y:65 P:67 SP:FB
//CBE7  LDY  A:44 X:55 Y:75 P:65 SP:FB
//CBF3  INX  A:44 X:56 Y:65 P:67 SP:FB
//CBEA  DEY  A:44 X:56 Y:66 P:65 SP:FB
//CBF5  DEY  A:44 X:58 Y:65 P:65 SP:FB
//CBED  BNE  A:44 X:56 Y:65 P:67 SP:FB
//CBF7  CPX  A:44 X:58 Y:63 P:65 SP:FB
//CBF1  BNE  A:44 X:56 Y:65 P:67 SP:FB
//CBFB  CPY  A:44 X:58 Y:63 P:67 SP:FB
//CBF4  INX  A:44 X:57 Y:65 P:65 SP:FB
//CBFF  DEX  A:44 X:58 Y:63 P:67 SP:FB
//CBF6  DEY  A:44 X:58 Y:64 P:65 SP:FB
//CC01  CPX  A:44 X:57 Y:64 P:65 SP:FB
//CBF9  BNE  A:44 X:58 Y:63 P:67 SP:FB
//CC05  CPY  A:44 X:57 Y:64 P:67 SP:FB
//CBFD  BNE  A:44 X:58 Y:63 P:67 SP:FB
//CC09  CMP  A:44 X:57 Y:64 P:67 SP:FB
//CC00  INY  A:44 X:57 Y:63 P:65 SP:FB
//CC0D  JMP  A:44 X:57 Y:64 P:67 SP:FB
//CC03  BNE  A:44 X:57 Y:64 P:67 SP:FB
//CC15  SEC  A:44 X:57 Y:64 P:67 SP:FB
//CC07  BNE  A:44 X:57 Y:64 P:67 SP:FB
//CC18  LDA  A:44 X:69 Y:64 P:65 SP:FB
//CC0B  BNE  A:44 X:57 Y:64 P:67 SP:FB
//CC1C  LDY  A:96 X:69 Y:64 P:E5 SP:FB
//CC14  NOP  A:44 X:57 Y:64 P:67 SP:FB
//CC1F  BNE  A:96 X:69 Y:00 P:67 SP:FB
//CC16  LDX  A:44 X:57 Y:64 P:67 SP:FB
//CC23  BCC  A:96 X:69 Y:00 P:67 SP:FB
//CC1A  BIT  A:96 X:69 Y:64 P:E5 SP:FB
//CC27  CPY  A:96 X:69 Y:00 P:67 SP:FB
//CC1E  INY  A:96 X:69 Y:FF P:E5 SP:FB
//CC2B  INY  A:96 X:69 Y:00 P:67 SP:FB
//CC21  BMI  A:96 X:69 Y:00 P:67 SP:FB
//CC2E  BMI  A:96 X:69 Y:01 P:65 SP:FB
//CC25  BVC  A:96 X:69 Y:00 P:67 SP:FB
//CC32  BVC  A:96 X:69 Y:01 P:65 SP:FB
//CC29  BNE  A:96 X:69 Y:00 P:67 SP:FB
//CC35  CLV  A:96 X:69 Y:01 P:64 SP:FB
//CC2C  BEQ  A:96 X:69 Y:01 P:65 SP:FB
//CC38  DEY  A:96 X:69 Y:00 P:26 SP:FB
//CC30  BCC  A:96 X:69 Y:01 P:65 SP:FB
//CC3B  BPL  A:96 X:69 Y:FF P:A4 SP:FB
//CC34  CLC  A:96 X:69 Y:01 P:65 SP:FB
//CC3F  BVS  A:96 X:69 Y:FF P:A4 SP:FB
//CC36  LDY  A:96 X:69 Y:01 P:24 SP:FB
//CC43  BNE  A:96 X:69 Y:FF P:27 SP:FB
//CC39  BEQ  A:96 X:69 Y:FF P:A4 SP:FB
//CC46  DEY  A:96 X:69 Y:FF P:26 SP:FB
//CC3D  BCS  A:96 X:69 Y:FF P:A4 SP:FB
//CC49  BPL  A:96 X:69 Y:FE P:A4 SP:FB
//CC41  CPY  A:96 X:69 Y:FF P:A4 SP:FB
//CC4D  BVS  A:96 X:69 Y:FE P:A4 SP:FB
//CC45  CLC  A:96 X:69 Y:FF P:27 SP:FB
//CC51  BNE  A:96 X:69 Y:FE P:27 SP:FB
//CC47  BEQ  A:96 X:69 Y:FE P:A4 SP:FB
//CC55  BNE  A:96 X:69 Y:FE P:27 SP:FB
//CC4B  BCS  A:96 X:69 Y:FE P:A4 SP:FB
//CC59  BNE  A:96 X:69 Y:FE P:27 SP:FB
//CC4F  CPY  A:96 X:69 Y:FE P:A4 SP:FB
//CC62  NOP  A:96 X:69 Y:FE P:27 SP:FB
//CC53  CMP  A:96 X:69 Y:FE P:27 SP:FB
//CC64  LDY  A:96 X:69 Y:FE P:27 SP:FB
//CC57  CPX  A:96 X:69 Y:FE P:27 SP:FB
//CC68  BIT  A:96 X:69 Y:69 P:A5 SP:FB
//CC5B  JMP  A:96 X:69 Y:FE P:27 SP:FB
//CC6C  INX  A:96 X:FF Y:69 P:E5 SP:FB
//CC63  SEC  A:96 X:69 Y:FE P:27 SP:FB
//CC6F  BMI  A:96 X:00 Y:69 P:67 SP:FB
//CC66  LDA  A:96 X:69 Y:69 P:25 SP:FB
//CC73  BVC  A:96 X:00 Y:69 P:67 SP:FB
//CC6A  LDX  A:96 X:69 Y:69 P:E5 SP:FB
//CC77  BNE  A:96 X:00 Y:69 P:67 SP:FB
//CC6D  BNE  A:96 X:00 Y:69 P:67 SP:FB
//CC7A  BEQ  A:96 X:01 Y:69 P:65 SP:FB
//CC71  BCC  A:96 X:00 Y:69 P:67 SP:FB
//CC7E  BCC  A:96 X:01 Y:69 P:65 SP:FB
//CC75  CPX  A:96 X:00 Y:69 P:67 SP:FB
//CC82  CLC  A:96 X:01 Y:69 P:65 SP:FB
//CC79  INX  A:96 X:00 Y:69 P:67 SP:FB
//CC84  LDX  A:96 X:01 Y:69 P:24 SP:FB
//CC7C  BMI  A:96 X:01 Y:69 P:65 SP:FB
//CC87  BEQ  A:96 X:FF Y:69 P:A4 SP:FB
//CC80  BVC  A:96 X:01 Y:69 P:65 SP:FB
//CC8B  BCS  A:96 X:FF Y:69 P:A4 SP:FB
//CC83  CLV  A:96 X:01 Y:69 P:64 SP:FB
//CC8F  CPX  A:96 X:FF Y:69 P:A4 SP:FB
//CC86  DEX  A:96 X:00 Y:69 P:26 SP:FB
//CC93  CLC  A:96 X:FF Y:69 P:27 SP:FB
//CC89  BPL  A:96 X:FF Y:69 P:A4 SP:FB
//CC95  BEQ  A:96 X:FE Y:69 P:A4 SP:FB
//CC8D  BVS  A:96 X:FF Y:69 P:A4 SP:FB
//CC99  BCS  A:96 X:FE Y:69 P:A4 SP:FB
//CC91  BNE  A:96 X:FF Y:69 P:27 SP:FB
//CC9D  CPX  A:96 X:FE Y:69 P:A4 SP:FB
//CC94  DEX  A:96 X:FF Y:69 P:26 SP:FB
//CCA1  CMP  A:96 X:FE Y:69 P:27 SP:FB
//CC97  BPL  A:96 X:FE Y:69 P:A4 SP:FB
//CCA5  CPY  A:96 X:FE Y:69 P:27 SP:FB
//CC9B  BVS  A:96 X:FE Y:69 P:A4 SP:FB
//CCA9  JMP  A:96 X:FE Y:69 P:27 SP:FB
//CC9F  BNE  A:96 X:FE Y:69 P:27 SP:FB
//CCB1  LDA  A:96 X:FE Y:69 P:27 SP:FB
//CCA3  BNE  A:96 X:FE Y:69 P:27 SP:FB
//CCB5  LDY  A:85 X:34 Y:69 P:25 SP:FB
//CCA7  BNE  A:96 X:FE Y:69 P:27 SP:FB
//CCB8  BIT  A:85 X:34 Y:99 P:A4 SP:FB
//CCB0  NOP  A:96 X:FE Y:69 P:27 SP:FB
//CCBB  BEQ  A:85 X:34 Y:85 P:E4 SP:FB
//CCB3  LDX  A:85 X:FE Y:69 P:A5 SP:FB
//CCBF  BVC  A:85 X:34 Y:85 P:E4 SP:FB
//CCB7  CLC  A:85 X:34 Y:99 P:A5 SP:FB
//CCC3  CMP  A:85 X:34 Y:85 P:E4 SP:FB
//CCBA  TAY  A:85 X:34 Y:99 P:E4 SP:FB
//CCC7  CPX  A:85 X:34 Y:85 P:67 SP:FB
//CCBD  BCS  A:85 X:34 Y:85 P:E4 SP:FB
//CCCB  CPY  A:85 X:34 Y:85 P:67 SP:FB
//CCC1  BPL  A:85 X:34 Y:85 P:E4 SP:FB
//CCCF  LDA  A:85 X:34 Y:85 P:67 SP:FB
//CCC5  BNE  A:85 X:34 Y:85 P:67 SP:FB
//CCD2  CLV  A:00 X:34 Y:85 P:67 SP:FB
//CCC9  BNE  A:85 X:34 Y:85 P:67 SP:FB
//CCD4  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CCCD  BNE  A:85 X:34 Y:85 P:67 SP:FB
//CCD8  BVS  A:00 X:34 Y:00 P:27 SP:FB
//CCD1  SEC  A:00 X:34 Y:85 P:67 SP:FB
//CCDC  CMP  A:00 X:34 Y:00 P:27 SP:FB
//CCD3  TAY  A:00 X:34 Y:85 P:27 SP:FB
//CCE0  CPX  A:00 X:34 Y:00 P:27 SP:FB
//CCD6  BCC  A:00 X:34 Y:00 P:27 SP:FB
//CCE4  CPY  A:00 X:34 Y:00 P:27 SP:FB
//CCDA  BMI  A:00 X:34 Y:00 P:27 SP:FB
//CCE8  JMP  A:00 X:34 Y:00 P:27 SP:FB
//CCDE  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CCF0  LDA  A:00 X:34 Y:00 P:27 SP:FB
//CCE2  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CCF4  LDY  A:85 X:34 Y:00 P:25 SP:FB
//CCE6  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CCF7  BIT  A:85 X:34 Y:99 P:A4 SP:FB
//CCEF  NOP  A:00 X:34 Y:00 P:27 SP:FB
//CCFA  BEQ  A:85 X:85 Y:99 P:E4 SP:FB
//CCF2  LDX  A:85 X:34 Y:00 P:A5 SP:FB
//CCFE  BVC  A:85 X:85 Y:99 P:E4 SP:FB
//CCF6  CLC  A:85 X:34 Y:99 P:A5 SP:FB
//CD02  CMP  A:85 X:85 Y:99 P:E4 SP:FB
//CCF9  TAX  A:85 X:34 Y:99 P:E4 SP:FB
//CD06  CPX  A:85 X:85 Y:99 P:67 SP:FB
//CCFC  BCS  A:85 X:85 Y:99 P:E4 SP:FB
//CD0A  CPY  A:85 X:85 Y:99 P:67 SP:FB
//CD00  BPL  A:85 X:85 Y:99 P:E4 SP:FB
//CD0E  LDA  A:85 X:85 Y:99 P:67 SP:FB
//CD04  BNE  A:85 X:85 Y:99 P:67 SP:FB
//CD11  CLV  A:00 X:85 Y:99 P:67 SP:FB
//CD08  BNE  A:85 X:85 Y:99 P:67 SP:FB
//CD13  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CD0C  BNE  A:85 X:85 Y:99 P:67 SP:FB
//CD17  BVS  A:00 X:00 Y:99 P:27 SP:FB
//CD10  SEC  A:00 X:85 Y:99 P:67 SP:FB
//CD1B  CMP  A:00 X:00 Y:99 P:27 SP:FB
//CD12  TAX  A:00 X:85 Y:99 P:27 SP:FB
//CD1F  CPX  A:00 X:00 Y:99 P:27 SP:FB
//CD15  BCC  A:00 X:00 Y:99 P:27 SP:FB
//CD23  CPY  A:00 X:00 Y:99 P:27 SP:FB
//CD19  BMI  A:00 X:00 Y:99 P:27 SP:FB
//CD27  JMP  A:00 X:00 Y:99 P:27 SP:FB
//CD1D  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CD2F  LDA  A:00 X:00 Y:99 P:27 SP:FB
//CD21  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CD33  LDY  A:85 X:34 Y:99 P:25 SP:FB
//CD25  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CD36  BIT  A:85 X:34 Y:99 P:A4 SP:FB
//CD2E  NOP  A:00 X:00 Y:99 P:27 SP:FB
//CD39  BEQ  A:99 X:34 Y:99 P:E4 SP:FB
//CD31  LDX  A:85 X:00 Y:99 P:A5 SP:FB
//CD3D  BVC  A:99 X:34 Y:99 P:E4 SP:FB
//CD35  CLC  A:85 X:34 Y:99 P:A5 SP:FB
//CD41  CMP  A:99 X:34 Y:99 P:E4 SP:FB
//CD38  TYA  A:85 X:34 Y:99 P:E4 SP:FB
//CD45  CPX  A:99 X:34 Y:99 P:67 SP:FB
//CD3B  BCS  A:99 X:34 Y:99 P:E4 SP:FB
//CD49  CPY  A:99 X:34 Y:99 P:67 SP:FB
//CD3F  BPL  A:99 X:34 Y:99 P:E4 SP:FB
//CD4D  LDY  A:99 X:34 Y:99 P:67 SP:FB
//CD43  BNE  A:99 X:34 Y:99 P:67 SP:FB
//CD50  CLV  A:99 X:34 Y:00 P:67 SP:FB
//CD47  BNE  A:99 X:34 Y:99 P:67 SP:FB
//CD52  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CD4B  BNE  A:99 X:34 Y:99 P:67 SP:FB
//CD56  BVS  A:00 X:34 Y:00 P:27 SP:FB
//CD4F  SEC  A:99 X:34 Y:00 P:67 SP:FB
//CD5A  CMP  A:00 X:34 Y:00 P:27 SP:FB
//CD51  TYA  A:99 X:34 Y:00 P:27 SP:FB
//CD5E  CPX  A:00 X:34 Y:00 P:27 SP:FB
//CD54  BCC  A:00 X:34 Y:00 P:27 SP:FB
//CD62  CPY  A:00 X:34 Y:00 P:27 SP:FB
//CD58  BMI  A:00 X:34 Y:00 P:27 SP:FB
//CD66  JMP  A:00 X:34 Y:00 P:27 SP:FB
//CD5C  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CD6E  LDA  A:00 X:34 Y:00 P:27 SP:FB
//CD60  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CD72  LDY  A:85 X:34 Y:00 P:25 SP:FB
//CD64  BNE  A:00 X:34 Y:00 P:27 SP:FB
//CD75  BIT  A:85 X:34 Y:99 P:A4 SP:FB
//CD6D  NOP  A:00 X:34 Y:00 P:27 SP:FB
//CD78  BEQ  A:34 X:34 Y:99 P:64 SP:FB
//CD70  LDX  A:85 X:34 Y:00 P:A5 SP:FB
//CD7C  BVC  A:34 X:34 Y:99 P:64 SP:FB
//CD74  CLC  A:85 X:34 Y:99 P:A5 SP:FB
//CD80  CMP  A:34 X:34 Y:99 P:64 SP:FB
//CD77  TXA  A:85 X:34 Y:99 P:E4 SP:FB
//CD84  CPX  A:34 X:34 Y:99 P:67 SP:FB
//CD7A  BCS  A:34 X:34 Y:99 P:64 SP:FB
//CD88  CPY  A:34 X:34 Y:99 P:67 SP:FB
//CD7E  BMI  A:34 X:34 Y:99 P:64 SP:FB
//CD8C  LDX  A:34 X:34 Y:99 P:67 SP:FB
//CD82  BNE  A:34 X:34 Y:99 P:67 SP:FB
//CD8F  CLV  A:34 X:00 Y:99 P:67 SP:FB
//CD86  BNE  A:34 X:34 Y:99 P:67 SP:FB
//CD91  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CD8A  BNE  A:34 X:34 Y:99 P:67 SP:FB
//CD95  BVS  A:00 X:00 Y:99 P:27 SP:FB
//CD8E  SEC  A:34 X:00 Y:99 P:67 SP:FB
//CD99  CMP  A:00 X:00 Y:99 P:27 SP:FB
//CD90  TXA  A:34 X:00 Y:99 P:27 SP:FB
//CD9D  CPX  A:00 X:00 Y:99 P:27 SP:FB
//CD93  BCC  A:00 X:00 Y:99 P:27 SP:FB
//CDA1  CPY  A:00 X:00 Y:99 P:27 SP:FB
//CD97  BMI  A:00 X:00 Y:99 P:27 SP:FB
//CDA5  JMP  A:00 X:00 Y:99 P:27 SP:FB
//CD9B  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CDAD  TSX  A:00 X:00 Y:99 P:27 SP:FB
//CD9F  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CDB1  LDY  A:00 X:FB Y:99 P:A5 SP:FB
//CDA3  BNE  A:00 X:00 Y:99 P:27 SP:FB
//CDB5  LDA  A:00 X:69 Y:33 P:25 SP:FB
//CDAC  NOP  A:00 X:00 Y:99 P:27 SP:FB
//CDB8  BIT  A:84 X:69 Y:33 P:A4 SP:FB
//CDAE  STX  A:00 X:FB Y:99 P:A5 SP:FB
//CDBB  BEQ  A:84 X:69 Y:33 P:E4 SP:69
//CDB3  LDX  A:00 X:FB Y:33 P:25 SP:FB
//CDBF  BCS  A:84 X:69 Y:33 P:E4 SP:69
//CDB7  CLC  A:84 X:69 Y:33 P:A5 SP:FB
//CDC3  CMP  A:84 X:69 Y:33 P:E4 SP:69
//CDBA  TXS  A:84 X:69 Y:33 P:E4 SP:FB
//CDC7  CPX  A:84 X:69 Y:33 P:67 SP:69
//CDBD  BPL  A:84 X:69 Y:33 P:E4 SP:69
//CDCB  CPY  A:84 X:69 Y:33 P:67 SP:69
//CDC1  BVC  A:84 X:69 Y:33 P:E4 SP:69
//CDCF  LDY  A:84 X:69 Y:33 P:67 SP:69
//CDC5  BNE  A:84 X:69 Y:33 P:67 SP:69
//CDD3  SEC  A:04 X:69 Y:01 P:65 SP:69
//CDC9  BNE  A:84 X:69 Y:33 P:67 SP:69
//CDD5  LDX  A:04 X:69 Y:01 P:25 SP:69
//CDCD  BNE  A:84 X:69 Y:33 P:67 SP:69
//CDD8  BEQ  A:04 X:69 Y:01 P:25 SP:69
//CDD1  LDA  A:84 X:69 Y:01 P:65 SP:69
//CDDC  BCC  A:04 X:69 Y:01 P:25 SP:69
//CDD4  CLV  A:04 X:69 Y:01 P:65 SP:69
//CDE0  CPX  A:04 X:69 Y:01 P:25 SP:69
//CDD7  TSX  A:04 X:00 Y:01 P:27 SP:69
//CDE4  CMP  A:04 X:69 Y:01 P:27 SP:69
//CDDA  BMI  A:04 X:69 Y:01 P:25 SP:69
//CDE8  CPY  A:04 X:69 Y:01 P:27 SP:69
//CDDE  BVS  A:04 X:69 Y:01 P:25 SP:69
//CDEC  JMP  A:04 X:69 Y:01 P:27 SP:69
//CDE2  BNE  A:04 X:69 Y:01 P:27 SP:69
//CDF6  TXS  A:04 X:FB Y:01 P:A5 SP:69
//CDE6  BNE  A:04 X:69 Y:01 P:27 SP:69
//C609  JSR  A:04 X:FB Y:01 P:A5 SP:FD
//CDEA  BNE  A:04 X:69 Y:01 P:27 SP:69
//CDFA  STA  A:FF X:FB Y:01 P:A5 SP:FB
//CDF3  LDX  A:04 X:69 Y:01 P:27 SP:69
//CDFD  STX  A:FF X:FB Y:01 P:A5 SP:FB
//CDF7  RTS  A:04 X:FB Y:01 P:A5 SP:FB
//CE01  LDX  A:FF X:FB Y:01 P:A5 SP:FB
//CDF8  LDA  A:04 X:FB Y:01 P:A5 SP:FB
//CDFC  TSX  A:FF X:FB Y:01 P:A5 SP:FB
//CE00  NOP  A:FF X:FB Y:01 P:A5 SP:FB