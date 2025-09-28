#include "s2650.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

const unsigned int START_ADDRESS = 0x440;


S2650::S2650(){
    IAR = START_ADDRESS;
    randByte = std::uniform_int_distribution<uint8_t>(0, 255);
	page_num = 0x00;
	val_t sp = STACK_LEVELS - 1; // Stack pointer starts at the top of the stack
	psu = (psu & 0xF8) | (sp & 0x07); // clear lower 3 bits and set to sp
	for (int i = 0; i <= 0xFF; ++i) {
		table[i] = &S2650::OP_ILLEGAL;
	}


	table[0x00] = &S2650::OP_LODZ;
	table[0x04] = &S2650::OP_LODI;
	table[0x08] = &S2650::OP_LODR;
	table[0x0C] = &S2650::OP_LODA;
	table[0x10] = &S2650::Table10;
	table[0x14] = &S2650::OP_RETC;
	table[0x18] = &S2650::OP_BCTR;
	table[0x1C] = &S2650::OP_BCTA;
	table[0x20] = &S2650::OP_EORZ;
	table[0x24] = &S2650::OP_EORI;
	table[0x28] = &S2650::OP_EORR;
	table[0x2C] = &S2650::OP_EORA;
	table[0x30] = &S2650::OP_REDC;
	table[0x34] = &S2650::OP_RETE;
	table[0x38] = &S2650::OP_BSTR;
	table[0x3C] = &S2650::OP_BSTA;
	table[0x40] = &S2650::Table40;
	table[0x44] = &S2650::OP_ANDI;
	table[0x48] = &S2650::OP_ANDR;
	table[0x4C] = &S2650::OP_ANDA;
	table[0x50] = &S2650::OP_RRR;
	table[0x54] = &S2650::OP_REDE;
	table[0x58] = &S2650::OP_BRNR;
	table[0x5C] = &S2650::OP_BRNA;
	table[0x60] = &S2650::OP_IORZ;
	table[0x64] = &S2650::OP_IORI;
	table[0x68] = &S2650::OP_IORR;
	table[0x6C] = &S2650::OP_IORA;
	table[0x70] = &S2650::OP_REDD;
	table[0x74] = &S2650::Table74;
	table[0x78] = &S2650::OP_BSNR;
	table[0x7C] = &S2650::OP_BSNA;
	table[0x80] = &S2650::OP_ADDZ;
	table[0x84] = &S2650::OP_ADDI;
	table[0x88] = &S2650::OP_ADDR;
	table[0x8C] = &S2650::OP_ADDA;
	table[0x90] = &S2650::Table90;
	table[0x94] = &S2650::OP_DAR;
	table[0x98] = &S2650::Table98;
	table[0x9C] = &S2650::Table9C;
	table[0xA0] = &S2650::OP_SUBZ;
	table[0xA4] = &S2650::OP_SUBI;
	table[0xA8] = &S2650::OP_SUBR;
	table[0xAC] = &S2650::OP_SUBA;
	table[0xB0] = &S2650::OP_WRTC;
	table[0xB4] = &S2650::TableB4;
	table[0xB8] = &S2650::TableB8;
	table[0xBC] = &S2650::TableBC;
	table[0xC0] = &S2650::TableC0;
	table[0xC8] = &S2650::OP_STRR;
	table[0xCC] = &S2650::OP_STRA;
	table[0xD0] = &S2650::OP_RRL;
	table[0xD4] = &S2650::OP_WRTE;
	table[0xD8] = &S2650::OP_BIRR;
	table[0xDC] = &S2650::OP_BIRA;
	table[0xE0] = &S2650::OP_COMZ;
	table[0xE4] = &S2650::OP_COMI;
	table[0xE8] = &S2650::OP_COMR;
	table[0xEC] = &S2650::OP_COMA;
	table[0xF0] = &S2650::OP_WRTD;
	table[0xF4] = &S2650::OP_TMI;
	table[0xF8] = &S2650::OP_BDRR;
	table[0xFC] = &S2650::OP_BDRA;


	for (int i = 0; i <= 0x3; ++i) {
		table10[i] = &S2650::OP_ILLEGAL;
		table40[i] = &S2650::OP_ILLEGAL;
		table74[i] = &S2650::OP_ILLEGAL;
		table90[i] = &S2650::OP_ILLEGAL;
		table9C[i] = &S2650::OP_ILLEGAL;
		tableB4[i] = &S2650::OP_ILLEGAL;
		tableB8[i] = &S2650::OP_ILLEGAL;
		tableBC[i] = &S2650::OP_ILLEGAL;
		tableC0[i] = &S2650::OP_ILLEGAL;
	}
	
	for (int i = 0; i <= 0xF; ++i) {
		table98[i] = &S2650::OP_ILLEGAL;
	}
	table10[0x2] = &S2650::OP_SPSU;
	table10[0x3] = &S2650::OP_SPSL;

	table40[0x0] = &S2650::OP_HALT;
	for (int i = 1; i <= 3; ++i) {
		table40[i] = &S2650::OP_ANDZ;
	}
	table74[0x0] = &S2650::OP_CPSU;
	table74[0x1] = &S2650::OP_CPSL;
	table74[0x2] = &S2650::OP_PPSU;
	table74[0x3] = &S2650::OP_PPSL;

	table90[0x2] = &S2650::OP_LPSU;
	table90[0x3] = &S2650::OP_LPSL;

	for (int i = 0; i <= 1; ++i) {
		table98[i] = &S2650::OP_BCFR;
		table9C[i] = &S2650::OP_BCFA;
	}
	table98[0x2] = &S2650::OP_ZBRR;
	table9C[0x2] = &S2650::OP_BXA;
	
	for (int i = 8; i <= 11; ++i) {
		table98[i] = &S2650::OP_BCFR;
	}

	tableB4[0x2] = &S2650::OP_TPSU;
	tableB4[0x3] = &S2650::OP_TPSL;

	for (int i = 0; i <= 1; ++i) {
		tableB8[i] = &S2650::OP_BSFR;
		tableBC[i] = &S2650::OP_BSFA;
	}
	tableB8[0x2] = &S2650::OP_ZBSR;
	tableBC[0x2] = &S2650::OP_BSXA;

	tableC0[0x0] = &S2650::OP_NOP;
	for (int i = 1; i <= 3; ++i) {
		tableC0[i] = &S2650::OP_STRZ;
	}
}


