#include "cpu.h"
#include <string.h>

#define NO_DECIMAL_ADC_SUPPORT
#define NO_DECIMAL_SBC_SUPPORT


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
    bool dma_transfer_initiated = CPUBusWrite(cpu->cpu_bus, address, data);
    if (dma_transfer_initiated) {
        cpu->dma_transfer = true;
        cpu->dma_aligned = false;
        cpu->dma_address = (data << 8);
        cpu->oam_address = 0;
        cpu->oam_data = 0;
    }
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

    SetNegativeFlagValue(cpu, (temp & 0x80));
    SetOverflowFlagValue(cpu, (temp & 0x40));

    temp &= cpu->registers.a_register;
    
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

    SetZeroFlagValue(cpu, !(res & 0x00FF));

#ifdef NO_DECIMAL_ADC_SUPPORT
    SetNegativeFlagValue(cpu, (res & 0x80));
    SetOverflowFlagValue(cpu, (cpu->registers.a_register ^ (uint8_t)(res & 0x00FF)) & (temp ^ (uint8_t)(res & 0x00FF)) & 0x80);
    SetCarryFlagValue(cpu, (res > 0x00FF));
#else
    if (GetDecimalModeFlagValue(cpu)) {
        if (((cpu->registers.a_register & 0x0F) + (temp & 0x0F) + GetCarryFlagValue(cpu)) > 0x09) {
            res += 6;
        }
    
        SetNegativeFlagValue(cpu, (res & 0x80));
        SetOverflowFlagValue(cpu, (!((cpu->registers.a_register ^ temp) & 0x80) && ((cpu->registers.a_register ^ (uint8_t)res) & 0x80)));
        
        if (res > 0x0099) {
            res += 96;
        }
    
        SetCarryFlagValue(cpu, (res > 0x0099));
    
        cpu->remaining_cycles++;
    } else {
        SetNegativeFlagValue(cpu, (res & 0x80));
        SetOverflowFlagValue(cpu, (cpu->registers.a_register ^ (uint8_t)(res & 0x00FF)) & (temp ^ (uint8_t)(res & 0x00FF)) & 0x80);
        SetCarryFlagValue(cpu, (res > 0x00FF));
    }
#endif

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
    SetOverflowFlagValue(cpu, (cpu->registers.a_register ^ (uint8_t)(res & 0x00FF)) & (~temp ^ (uint8_t)(res & 0x00FF)) & 0x80);

#ifdef NO_DECIMAL_SBC_SUPPORT
#else
    if (GetDecimalModeFlagValue(cpu)) {
        if (((cpu->registers.a_register & 0x0F) + GetCarryFlagValue(cpu) - 1) < (temp & 0x0F)) {
            res -= 6;
        }

        
        if (res > 0x0099) {
            res -= 0x60;
        }

        cpu->remaining_cycles++;
    }
#endif

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
    // 0
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
    // 1
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
	// 2
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
	// 3
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
	// 4
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
	// 5
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
	// 6
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
	// 7
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
	// 8
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
	// 9
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
	// A
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
	// B
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
	// C
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
	// D
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
	// E
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
	// F
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

    cpu->dma_transfer = false;
    cpu->dma_aligned = false;
    cpu->dma_address = 0;
    cpu->oam_address = 0;
    cpu->oam_data = 0;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);
}


void CPUReset(struct CPU* cpu) {
    cpu->registers.program_counter = ReadLittleEndianWord(cpu, RESET_OFFSET);
    
    cpu->registers.stack_pointer -= 3;

    SetUnusedFlagValue(cpu, 1);
    SetIrgDisableFlagValue(cpu, 1);

    cpu->remaining_cycles = 8;

    cpu->dma_transfer = false;
    cpu->dma_aligned = false;
    cpu->dma_address = 0;
    cpu->oam_address = 0;
    cpu->oam_data = 0;
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
    if (cpu->dma_transfer) {
        if (cpu->dma_aligned) {
            if (cpu->tick_counter % 2 == 0) {
                cpu->oam_data = ReadByte(cpu, cpu->dma_address);
                cpu->dma_address++;
            } else {
                cpu->cpu_bus->ppu->OAM[cpu->oam_address] = cpu->oam_data;
                cpu->cpu_bus->cpu_open_bus_data = cpu->oam_data;
                cpu->oam_address++;

                if (cpu->oam_address == 0) {
                    cpu->dma_transfer = false;
                }
            }
        } else if (cpu->tick_counter % 2 == 1) {
            cpu->dma_aligned = true;
        }
    } else {
        if (cpu->remaining_cycles == 0) {
            uint8_t op_code = ReadByte(cpu, cpu->registers.program_counter);
            cpu->registers.program_counter++;

            Instruction instruction = instructions[op_code];

            //printf("mnemonic: %s  -  opcode: 0x%02X  -  A: 0x%02X  -  X: 0x%02X  -  Y: 0x%02X  -  Status: 0x%02X  -  Stack pointer: 0x%02X  -  Program counter: 0x%04X  -  tick: %lu\n", 
            //       instruction.mnemonic, op_code, cpu->registers.a_register, cpu->registers.x_register, cpu->registers.y_register, 
            //       cpu->registers.status_flags, cpu->registers.stack_pointer, cpu->registers.program_counter, cpu->tick_counter - 1);

            cpu->remaining_cycles = instruction.cycles;
            uint16_t absolute_address = instruction.address_mode(cpu);

            instruction.operator(cpu, absolute_address);
        }
        cpu->remaining_cycles--;
    }

    cpu->tick_counter++;
}


