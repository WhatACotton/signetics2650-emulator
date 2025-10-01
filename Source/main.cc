// #include "Platform.hpp"
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include "s2650.h"
int main(int argc, char** argv)
{
	if (argc != 3 && argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Delay> <ROM> [output_file]\n";
		std::cerr << "  If output_file is specified, JSON will be written to that file instead of stdout\n";
		std::exit(EXIT_FAILURE);
	}

	int cycleDelay = std::stoi(argv[1]);
	char const* romFilename = argv[2];
	char const* outputFilename = (argc == 4) ? argv[3] : nullptr;

    S2650 cpu;

    cpu.LoadROM(romFilename);

	auto lastCycleTime = std::chrono::high_resolution_clock::now();


	while (!cpu.halted)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;
            cpu.Cycle();
			// opcode debug print
			std::cout << "PC: 0x" << std::hex << cpu.IAR << " Opcode: 0x" << std::hex << (int)cpu.opcode;
			std::cout << " R0: 0x" << std::hex << (int)cpu.registers[0];
			std::cout << " R1: 0x" << std::hex << (int)cpu.registers[1];  
			std::cout << " R2: 0x" << std::hex << (int)cpu.registers[2];
			std::cout << " R3: 0x" << std::hex << (int)cpu.registers[3] << std::endl;

		}
	}
	
	// Output register information in JSON format when halted
	std::cout << "\n=== Final Register State (JSON) ===\n";
	if (outputFilename) {
		cpu.OutputRegisterInfoJSON(outputFilename);
		std::cout << "JSON output saved to: " << outputFilename << std::endl;
	} else {
		cpu.OutputRegisterInfoJSON();
	}
	

	return 0;
}