void S2650::LoadROM(char const* filename)
{
	std::string fname(filename);
	
	// Check file extension
	if (fname.substr(fname.find_last_of(".") + 1) == "hex") {
		LoadHexFile(filename);
	} else {
		LoadBinaryFile(filename);
	}
}

void S2650::LoadBinaryFile(char const* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char* buffer = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i)
		{
			memory0[START_ADDRESS + i] = buffer[i];
		}

		delete[] buffer;
		std::cout << "Loaded binary file: " << filename << " (" << size << " bytes)" << std::endl;
	} else {
		std::cerr << "Failed to open binary file: " << filename << std::endl;
	}
}

void S2650::LoadHexFile(char const* filename)
{
	std::ifstream file(filename);
	std::string line;
	int bytesLoaded = 0;
	
	if (!file.is_open()) {
		std::cerr << "Failed to open hex file: " << filename << std::endl;
		return;
	}
	
	std::cout << "Loading hex file: " << filename << std::endl;
	
	while (std::getline(file, line)) {
		// Skip empty lines and comments
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			continue;
		}
		
		// Remove inline comments
		size_t comment_pos = line.find('#');
		if (comment_pos != std::string::npos) {
			line = line.substr(0, comment_pos);
		}
		
		// Parse hex bytes (space or comma separated)
		std::istringstream iss(line);
		std::string hexByte;
		
		while (iss >> hexByte) {
			// Remove comma if present
			if (!hexByte.empty() && hexByte.back() == ',') {
				hexByte.pop_back();
			}
			
			// Skip if empty after comma removal
			if (hexByte.empty()) continue;
			
			// Convert hex string to byte
			if (hexByte.length() >= 2) {
				try {
					unsigned int value = std::stoul(hexByte, nullptr, 16);
					if (value <= 0xFF) {
						memory0[START_ADDRESS + bytesLoaded] = static_cast<uint8_t>(value);
						bytesLoaded++;
						std::cout << "0x" << std::hex << std::uppercase << value << " ";
					}
				} catch (const std::exception& e) {
					std::cerr << "Invalid hex value: " << hexByte << std::endl;
				}
			}
		}
	}
	
	file.close();
	std::cout << std::endl << "Loaded " << std::dec << bytesLoaded << " bytes from hex file" << std::endl;
}