static inline bool IsSafeToReadByte(uint16_t address) {
    return (address < 0x2000 || address >= 0x4020);
}

static inline bool IsSafeToReadLittleEndieanWord(uint16_t address) {
    return (address < 0x1FFF || address >= 0x4021);
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
        for (int x = 0; x < ZERO_PAGE_BYTE_BUFFER_WIDTH; x++) {
            uint16_t temp_address = y * ZERO_PAGE_BYTE_BUFFER_WIDTH + x;
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
        
        if (IsSafeToReadByte(dummy_address)) { // with certain mappers this still could cause side effects but minimizes the chance
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
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " IMM  0x%02X", temp_address);
                    } 
                    x += 10;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &ZeroPage) {
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " ZP   0x%02X", temp_address);
                    } 
                    x += 10;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &ZeroPageX) {
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " ZPX  0x%02X", temp_address);
                    } 
                    x += 10;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &ZeroPageY) {
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " ZPY  0x%02X", temp_address);
                    } 
                    x += 10;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &Relative) {
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address = ReadByte(cpu, dummy_address);
                    dummy_address++;
                    if ((x + 11) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 11 * sizeof(char), " REL %c0x%02X", (((int8_t)temp_address < 0) ? '-' : '+'), (((int8_t)temp_address < 0) ? (-1 * (int8_t)temp_address) : temp_address));
                    } 
                    x += 10;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &Absolute) {
                if (IsSafeToReadLittleEndieanWord(dummy_address)) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " ABS  0x%04X", temp_address);
                    } 
                    x += 12;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &AbsoluteX) {
                if (IsSafeToReadLittleEndieanWord(dummy_address)) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " ABX  0x%04X", temp_address);
                    } 
                    x += 12;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &AbsoluteY) {
                if (IsSafeToReadLittleEndieanWord(dummy_address)) {
                    uint16_t temp_address = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
                    if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " ABY  0x%04X", temp_address);
                    } 
                    x += 12;
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &Indirect) {
                if (IsSafeToReadLittleEndieanWord(dummy_address)) {
                    uint16_t temp_address_ptr = ReadLittleEndianWord(cpu, dummy_address);
                    dummy_address += 2;
            
                    uint16_t temp_address_ptr_to_high = ((temp_address_ptr & 0xFF00) | ((temp_address_ptr + 1) & 0x00FF));
                    uint16_t temp_address_ptr_to_low = temp_address_ptr;
            
                    if (IsSafeToReadByte(temp_address_ptr_to_low) && IsSafeToReadByte(temp_address_ptr_to_high)) {
                        uint16_t temp_address = (uint16_t)(ReadByte(cpu, temp_address_ptr_to_high) << 8) | (uint16_t)ReadByte(cpu, temp_address_ptr_to_low);
                        if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " IND  0x%04X", temp_address);
                        } 
                        x += 12;
                    } else { 
                        if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                        }
                        x += 15;
                    }
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &IndirectX) {
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address_ptr = ReadByte(cpu, dummy_address);
                    dummy_address++;
                
                    uint16_t temp_address_ptr_to_high = (((uint16_t)temp_address_ptr + cpu->registers.x_register + 1) & 0x00FF);
                    uint16_t temp_address_ptr_to_low = (((uint16_t)temp_address_ptr + cpu->registers.x_register) & 0x00FF);
                
                    if (IsSafeToReadByte(temp_address_ptr_to_low) && IsSafeToReadByte(temp_address_ptr_to_high)) {
                        uint16_t temp_address = (uint16_t)(ReadByte(cpu, temp_address_ptr_to_high) << 8) | (uint16_t)ReadByte(cpu, temp_address_ptr_to_low);
                        if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " INX  0x%04X", temp_address);
                        } 
                        x += 12;
                    } else { 
                        if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                        }
                        x += 15;
                    }
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else if (instruction.address_mode == &IndirectY) {
                if (IsSafeToReadByte(dummy_address)) {
                    uint8_t temp_address_ptr = ReadByte(cpu, dummy_address);
                    dummy_address++;
                
                    uint16_t temp_address_ptr_to_high = (((uint16_t)temp_address_ptr + 1) & 0x00FF);
                    uint16_t temp_address_ptr_to_low = (uint16_t)temp_address_ptr;
                
                    if (IsSafeToReadByte(temp_address_ptr_to_low) && IsSafeToReadByte(temp_address_ptr_to_high)) {
                        uint16_t temp_address = (uint16_t)(ReadByte(cpu, temp_address_ptr_to_high) << 8) | (uint16_t)ReadByte(cpu, temp_address_ptr_to_low);
                        temp_address += cpu->registers.y_register;
                        if ((x + 13) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 13 * sizeof(char), " INY  0x%04X", temp_address);
                        } 
                        x += 12;
                    } else { 
                        if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                            snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                        }
                        x += 15;
                    }
                } else { 
                    if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                        snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
                    }
                    x += 15;
                }
            }
            else {
                printf("unknown addressing mode\n");
                exit(1);
            }
        } else { 
            if ((x + 16) < (DISASSEMBLY_BUFFER_WIDTH + 1)) {
                snprintf(&disassembly_row_buffer[x], 16 * sizeof(char), " UNSAFE ADDRESS");
            }
            x += 15;
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