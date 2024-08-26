# NES emulator

written in C (with SDL2 for graphics) for fun

## Implemented mappers:
* 000 - NROM
* 001 - MMC1
* 002 - UxROM
* 003 - CNROM
* 004 - MMC3
* 007 - AxROM
* 011 - Color Dreams
* 066 - GxROM

## Supports:
* Official opcodes
* NTSC video (USA and Japan roms)
* 2 seperate controller
* Archaic iNES format
* iNES format
* Custom debug view

## Not supported/implemented:
* Unofficial opcodes
* PAL, Dendy video
* Turbo keys on the controllers
* NES 2.0 format
* APU (so no audio)
* Battery backed RAM (so no saves)
* other mappers

## Other limitations:
* The emulator is only mostly cycle accurate
* GUI is very limited and to switch keys you have to edit the source
* definitely needs more testing

## About debug view:
it displays:
* Zero Page (CPU memmory at 0x0000 - 0x00FF) - green
* CPU registers - purple
* Disassembly of the code - red with blue (blue is at the program counter, so what is being executed)
* PPU palette
* PPU pattern table with the selected palette
* The selected nametable (PPU memmory at 0x2000 - 0x23FF or 0x2400 - 0x27FF or 0x2800 - 0x2BFF or 0x2C00 - 0x2FFF) - yellow

![Debug View](screenshots/DebugView.png)

## Favorite screenshots:
![TheLegendOfZelda_1](screenshots/TheLegendOfZelda_1.png)
![cool_logo](screenshots/cool_logo.png)
![MegaMan5_6](screenshots/MegaMan5_6.png)
![MegaMan4_2](screenshots/MegaMan4_2.png)

## Build instructions for Linux
### Option 1 build locally:
* install required packages:
```shell
sudo apt install git
```

```shell
sudo apt install cmake
```

```shell
sudo apt install libsdl2-dev
```

```shell
sudo apt install libsdl2-image-dev
```

* clone repo:
```shell
git clone --recurse-submodules https://github.com/monkeysoop/NES
```

```shell
cd NES
```

* running setup.sh creates /build and generates all the required Makefiles and copies font.png and /tests directory to where the executable will be  
```shell
chmod +x setup.sh
```

```shell
./setup.sh
```

* running run.sh builds the project and runs it, pass the path to the .nes rom file to it (Note: the executable will be in /build so you should either put the .nes rom in there or put it in /tests and rerun setup.sh)
```shell
chmod +x run.sh
```

```shell
./run.sh tests/nestest.nes
```
nestest.nes is a simple program that is included in the repo (which tests the cpu intructions)

### Option 2 build and run in docker:
TODO