void S2650::Cycle(){
	// Ensure IAR is within memory bounds
	if (IAR >= sizeof(memory0)) {
		std::cerr << "PC out of bounds: " << IAR << std::endl;
		std::exit(EXIT_FAILURE);
	}
	std::cout << "PC: " << std::hex << IAR << std::endl;
	opcode = memory0[IAR];  // Read single byte opcode first
	if (IAR + 2 < sizeof(memory0)) {
		operand0 = memory0[IAR + 1];  // Read operand0
		operand1 = memory0[IAR + 2];  // Read operand1
	} else {
		operand0 = 0;
		operand1 = 0;
	}
	reg = opcode & 0x3; // Extract reg from opcode bits 2-3
	int8_t signed_offset = operand0 & 0x7F; // Get lower 7 bits
	if (signed_offset & 0x40) { // If bit 6 is set (sign bit for 7-bit value)
		signed_offset |= 0x80; // Sign extend to make it negative (for 8-bit signed)
	}
	offset = (uint8_t)signed_offset;
	abs_addr = ((operand0 << 8) | operand1) & 0x7FFF;
	abs_ic = (operand0 >> 5) & 0x3;
	index_mode = (operand0 >> 7) & 0x1;
	
	bool cc0 = (psl >> PSL_CC0) & 0x1;
	bool cc1 = (psl >> PSL_CC1) & 0x1;
	
	switch(reg) {
		case 0x0: branch_taken = true; break; 
		case 0x1: branch_taken = cc0; break;  
		case 0x2: branch_taken = cc1; break; 
		case 0x3: branch_taken = cc0 && cc1; break;
	}


	std::cout << "Opcode: " << std::hex << (int)opcode << std::endl;
	IAR += 1;  // Move past opcode
	
	uint8_t instruction = opcode & 0xFC;  // 0xFC = 11111100 (upper 6 bits)
	(this->*table[instruction])();  // Execute the opcode
}
void S2650::Table10(){
	(this->*table10[opcode & 0xF])();
}
void S2650::OP_ILLEGAL(){
	std::cerr << "Illegal opcode: " << std::hex << (int)opcode << " at PC: " << (IAR - 1) << std::endl;
	std::exit(EXIT_FAILURE);
}
void S2650::Table40(){
	(this->*table40[opcode & 0xF])();
}
void S2650::Table74(){
	(this->*table74[opcode & 0xF])();
}

void S2650::Table90(){
	(this->*table90[opcode & 0xF])();
}

void S2650::Table98(){
	(this->*table98[opcode & 0xF])();
}

void S2650::Table9C(){
	(this->*table9C[opcode & 0xF])();
}

void S2650::TableB4(){
	(this->*tableB4[opcode & 0xF])();
}

void S2650::TableB8(){
	(this->*tableB8[opcode & 0xF])();
}
void S2650::TableBC(){
	(this->*tableBC[opcode & 0xF])();
}
void S2650::TableC0(){
	(this->*tableC0[opcode & 0xF])();
}


// LOD Operations
void S2650::OP_LODZ(){
	LOD(0,fetchFromRegister());
}
void S2650::OP_LODI(){
	LOD(reg,immediateAddressing());
	IAR += 1; 
}
void S2650::OP_LODR(){
	LOD(reg,relativeAddressing());
	IAR += 1;
}
void S2650::OP_LODA(){
	LOD(reg, absoluteAddressing());
	IAR += 2;
}

void S2650::OP_NOP(){
	// No operation
}	

// STR Operations
void S2650::OP_STRZ(){
	val_t v = fetchFromRegisterZero();
	STR(reg,v);
	setCCBit(v);
}
void S2650::OP_STRR(){
	STR(reg,relativeAddressing());
	IAR += 1;
}
void S2650::OP_STRA(){
	STR(reg, absoluteAddressing());
	IAR += 2;
}

// ADD Operations
void S2650::OP_ADDZ(){
	ADD(reg, registers[0]);
}
void S2650::OP_ADDI(){
	ADD(reg, immediateAddressing());
	IAR += 1;
}
void S2650::OP_ADDR(){
	ADD(0, relativeAddressing());
	IAR += 1;
}
void S2650::OP_ADDA(){
	ADD(reg, absoluteAddressing());
	IAR += 2;
}

