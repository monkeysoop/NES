#include "cpu_bus.h"


void CPUBusInit(CPUBus* cpu_bus, Cartridge* cartridge, PPU* ppu) {
    memset(cpu_bus->cpu_ram, 0x00, CPU_RAM_SIZE * sizeof(uint8_t));

    cpu_bus->cpu_open_bus_data = 0x00;
    cpu_bus->ppu_io_open_bus_data = 0x00;

    cpu_bus->cartridge = cartridge;
    cpu_bus->ppu = ppu;
}

void CPUBusReset(CPUBus* cpu_bus) {
    memset(cpu_bus->cpu_ram, 0x00, CPU_RAM_SIZE * sizeof(uint8_t));

    cpu_bus->cpu_open_bus_data = 0x00;
    cpu_bus->ppu_io_open_bus_data = 0x00;
}


uint8_t CPUBusRead(CPUBus* cpu_bus, const uint16_t address) {
    if (address < 0x2000) {
        cpu_bus->cpu_open_bus_data = cpu_bus->cpu_ram[(address & 0x07FF)];
    } else if (address < 0x4000) {
        // ppu io registers
        switch (address & 0x2007) {
            case PPU_CTRL: printf("open buss read\n"); break;
            case PPU_MASK: printf("open buss read\n"); break;
            case PPU_STATUS: 
                cpu_bus->ppu_io_open_bus_data &= STALE_PPU_BUS_CONTENTS_BITS;
                cpu_bus->ppu_io_open_bus_data |= (PPUReadStatus(cpu_bus->ppu) & (SPRITE_OVERFLOW_BITS | SPRITE_ZERO_HIT_BITS | VERTICAL_BLANK_BITS)); 
                break;
            case OAM_ADDRESS: printf("open buss read\n"); break;
            case OAM_DATA: 
                cpu_bus->ppu_io_open_bus_data = PPUReadOAMData(cpu_bus->ppu); 
                break;
            case PPU_SCROLL: printf("open buss read\n"); break;
            case PPU_ADDRESS: printf("open buss read\n"); break;
            case PPU_DATA: 
                cpu_bus->ppu_io_open_bus_data = PPUReadPPUData(cpu_bus->ppu); break;
        }
        cpu_bus->cpu_open_bus_data = cpu_bus->ppu_io_open_bus_data;
    } else if (address < 0x4018) {
        // apu and io
        switch (address) {
            case APU_CTRL: ; break;
            case JOYSTICK_1_DATA: ; break;
            case JOYSTICK_2_DATA: ; break;
            default: printf("open buss read\n"); break;
        }

        printf("apu and io registers not implemented\n"); 
        exit(1);
    } else if (address < 0x4020) {
        // ignored
        printf("cpu test mode not implemented\n");
        exit(1);
    } else {
        cpu_bus->cpu_open_bus_data = CPUReadCartridge(cpu_bus->cartridge, address);
    }
    return cpu_bus->cpu_open_bus_data;
}

void CPUBusWrite(CPUBus* cpu_bus, const uint16_t address, const uint8_t data) {
    cpu_bus->cpu_open_bus_data = data;

    if (address < 0x2000) {
        cpu_bus->cpu_ram[(address & 0x07FF)] = data;
    } else if (address < 0x4000) {
        // ppu registers
        cpu_bus->ppu_io_open_bus_data = data;
        switch (address & 0x2007) {
            case PPU_CTRL: 
                PPUWriteCtrl(cpu_bus->ppu, data);
                break;
            case PPU_MASK: 
                PPUWriteMask(cpu_bus->ppu, data);
                break;
            case PPU_STATUS: 
                printf("open buss write\n");
                break;
            case OAM_ADDRESS: 
                PPUWriteOAMAddress(cpu_bus->ppu, data);
                break;
            case OAM_DATA: 
                PPUWriteOAMData(cpu_bus->ppu, data);
                break;
            case PPU_SCROLL: 
                PPUWriteScroll(cpu_bus->ppu, data);
                break;
            case PPU_ADDRESS: 
                PPUWritePPUAddress(cpu_bus->ppu, data);
                break;
            case PPU_DATA: 
                PPUWritePPUData(cpu_bus->ppu, data);
                break;
            case PPU_DMA:
                break;
        }
    } else if (address < 0x4018) {
        switch (address) {
            case APU_PULSE_1_CTRL: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_1_SWEEP: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_1_LOW_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_1_HIGH_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_2_CTRL: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_2_SWEEP: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_2_LOW_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_PULSE_2_HIGH_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_TRIANGLE_CTRL: printf("apu not implemented\n"); exit(1); break;
            case APU_TRIANGLE_UNUSED: printf("write to unused address"); exit(1); break;
            case APU_TRIANGLE_LOW_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_TRIANGLE_HIGH_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_NOISE_CTRL: printf("apu not implemented\n"); exit(1); break;
            case APU_NOISE_UNUSED: printf("write to unused address"); exit(1); break;
            case APU_NOISE_LOW_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_NOISE_HIGH_BYTE: printf("apu not implemented\n"); exit(1); break;
            case APU_DMC_CTRL: printf("apu not implemented\n"); exit(1); break;
            case APU_DMC_DIRECT_LOAD: printf("apu not implemented\n"); exit(1); break;
            case APU_DMC_ADDRESS: printf("apu not implemented\n"); exit(1); break;
            case APU_DMC_LENGTH: printf("apu not implemented\n"); exit(1); break;
            case PPU_DMA: ; 
                PPUWriteDMAAddress(cpu_bus->ppu, data);
                break;
            case APU_CTRL: printf("apu not implemented\n"); exit(1); break;
            case JOYSTICK_STROBE: printf("joystick not implemented\n"); exit(1); break; // TODO: open bus behavior
            case APU_FRAME_COUNTER: printf("apu not implemented\n"); exit(1); break;    // TODO: open bus behavior
        }
    } else if (address < 0x4020) {
        // ignored
        printf("cpu test mode not implemented\n");
        exit(1);
    } else {
        CPUWriteCartridge(cpu_bus->cartridge, address, data);
    }
}