## "Some" of the games that I ran succesfully:
* Castlevania
![Castlevania_1](screenshots/Castlevania_1.png)
![Castlevania_2](screenshots/Castlevania_2.png)
* Castlevania2SimonsQuest
![Castlevania2SimonsQuest_1](screenshots/Castlevania2SimonsQuest_1.png)
![Castlevania2SimonsQuest_2](screenshots/Castlevania2SimonsQuest_2.png)
![Castlevania2SimonsQuest_3](screenshots/Castlevania2SimonsQuest_3.png)
* Contra
![Contra_1](screenshots/Contra_1.png)
![Contra_2](screenshots/Contra_2.png)
* ContraForce
![ContraForce_1](screenshots/ContraForce_1.png)
![ContraForce_2](screenshots/ContraForce_2.png)
* DonkeyKong
![DonkeyKong_1](screenshots/DonkeyKong_1.png)
![DonkeyKong_2](screenshots/DonkeyKong_2.png)
* DonkeyKong3
![DonkeyKong3_1](screenshots/DonkeyKong3_1.png)
![DonkeyKong3_2](screenshots/DonkeyKong3_2.png)
* DuckHunt
![DuckHunt_1](screenshots/DuckHunt_1.png)
![DuckHunt_2](screenshots/DuckHunt_2.png)
* DuckTales
![DuckTales_1](screenshots/DuckTales_1.png)
![DuckTales_2](screenshots/DuckTales_2.png)
![DuckTales_3](screenshots/DuckTales_3.png)
* DuckTales2
![DuckTales2_1](screenshots/DuckTales2_1.png)
![DuckTales2_2](screenshots/DuckTales2_2.png)
![DuckTales2_3](screenshots/DuckTales2_3.png)
* Jaws
![Jaws_1](screenshots/Jaws_1.png)
![Jaws_2](screenshots/Jaws_2.png)
![Jaws_3](screenshots/Jaws_3.png)
* JurassicPark
![JurassicPark_1](screenshots/JurassicPark_1.png)
![JurassicPark_2](screenshots/JurassicPark_2.png)
![JurassicPark_3](screenshots/JurassicPark_3.png)
![JurassicPark_4](screenshots/JurassicPark_4.png)
* MadMax
![MadMax_1](screenshots/MadMax_1.png)
![MadMax_2](screenshots/MadMax_2.png)
* MarioBros
![MarioBros_1](screenshots/MarioBros_1.png)
![MarioBros_2](screenshots/MarioBros_2.png)
* MarioIsMissing
![MarioIsMissing_1](screenshots/MarioIsMissing_1.png)
![MarioIsMissing_2](screenshots/MarioIsMissing_2.png)
![MarioIsMissing_3](screenshots/MarioIsMissing_3.png)
![MarioIsMissing_4](screenshots/MarioIsMissing_4.png)
* MariosTimeMachine
![MariosTimeMachine_1](screenshots/MariosTimeMachine_1.png)
![MariosTimeMachine_2](screenshots/MariosTimeMachine_2.png)
![MariosTimeMachine_3](screenshots/MariosTimeMachine_3.png)
* MegaMan
![MegaMan_1](screenshots/MegaMan_1.png)
![MegaMan_2](screenshots/MegaMan_2.png)
![MegaMan_3](screenshots/MegaMan_3.png)
![MegaMan_4](screenshots/MegaMan_4.png)
* MegaMan2
![MegaMan2_1](screenshots/MegaMan2_1.png)
![MegaMan2_2](screenshots/MegaMan2_2.png)
![MegaMan2_3](screenshots/MegaMan2_3.png)
![MegaMan2_4](screenshots/MegaMan2_4.png)
![MegaMan2_5](screenshots/MegaMan2_5.png)
![MegaMan2_6](screenshots/MegaMan2_6.png)
* MegaMan3
![MegaMan3_1](screenshots/MegaMan3_1.png)
![MegaMan3_2](screenshots/MegaMan3_2.png)
![MegaMan3_3](screenshots/MegaMan3_3.png)
![MegaMan3_4](screenshots/MegaMan3_4.png)
![MegaMan3_5](screenshots/MegaMan3_5.png)
* MegaMan4
![MegaMan4_1](screenshots/MegaMan4_1.png)
![MegaMan4_2](screenshots/MegaMan4_2.png)
![MegaMan4_3](screenshots/MegaMan4_3.png)
![MegaMan4_4](screenshots/MegaMan4_4.png)
![MegaMan4_5](screenshots/MegaMan4_5.png)
![MegaMan4_6](screenshots/MegaMan4_6.png)
![MegaMan4_7](screenshots/MegaMan4_7.png)
![MegaMan4_8](screenshots/MegaMan4_8.png)
* MegaMan5
![MegaMan5_1](screenshots/MegaMan5_1.png)
![MegaMan5_2](screenshots/MegaMan5_2.png)
![MegaMan5_3](screenshots/MegaMan5_3.png)
![MegaMan5_4](screenshots/MegaMan5_4.png)
![MegaMan5_5](screenshots/MegaMan5_5.png)
![MegaMan5_6](screenshots/MegaMan5_6.png)
![MegaMan5_7](screenshots/MegaMan5_7.png)
* MegaMan6
![MegaMan6_1](screenshots/MegaMan6_1.png)
![MegaMan6_2](screenshots/MegaMan6_2.png)
![MegaMan6_3](screenshots/MegaMan6_3.png)
![MegaMan6_4](screenshots/MegaMan6_4.png)
![MegaMan6_5](screenshots/MegaMan6_5.png)
![MegaMan6_6](screenshots/MegaMan6_6.png)
![MegaMan6_7](screenshots/MegaMan6_7.png)
![MegaMan6_8](screenshots/MegaMan6_8.png)
![MegaMan6_9](screenshots/MegaMan6_9.png)
* MetalFighter
![MetalFighter_1](screenshots/MetalFighter_1.png)
![MetalFighter_2](screenshots/MetalFighter_2.png)
![MetalFighter_3](screenshots/MetalFighter_3.png)
* Metroid
![Metroid_1](screenshots/Metroid_1.png)
![Metroid_2](screenshots/Metroid_2.png)
* PacMan
![PacMan_1](screenshots/PacMan_1.png)
![PacMan_2](screenshots/PacMan_2.png)
![PacMan_3](screenshots/PacMan_3.png)
* PinBall
![PinBall_1](screenshots/PinBall_1.png)
![PinBall_2](screenshots/PinBall_2.png)
![PinBall_3](screenshots/PinBall_3.png)
* Robocop
![Robocop_1](screenshots/Robocop_1.png)
![Robocop_2](screenshots/Robocop_2.png)
* Robocop2
![Robocop2_1](screenshots/Robocop2_1.png)
![Robocop2_2](screenshots/Robocop2_2.png)
![Robocop2_3](screenshots/Robocop2_3.png)
* Robocop3
![Robocop3_1](screenshots/Robocop3_1.png)
![Robocop3_2](screenshots/Robocop3_2.png)
* SuperMarioBros
![SuperMarioBros_1](screenshots/SuperMarioBros_1.png)
![SuperMarioBros_2](screenshots/SuperMarioBros_2.png)
![SuperMarioBros_3](screenshots/SuperMarioBros_3.png)
* SuperMarioBros2
![SuperMarioBros2_1](screenshots/SuperMarioBros2_1.png)
![SuperMarioBros2_2](screenshots/SuperMarioBros2_2.png)
![SuperMarioBros2_3](screenshots/SuperMarioBros2_3.png)
* SuperMarioBros3
![SuperMarioBros3_1](screenshots/SuperMarioBros3_1.png)
![SuperMarioBros3_2](screenshots/SuperMarioBros3_2.png)
* SuperMarioBrosDuckHunt
![SuperMarioBrosDuckHunt_1](screenshots/SuperMarioBrosDuckHunt_1.png)
![SuperMarioBrosDuckHunt_2](screenshots/SuperMarioBrosDuckHunt_2.png)
* Tetris
![Tetris_1](screenshots/Tetris_1.png)
![Tetris_2](screenshots/Tetris_2.png)
* Tetris2
![Tetris2_1](screenshots/Tetris2_1.png)
![Tetris2_2](screenshots/Tetris2_2.png)
* TheLegendOfZelda
![TheLegendOfZelda_1](screenshots/TheLegendOfZelda_1.png)
![TheLegendOfZelda_2](screenshots/TheLegendOfZelda_2.png)
* ThunderAndLightning
![ThunderAndLightning_1](screenshots/ThunderAndLightning_1.png)
![ThunderAndLightning_2](screenshots/ThunderAndLightning_2.png)
* WariosWoods
![WariosWoods_1](screenshots/WariosWoods_1.png)
![WariosWoods_2](screenshots/WariosWoods_2.png)
![WariosWoods_3](screenshots/WariosWoods_3.png)
![WariosWoods_4](screenshots/WariosWoods_4.png)
* WreckingCrew
![WreckingCrew_1](screenshots/WreckingCrew_1.png)
![WreckingCrew_2](screenshots/WreckingCrew_2.png)
* Zelda2TheAdventureOfLink
![Zelda2TheAdventureOfLink_1](screenshots/Zelda2TheAdventureOfLink_1.png)
![Zelda2TheAdventureOfLink_2](screenshots/Zelda2TheAdventureOfLink_2.png)
![Zelda2TheAdventureOfLink_3](screenshots/Zelda2TheAdventureOfLink_3.png)