// SUB Operations
void S2650::OP_SUBZ(){
	SUB(0, registers[0]);
}
void S2650::OP_SUBI(){
	SUB(reg, immediateAddressing());
	IAR += 1;
}
void S2650::OP_SUBR(){
	SUB(0, relativeAddressing());
	IAR += 1;
}
void S2650::OP_SUBA(){
	SUB(reg, absoluteAddressing());
	IAR += 2;
}


void S2650::OP_HALT(){
	std::cout << "HALT encountered at PC: " << std::hex << (IAR - 1) << std::endl;
	std::exit(EXIT_SUCCESS);
}

// AND Operations
void S2650::OP_ANDZ(){
	AND(0, registers[reg]);
}
void S2650::OP_ANDI(){
	AND(reg, immediateAddressing());
	IAR += 1;
}
void S2650::OP_ANDR(){
	AND(0, relativeAddressing());
	IAR += 1;
}
void S2650::OP_ANDA(){
	AND(reg, absoluteAddressing());
	IAR += 2;
}

// EOR Operations
void S2650::OP_EORZ(){
	EOR(0, registers[reg]);
}
void S2650::OP_EORI(){
	EOR(reg, immediateAddressing());
	IAR += 1;
}
void S2650::OP_EORR(){
	EOR(0, relativeAddressing());
	IAR += 1;
}
void S2650::OP_EORA(){
	EOR(reg, absoluteAddressing());
	IAR += 2;
}

// IOR Operations
void S2650::OP_IORZ(){
	IOR(0, registers[reg]);
}
void S2650::OP_IORI(){
	IOR(reg, immediateAddressing());
	IAR += 1;
}
void S2650::OP_IORR(){
	IOR(0, relativeAddressing());
	IAR += 1;
}
void S2650::OP_IORA(){
	IOR(reg, absoluteAddressing());
	IAR += 2;
}

// COM Operations
void S2650::OP_COMZ(){
	COM(0, immediateAddressing());
	IAR += 1;
}
void S2650::OP_COMI(){
	COM(reg, immediateAddressing());
	IAR += 1;
}
void S2650::OP_COMR(){
	COM(0, relativeAddressing());
	IAR += 1;
}
void S2650::OP_COMA(){
	COM(reg, absoluteAddressing());
	IAR += 2;
}

void S2650::OP_RRL(){
	// Rotate Right Left
	uint8_t x = fetchFromRegister();
	bool msb = (x >> 7) & 0x1;
	uint8_t result = (x << 1) | msb;
	// overflow from bit 7 to bit 0
	bool overflow = (x & 0x80) != 0;
	// if 4th bit is 1, set IDC
	bool idc = (x >> 3) & 0x1;
	STR(reg, result);
	setCCBit(result);
	setCBit(msb);
	setIDCBit(idc);
	setOVFBit(overflow);
}

void S2650::OP_RRR(){
	// Rotate Right Right
	uint8_t x = fetchFromRegister();
	bool lsb = x & 0x1;
	uint8_t result = (x >> 1) | (lsb << 7);
	// overflow from bit 0 to bit 7
	bool overflow = (x & 0x1) != 0;
	// if 4th bit is 1, set IDC
	bool idc = (x >> 3) & 0x1;
	STR(reg, result);
	setCCBit(result);
	setCBit(lsb);
	setIDCBit(idc);
	setOVFBit(overflow);
}

void S2650::OP_LPSU(){
	val_t v = fetchFromRegisterZero();
	// 3,4 bit is always 0
	psl &= 0xE7; // 1110 0111
}
void S2650::OP_LPSL(){
	val_t v = fetchFromRegisterZero();
	psl = v & 0x3F; // 0011 1111
}

