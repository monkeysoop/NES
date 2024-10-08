#include <string.h>

#include "cpu_bus.h"
#include "logger.h"


void CPUBusInit(struct CPUBus* cpu_bus, struct Cartridge* cartridge, struct PPU* ppu, struct Controller* controller) {
    memset(cpu_bus->cpu_ram, 0x00, CPU_RAM_SIZE * sizeof(uint8_t));

    cpu_bus->cpu_open_bus_data = 0x00;
    cpu_bus->ppu_io_open_bus_data = 0x00;

    cpu_bus->cartridge = cartridge;
    cpu_bus->ppu = ppu;
    cpu_bus->controller = controller;
}

void CPUBusReset(struct CPUBus* cpu_bus) {
    memset(cpu_bus->cpu_ram, 0x00, CPU_RAM_SIZE * sizeof(uint8_t));

    cpu_bus->cpu_open_bus_data = 0x00;
    cpu_bus->ppu_io_open_bus_data = 0x00;
}


uint8_t CPUBusRead(struct CPUBus* cpu_bus, const uint16_t address) {
    if (address < 0x2000) {
        cpu_bus->cpu_open_bus_data = cpu_bus->cpu_ram[(address & 0x07FF)];
    } else if (address < 0x4000) {
        // ppu io registers
        switch (address & 0x2007) {
            case PPU_CTRL: LOG(WARNING, CPU_BUS, "open bus read: 0x%04X\n", address); break;
            case PPU_MASK: LOG(WARNING, CPU_BUS, "open bus read: 0x%04X\n", address); break;
            case PPU_STATUS: 
                cpu_bus->ppu_io_open_bus_data &= STALE_PPU_BUS_CONTENTS_BITS;
                cpu_bus->ppu_io_open_bus_data |= (PPUReadStatus(cpu_bus->ppu) & (SPRITE_OVERFLOW_BIT | SPRITE_ZERO_HIT_BIT | VERTICAL_BLANK_BIT)); 
                break;
            case OAM_ADDRESS: LOG(WARNING, CPU_BUS, "open bus read: 0x%04X\n", address); break;
            case OAM_DATA: 
                cpu_bus->ppu_io_open_bus_data = PPUReadOAMData(cpu_bus->ppu); 
                break;
            case PPU_SCROLL: LOG(WARNING, CPU_BUS, "open bus read: 0x%04X\n", address); break;
            case PPU_ADDRESS: LOG(WARNING, CPU_BUS, "open bus read: 0x%04X\n", address); break;
            case PPU_DATA: 
                cpu_bus->ppu_io_open_bus_data = PPUReadPPUData(cpu_bus->ppu); break;
        }
        cpu_bus->cpu_open_bus_data = cpu_bus->ppu_io_open_bus_data;
    } else if (address < 0x4018) {
        // apu and io
        switch (address) {
            case APU_CTRL:  
                LOG(WARNING, CPU_BUS, "apu and io registers not implemented\n"); 
                break;
            case JOYSTICK_1_DATA: 
                cpu_bus->cpu_open_bus_data = 0x40;
                cpu_bus->cpu_open_bus_data |= (ControllerRead1(cpu_bus->controller) & 0x1F); 
                break;
            case JOYSTICK_2_DATA: 
                cpu_bus->cpu_open_bus_data = 0x40;
                cpu_bus->cpu_open_bus_data |= (ControllerRead2(cpu_bus->controller) & 0x1F); 
                break;
            default: LOG(WARNING, CPU_BUS, "open bus read: 0x%04X\n", address); break;
        }
    } else if (address < 0x4020) {
        // ignored
        LOG(ERROR, CPU_BUS, "cpu test mode not implemented\n");
    } else {
        cpu_bus->cpu_open_bus_data = CartridgeReadCPU(cpu_bus->cartridge, address);
    }
    return cpu_bus->cpu_open_bus_data;
}

bool CPUBusWrite(struct CPUBus* cpu_bus, const uint16_t address, const uint8_t data) {
    bool dma_transfer_initiated = false;

    uint8_t previous_cpu_open_bus_data = cpu_bus->cpu_open_bus_data;
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
                LOG(DEBUG_INFO, CPU_BUS, "open bus write: 0x%04X\n", address);
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
            case APU_PULSE_1_CTRL: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_1_SWEEP: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_1_LOW_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_1_HIGH_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_2_CTRL: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_2_SWEEP: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_2_LOW_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_PULSE_2_HIGH_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_TRIANGLE_CTRL: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_TRIANGLE_UNUSED: LOG(WARNING, CPU_BUS, "write to unused address"); break;
            case APU_TRIANGLE_LOW_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_TRIANGLE_HIGH_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_NOISE_CTRL: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_NOISE_UNUSED: LOG(WARNING, CPU_BUS, "write to unused address"); break;
            case APU_NOISE_LOW_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_NOISE_HIGH_BYTE: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_DMC_CTRL: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_DMC_DIRECT_LOAD: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_DMC_ADDRESS: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case APU_DMC_LENGTH: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case PPU_DMA:
                dma_transfer_initiated = true;
                break;
            case APU_CTRL: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;
            case JOYSTICK_STROBE: 
                cpu_bus->cpu_open_bus_data = (previous_cpu_open_bus_data & 0xE0) | (data & 0x1F);
                ControllerWrite(cpu_bus->controller, data);
                break;
            case APU_FRAME_COUNTER: LOG(WARNING, CPU_BUS, "apu not implemented\n"); break;    // TODO: open bus behavior
        }
    } else if (address < 0x4020) {
        // ignored
        LOG(ERROR, CPU_BUS, "cpu test mode not implemented\n");
    } else {
        CartridgeWriteCPU(cpu_bus->cartridge, address, data);
    }

    return dma_transfer_initiated;
}
