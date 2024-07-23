#include "emulator.h"


void EmulatorInit(Emulator* emulator, const char* filename) {
    CartridgeInit(&emulator->cartridge, filename);
    CPUInit(&emulator->cpu);

    PPUBusInit(&emulator->ppu_bus, &emulator->cartridge);
    PPUInit(&emulator->ppu, &emulator->ppu_bus, emulator->cartridge.tv_system);
    
    CPUBusInit(&emulator->cpu_bus, &emulator->cartridge, &emulator->ppu);
}

void EmulatorClean(Emulator* emulator) {
    CartridgeClean(&emulator->cartridge);
}

void EmulatorReset(Emulator* emulator) {
    CPUReset(&emulator->cpu);
    PPUReset(&emulator->ppu, emulator->cartridge.tv_system);
    
    CPUBusReset(&emulator->cpu_bus);
    PPUBusReset(&emulator->ppu_bus);
}

void EmulatorReloadCartridge(Emulator* emulator, const char* filename) {

}

void EmulatorTick(Emulator* emulator) {
    switch (emulator->cartridge.tv_system) {
        case NTSC: 
            while (emulator->ppu.render_state != FINISHED) {
                PPUClockNTSC(&emulator->ppu);
                PPUClockNTSC(&emulator->ppu);
                PPUClockNTSC(&emulator->ppu);
                
                CPUClock(&emulator->cpu);

                // apu
            }
            break;
        case PAL:
            uint8_t temp = 0;
            while (emulator->ppu.render_state != FINISHED) {
                temp++;
                PPUClockPAL(&emulator->ppu);
                PPUClockPAL(&emulator->ppu);
                PPUClockPAL(&emulator->ppu);
                if (temp == 5) {
                    temp = 0;
                    PPUClockPAL(&emulator->ppu);
                }

                CPUClock(&emulator->cpu);

                // apu
            } 
            break;
    }
    emulator->ppu.render_state = PRE_RENDER;
}