void S2650::OP_SPSU(){
	val_t v = fetchFromRegisterZero();
	// 3,4 bit is always 0
	STR(0, v & 0xE7);
	setCCBit(v & 0xE7);
}
void S2650::OP_SPSL(){
	val_t v = fetchFromRegisterZero();
	STR(0, psl);
	setCCBit(v);
}
void S2650::OP_PPSU(){
	IAR += 1;
	// 3,4 bit is always 0
	STR(0, (operand0 & 0xE7) | psu);
}
void S2650::OP_PPSL(){
	IAR += 1;
	STR(0, operand0 | psl);
	setCCBit(operand0 | psl);
}
void S2650::OP_CPSU(){
	val_t v = operand0;
	v = ~v;
	psu &= v;
	IAR += 1;
}
void S2650::OP_CPSL(){
	val_t v = operand0;
	v = ~v;
	psl &= v;
	IAR += 1;
}
void S2650::OP_TPSU(){
	val_t v = operand0;
	val_t temp = v & psu;
	if (temp == v) {
		psl &= ~(1 << PSL_CC0);
		psl &= ~(1 << PSL_CC1);
	} else {
		psl &= ~(1 << PSL_CC0);
		psl |= (1 << PSL_CC1);
	}
	IAR += 1;
}
void S2650::OP_TPSL(){
	val_t v = memory0[IAR];
	val_t temp = v & psl;
	if (temp == v) {
		psl &= ~(1 << PSL_CC0);
		psl &= ~(1 << PSL_CC1);
	} else {
		psl &= ~(1 << PSL_CC0);
		psl |= (1 << PSL_CC1);
	}
	IAR += 1;
}

void S2650::OP_ZBRR(){
	// must jump to page zero
	val_t v = relativeAddressing();
	page_num = 0x00;
	IAR = 0x1FFF + v;
}

