#include "emulator.h"
#include "logger.h"


void EmulatorInit(struct Emulator* emulator, const char* filename) {
    CartridgeInit(&emulator->cartridge, filename);
    PPUBusInit(&emulator->ppu_bus, &emulator->cartridge);
    PPUInit(&emulator->ppu, &emulator->ppu_bus, emulator->cartridge.tv_system);
    ControllerInit(&emulator->controller);
    CPUBusInit(&emulator->cpu_bus, &emulator->cartridge, &emulator->ppu, &emulator->controller);
    CPUInit(&emulator->cpu, &emulator->cpu_bus);
}

void EmulatorClean(struct Emulator* emulator) {
    CartridgeClean(&emulator->cartridge);
}

void EmulatorReset(struct Emulator* emulator) {
    CPUReset(&emulator->cpu);
    PPUReset(&emulator->ppu, emulator->cartridge.tv_system);
    
    CPUBusReset(&emulator->cpu_bus);
    PPUBusReset(&emulator->ppu_bus);

    ControllerReset(&emulator->controller);
}

void EmulatorReloadCartridge(struct Emulator* emulator, const char* filename) {

}

void EmulatorKeyDown(struct Emulator* emulator, enum Button button) {
    ControllerKeyDown1(&emulator->controller, button);
    ControllerKeyDown2(&emulator->controller, button);
}

void EmulatorKeyUp(struct Emulator* emulator, enum Button button) {
    ControllerKeyUp1(&emulator->controller, button);
    ControllerKeyUp2(&emulator->controller, button);
}

void EmulatorRender(struct Emulator* emulator, uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT]) {
    uint64_t temp = 0;
    switch (emulator->cartridge.tv_system) {
        case NTSC: 
            while (emulator->ppu.render_state != FINISHED) {
                switch (PPUClockNTSC(&emulator->ppu, pixels_buffer)) {
                    case GENERATE_NMI: CPUNonMaskableInterrupt(&emulator->cpu); break;
                    case GENERATE_IRQ: CPUInterruptRequest(&emulator->cpu); break;
                    case GENERATE_NO_INTERRUPT: break;
                }
                
                if (temp % 3 == 2) {
                    CPUClock(&emulator->cpu);
                    // apu
                }
            
                temp++;
            }
            break;
        case PAL:
            while (emulator->ppu.render_state != FINISHED) {
                switch (PPUClockPAL(&emulator->ppu, pixels_buffer)) {
                    case GENERATE_NMI: CPUNonMaskableInterrupt(&emulator->cpu); break;
                    case GENERATE_IRQ: CPUInterruptRequest(&emulator->cpu); break;
                    case GENERATE_NO_INTERRUPT: break;
                }

                if (temp % 15 == 14 && emulator->ppu.render_state != FINISHED) {    // makes the ratio of ppu and cpu cycles to 3.2 : 1 instead of 3 : 1
                    switch (PPUClockPAL(&emulator->ppu, pixels_buffer)) {
                        case GENERATE_NMI: CPUNonMaskableInterrupt(&emulator->cpu); break;
                        case GENERATE_IRQ: CPUInterruptRequest(&emulator->cpu); break;
                        case GENERATE_NO_INTERRUPT: break;
                    }
                }

                if (temp % 3 == 2) {
                    CPUClock(&emulator->cpu);
                    // apu
                }

                temp++;
            } 
            break;
    }
    emulator->ppu.render_state = PRE_RENDER;
}