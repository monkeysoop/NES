#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/stat.h>

#define STACK_OFFSET 0x0100
#define RESET_OFFSET 0xFFFC
#define BREAK_INTERRUPT_OFFSET 0xFFFE
#define NON_MASKABLE_INTERRUPT_OFFSET 0xFFFA

typedef enum {
    CARRY =         0b00000001,
    ZERO =          0b00000010,
    IRQ_DISABLE =   0b00000100,
    DECIMAL_MODE =  0b00001000,
    BRK_COMMAND =   0b00010000,
    UNUSED =        0b00100000,
    OVERFLOW =      0b01000000,
    NEGATIVE =      0b10000000
} StatusFlags;


typedef struct {
    uint8_t a_register;
	uint8_t x_register; 
	uint8_t y_register;   
    uint8_t status_flags;
    uint8_t stack_pointer; 
	uint16_t program_counter;
} Registers;

typedef struct {
    char mnemonic[4];
    void (*operator)(Registers*, uint8_t*, const uint16_t);
    uint16_t (*address_mode)(Registers*, uint8_t*);
    uint8_t cycles;
} Instruction;


uint16_t IlligalMode(Registers* registers, uint8_t* remaining_cycles);

uint16_t Accumulator(Registers* registers, uint8_t* remaining_cycles);
uint16_t Immediate(Registers* registers, uint8_t* remaining_cycles);
uint16_t Absolute(Registers* registers, uint8_t* remaining_cycles);
uint16_t ZeroPage(Registers* registers, uint8_t* remaining_cycles);
uint16_t ZeroPageX(Registers* registers, uint8_t* remaining_cycles);
uint16_t ZeroPageY(Registers* registers, uint8_t* remaining_cycles);
uint16_t AbsoluteX(Registers* registers, uint8_t* remaining_cycles);
uint16_t AbsoluteY(Registers* registers, uint8_t* remaining_cycles);
uint16_t Implied(Registers* registers, uint8_t* remaining_cycles);
uint16_t Relative(Registers* registers, uint8_t* remaining_cycles);
uint16_t IndirectX(Registers* registers, uint8_t* remaining_cycles);
uint16_t IndirectY(Registers* registers, uint8_t* remaining_cycles);
uint16_t Indirect(Registers* registers, uint8_t* remaining_cycles);



void ILL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);

void BRK(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void ORA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void ASL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void PHP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BPL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CLC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void JSR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void AND(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BIT(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void ROL(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void PLP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BMI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void SEC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void RTI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void EOR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void LSR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void PHA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void JMP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BVC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CLI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void RTS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void ADC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void ROR(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void PLA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BVS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void SEI(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void STA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void STY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void STX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void DEY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void TXA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BCC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void TYA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void TXS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void LDY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void LDA(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void LDX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void TAY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void TAX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BCS(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CLV(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void TSX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CPY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CMP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void DEC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void INY(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void DEX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BNE(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CLD(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void CPX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void SBC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void INC(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void INX(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void NOP(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void BEQ(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);
void SED(Registers* registers, uint8_t* remaining_cycles, const uint16_t absolute_address);

#endif