void S2650::OP_BCTR(){
	if (branch_taken) {
		IAR = (relativeBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 1;
	}
}

void S2650::OP_BCTA(){
	if (branch_taken) {
		IAR = (absoluteBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 2;
	}
}

void S2650::OP_BCFR(){
	if (!branch_taken) {
		IAR = (relativeBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 1;
	}
}

void S2650::OP_BCFA(){
	if (!branch_taken) {
		IAR = (absoluteBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 2;
	}
}

void S2650::OP_BIRR(){
	val_t i = fetchFromRegister();
	if(++i == 0x00){
		IAR = (relativeAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
	STR(reg,i);
}

void S2650::OP_BIRA(){
	val_t i = fetchFromRegister();
	if(++i == 0x00){
		IAR = (absoluteBranchAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
	STR(reg,i);
}

void S2650::OP_BDRR(){
	val_t i = fetchFromRegister();
	if(--i == 0x00){
		IAR = (relativeAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 1; // Should be 1, not 2, since global increment already happened
	}
	STR(reg,i);
}

void S2650::OP_BDRA(){
	val_t i = fetchFromRegister();
	if(--i == 0x00){
		IAR = (absoluteBranchAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
	STR(reg,i);
}

void S2650::OP_BRNR(){
	val_t i = fetchFromRegister();
	if(i == 0x00){
		IAR = (relativeAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
}

void S2650::OP_BRNA(){
	val_t i = fetchFromRegister();
	if(i == 0x00){
		IAR = (absoluteBranchAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
}

void S2650::OP_BXA(){
	val_t i = fetchFromRegister();
	addr_t addr = abs_addr;
	if (index_mode){
		addr = indirectAddressing(addr);
	}
	IAR = (addr + i) & 0x1FFF;
	page_num = (IAR >> 13) & 0x3;
}

void S2650::OP_ZBSR(){
	// must jump to page zero
	pushRAS();
	val_t v = relativeAddressing();
	IAR = 0x1FFF + v;
	page_num = 0x00;
}

void S2650::OP_BSTR(){
	pushRAS();
	if (branch_taken) {
		IAR = (relativeBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 1;
	}
}

void S2650::OP_BSTA(){
	// Calculate return address (add +2 for 3-byte instruction)
	IAR += 2; // Skip operand0 and operand1
	pushRAS();
	// Subroutine call executed unconditionally
	IAR = (absoluteBranchAddressing() & 0x1FFF) - 1; 
}
void S2650::OP_BSFR(){
	pushRAS();
	if (!branch_taken) {
		IAR = (relativeBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 1;
	}
}
void S2650::OP_BSFA(){
	pushRAS();
	if (!branch_taken) {
		IAR = (absoluteBranchAddressing() & 0x1FFF) - 1; 
	} else {
		IAR += 2;
	}
}
void S2650::OP_BSNR(){
	pushRAS();
	val_t i = fetchFromRegister();
	if(i != 0x00){
		IAR = (relativeAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
	STR(reg,i);
}
void S2650::OP_BSNA(){
	pushRAS();
	val_t i = fetchFromRegister();
	if(i != 0x00){
		IAR = (absoluteAddressing() & 0x1FFF) - 1;
	} else {
		IAR += 2;
	}
	STR(reg,i);
}
void S2650::OP_BSXA(){
	pushRAS();
	val_t i = fetchFromRegister();
	addr_t addr = abs_addr;
	if (index_mode){
		addr = indirectAddressing(addr);
	}
	IAR = (addr + i) & 0x1FFF;
	page_num = (IAR >> 13) & 0x3;
}

void S2650::OP_RETC(){
	if (branch_taken) {
		IAR = popRAS();
	} else {
		IAR += 1;
	}
}

void S2650::OP_RETE(){
	clearInterrupt();
	IAR = popRAS();
}

void S2650::OP_REDD(){
	OPREQ = true;
	MIO = false;
	RWIO = false;
	DCIO = true;
	ENEIO = false;
	val_t v = readDBUS();
	STR(reg, v);
	setCCBit(v);
}
void S2650::OP_REDC(){
	OPREQ = true;
	MIO = false;
	RWIO = false;
	DCIO = false;
	ENEIO = false;
	val_t v = readDBUS();
	STR(reg, v);
	setCCBit(v);
	IAR += 1;
}
void S2650::OP_REDE(){
	OPREQ = true;
	MIO = false;
	RWIO = false;
	ENEIO = true;
	val_t v = readDBUS();
	STR(reg, v);
	setCCBit(v);
	IAR += 1;
}

void S2650::OP_WRTD(){
	OPREQ = true;
	MIO = false;
	RWIO = true;
	DCIO = true;
	ENEIO = true;
	val_t v = fetchFromRegister();
	writeDBUS(v);
}

void S2650::OP_WRTC(){
	OPREQ = true;
	MIO = false;
	RWIO = true;
	DCIO = false;
	ENEIO = false;
	val_t v = fetchFromRegister();
	writeDBUS(v);
}
void S2650::OP_WRTE(){
	OPREQ = true;
	MIO = false;
	RWIO = true;
	DCIO = true;
	ENEIO = false;
	val_t v = fetchFromRegister();
	writeDBUS(v);
	IAR += 1;
}
void S2650::OP_TMI(){
	val_t v = fetchFromRegister();
	val_t mask;
	mask &= v;
	if (v == mask){
		setCCBit(0);
	} else {
		setCCBit(-1);
	}
	IAR += 1;
}

void S2650::OP_DAR(){
    val_t v = fetchFromRegister();
    bool carry = (psl >> PSL_C) & 0x1;
    bool idc = (psl >> PSL_IDC) & 0x1;

    if (!carry && !idc) {
        v += 0xAA;
    } else if (!carry && idc) {
        v += 0xA0;
    } else if (carry && !idc) {
        v += 0x0A;
    }

    STR(reg, v);
}

val_t S2650::readDBUS(){
	val_t v;
	// read from DBUS
	v = (DBUS[7]<<7) | (DBUS[6]<<6) | (DBUS[5]<<5) | (DBUS[4]<<4) |
		(DBUS[3]<<3) | (DBUS[2]<<2) | (DBUS[1]<<1) | (DBUS[0]);
	return v;
}
void S2650::writeDBUS(val_t val){
	// write to DBUS
	DBUS[0] = val & 0x01;
	DBUS[1] = (val >> 1) & 0x01;
	DBUS[2] = (val >> 2) & 0x01;
	DBUS[3] = (val >> 3) & 0x01;	
	DBUS[4] = (val >> 4) & 0x01;
	DBUS[5] = (val >> 5) & 0x01;
	DBUS[6] = (val >> 6) & 0x01;
	DBUS[7] = (val >> 7) & 0x01;	
}

void S2650::selectADR(val_t val){
	ADRBUS[0] = val & 0x01;
	ADRBUS[1] = (val >> 1) & 0x01;
	ADRBUS[2] = (val >> 2) & 0x01;
	ADRBUS[3] = (val >> 3) & 0x01;	
	ADRBUS[4] = (val >> 4) & 0x01;
	ADRBUS[5] = (val >> 5) & 0x01;
	ADRBUS[6] = (val >> 6) & 0x01;
	ADRBUS[7] = (val >> 7) & 0x01;
	ADRBUS[8] = 0;
	ADRBUS[9] = 0;
	ADRBUS[10] = 0;
	ADRBUS[11] = 0;
	ADRBUS[12] = 0;
	ADRBUS[13] = 0;	
}

void S2650::AND(reg_t reg,val_t val){
	uint8_t x = registers[reg];
	uint8_t result = x & val;
	STR(reg, result);
	setCCBit(result);
}

// Common subtraction processing function - sets all flags appropriately
void S2650::SUB(reg_t reg, val_t val){
	// Subtraction implemented as a - b = a + (~b + 1)
	uint8_t a = registers[reg];
	uint8_t b = val;
	uint16_t result = a - b;
	
	// 8-bit result value
	uint8_t result8 = result & 0xFF;
	
	// Borrow flag (carry in subtraction) - when a is smaller than b
	bool borrow = (a < b);
	setCBit(borrow);
	
	// Carry from 3rd to 4th digit (lower nibble)
	uint8_t lower_nibble_a = a & 0x0F;
	uint8_t lower_nibble_b = b & 0x0F;
	bool intermediate_borrow = (lower_nibble_a < lower_nibble_b);
	setIDCBit(intermediate_borrow);
	
	// Overflow flag (signed arithmetic overflow)
	// When operands have different signs and result has same sign as b
	bool sign_a = (a & 0x80) != 0;
	bool sign_b = (b & 0x80) != 0;
	bool sign_result = (result8 & 0x80) != 0;
	bool overflow = (sign_a != sign_b) && (sign_b == sign_result);
	setOVFBit(overflow);
	
	// Store result in register
	STR(reg, result8);
	
	// Condition code bit
	setCCBit(result8);
}
void S2650::EOR(reg_t reg,val_t val){
	uint8_t x = registers[reg];
	uint8_t result = x ^ val;
	STR(reg, result);
	setCCBit(result);
}


void S2650::ADD(reg_t reg, val_t val){
	uint8_t a = registers[reg];
	uint8_t b = val;
	uint16_t result = a + b;
	
	uint8_t result8 = result & 0xFF;
	
	bool carry = (result > 0xFF);
	setCBit(carry);
	
	uint8_t lower_nibble_a = a & 0x0F;
	uint8_t lower_nibble_b = b & 0x0F;
	bool intermediate_carry = ((lower_nibble_a + lower_nibble_b) > 0x0F);
	setIDCBit(intermediate_carry);

	bool sign_a = (a & 0x80) != 0;
	bool sign_b = (b & 0x80) != 0;
	bool sign_result = (result8 & 0x80) != 0;
	bool overflow = (sign_a == sign_b) && (sign_a != sign_result);
	setOVFBit(overflow);
	
	STR(reg, result8);
	setCCBit(result8);
}


void S2650::STR(reg_t reg,val_t val){
	registers[reg] = val;
}

void S2650::IOR(reg_t reg,val_t val){
	uint8_t x = registers[reg];
	uint8_t result = x | val;
	STR(reg, result);
	setCCBit(result);
}

void S2650::LOD(reg_t reg,val_t val){
	registers[reg] = val;
	setCCBit(val);
}

void S2650::COM(reg_t reg,val_t val){
	val_t a = registers[reg];
	val_t b = val;
	
	uint16_t result = a - b;
	uint8_t result8 = result & 0xFF;
	
	bool carry = (a < b);
	setCBit(carry);
	setCCBit(result8);
	
	// Overflow flag
	bool sign_a = (a & 0x80) != 0;
	bool sign_b = (b & 0x80) != 0;
	bool sign_result = (result8 & 0x80) != 0;
	bool overflow = (sign_a != sign_b) && (sign_b == sign_result);
	setOVFBit(overflow);
}

// PSL bit manipulation helpers
void S2650::setCCBit(int8_t value) {
	if (value > 0) {
		psl |= (1 << PSL_CC0);
		psl &= ~(1 << PSL_CC1);
	} else if (value < 0) {
		psl &= ~(1 << PSL_CC0);
		psl |= (1 << PSL_CC1);
	} else {
		psl &= ~(1 << PSL_CC1);
		psl &= ~(1 << PSL_CC0);
	}
}
void S2650::setIDCBit(bool idc) {
	if (idc) {
		psl |= (1 << PSL_IDC);
	} else {
		psl &= ~(1 << PSL_IDC);
	}
}
void S2650::setRSBit(bool rs) {
	if (rs) {
		psl |= (1 << PSL_RS);
	} else {
		psl &= ~(1 << PSL_RS);
	}
}
void S2650::setWCBit(bool wc) {
	if (wc) {
		psl |= (1 << PSL_WC);
	} else {
		psl &= ~(1 << PSL_WC);
	}
}
void S2650::setOVFBit(bool overflow) {
	if (overflow) {
		psl |= (1 << PSL_OVF);
	} else {
		psl &= ~(1 << PSL_OVF);
	}
}

void S2650::setCOMBit(bool com) {
	if (com) {
		psl |= (1 << PSL_COM);
	} else {
		psl &= ~(1 << PSL_COM);
	}
}
void S2650::setCBit(bool carry) {
	if (carry) {
		psl |= (1 << PSL_C);
	} else {
		psl &= ~(1 << PSL_C);
	}
}

val_t S2650::relativeAddressing(){
	addr_t addr = IAR + 1 + (int8_t)offset;
	val_t v;
	if(index_mode) {
		v = indirectAddressing(addr);
	} else {
		v = memoryAddressing(addr);
	}
	return v;
}

val_t S2650::absoluteAddressing(){
	addr_t addr = abs_addr;
	if (index_mode){
		addr = indirectAddressing(addr);
	}
	val_t idx = fetchFromRegister();
	if(abs_ic == 0b00) {
	} else if (abs_ic == 0b01) {
		addr += ++idx & 0xFF;
	} else if (abs_ic == 0b10) {
		addr += --idx & 0xFF;
	} else if (abs_ic == 0b11) {
		addr += idx & 0xFF;
	}
	STR(reg, idx);
}
val_t S2650::memoryAddressing(addr_t addr){
	switch (page_num)
	{
	case 0x00:
		return memory0[addr & 0x1FFF];
	case 0x01:
		return memory1[addr & 0x1FFF];
	case 0x02:
		return memory2[addr & 0x1FFF];
	case 0x03:
		return memory3[addr & 0x1FFF];
	default:
		std::cerr << "Invalid page number: " << std::hex << page_num << " at address: " << std::hex << addr << std::endl;
		std::exit(1);
		return 0; // Return 0 for invalid page
	}
	
}

val_t S2650::fetchFromRegister(){
	return registers[reg & 0x3];
}

val_t S2650::fetchFromRegisterZero(){
	return registers[0];
}
val_t S2650::immediateAddressing(){
	return operand0;
}

addr_t S2650::indirectAddressing(addr_t pc){
	addr_t addr = (pc + 1 + offset) & 0x1FFF;
	page_num = (abs_addr >> 13) & 0x03;
	return memoryAddressing((memoryAddressing(addr) << 8) | memoryAddressing(addr + 1));
}


addr_t S2650::relativeBranchAddressing(){
	addr_t addr = (IAR + 1 + offset) & 0x1FFF;
	if(index_mode) {
		addr = (memoryAddressing(addr) << 8) | memoryAddressing(addr + 1);
	} 
	return addr & 0x1FFF;
}

addr_t S2650::absoluteBranchAddressing(){
	page_num = (abs_addr >> 13) & 0x03;
	addr_t addr = (abs_addr & 0x1FFF);
	if (index_mode)
	{
		addr = (memoryAddressing(addr) << 8) | memoryAddressing(addr + 1);
	}
	return addr & 0x1FFF;
}

void S2650::pushRAS(){
	val_t sp = psu & 0x07; // 8 levels of stack
	if (sp == 0) {
		std::cerr << "Stack overflow on subroutine call at PC: " << std::hex << (IAR - 1) << std::endl;
		std::exit(EXIT_FAILURE);
	}
	RAS[--sp] = (page_num << 13) | (IAR & 0x1FFF);
	// update PSU
	psu = (psu & 0xF8) | (sp & 0x07); 
}

addr_t S2650::popRAS(){
	val_t sp = psu & 0x07; // 8 levels of stack
	if (sp == STACK_LEVELS) {
		std::cerr << "Stack underflow on return from subroutine at PC: " << std::hex << (IAR - 1) << std::endl;
		std::exit(EXIT_FAILURE);
	}
	addr_t addr = RAS[sp++];
	page_num = (addr >> 13) & 0x03;
	// update PSU
	psu = (psu & 0xF8) | (sp & 0x07); 
	return addr & 0x1FFF;
}

void S2650::clearInterrupt(){
	psu &= ~(1 << PSU_INTINH);
}