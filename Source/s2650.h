#pragma once

#include <cstdint>
#include <random>



#define GET_PSL_BIT(cpu, bit) (((cpu)->psl & (1 << (bit))) != 0)

typedef uint8_t val_t;
typedef uint16_t addr_t;
typedef uint8_t reg_t;
typedef uint8_t page_t;

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 8192; // 8KB
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;

const int PSU_SENSE = 0x07;
const int PSU_FLAG  = 0x06;
const int PSU_INTINH = 0x05;
const int PSU_SP2  = 0x02;
const int PSU_SP1  = 0x01;
const int PSU_SP0  = 0x00;

const int PSL_CC1 = 0x07;
const int PSL_CC0 = 0x06;
const int PSL_IDC = 0x05;
const int PSL_RS  = 0x04;
const int PSL_WC  = 0x03;
const int PSL_OVF = 0x02;
const int PSL_COM = 0x01;
const int PSL_C   = 0x00;


class S2650
{
public:
	S2650();
	void LoadROM(char const* filename);
	void LoadBinaryFile(char const* filename);
	void LoadHexFile(char const* filename);
	void Cycle();


	addr_t IAR{};
	addr_t RAS[STACK_LEVELS]{};
	addr_t opcode{};
	uint8_t operand0{}; // First operand byte after opcode
	uint8_t operand1{}; // Second operand byte after opcode
	val_t offset{};   // Offset for relative addressing mode
	addr_t abs_addr{}; // Absolute address for absolute addressing mode
	val_t abs_ic{}; // Absolute index control
	bool index_mode{}; // Indirect mode
	page_t page_num;

    uint8_t memory0[MEMORY_SIZE]{};
	uint8_t memory1[MEMORY_SIZE]{};
	uint8_t memory2[MEMORY_SIZE]{};
	uint8_t memory3[MEMORY_SIZE]{};

	// I/O
	bool ADRBUS[15]{};
	bool OPREQ{};
	bool SENSE{};
	bool ADREN_X{};
	bool RESET{};
	bool INTREQ_X{};
	bool DCIO{};
	bool ENEIO{};
	bool MIO{};
	bool FLAG{};
	bool VCC{};
	bool CLOCK{};
	bool PAUSE_X{};
	bool OPACK_X{};
	bool RUN_WAIT_X{};
	bool INTACK{};
	bool DBUS[8]{};
	bool DBUSEN_X{};
	bool RWIO{};
	bool WRP{};
	bool GND{};


	reg_t reg;
	bool branch_taken{};
	bool unconditional_branch_taken{};

	uint8_t registers[4]{};

	uint8_t psu{};
	uint8_t psl{};

	
private:

	// 0x00
	void OP_LODZ();
	void OP_LODI();
	void OP_LODR();
	void OP_LODA();

	void LOD(reg_t reg,val_t val);

	// // 0x01
	void OP_SPSU();
	void OP_SPSL();

	void OP_RETC();

	void OP_BCTR();
	void OP_BCTA();

	// // 0x02
	void OP_EORZ();
	void OP_EORI();
	void OP_EORR();
	void OP_EORA();

	// // 0x03
	void OP_REDC();
	void OP_RETE();
	void OP_BSTR();
	void OP_BSTA();

	// // 0x04
	void OP_HALT();

	void OP_ANDZ();
	void OP_ANDI();
	void OP_ANDR();
	void OP_ANDA();


	// // 0x05
	void OP_RRR();

	void OP_REDE();

	void OP_BRNR();
	void OP_BRNA();

	// // 0x06
	void OP_IORZ();
	void OP_IORI();
	void OP_IORR();
	void OP_IORA();

	// // 0x07
	void OP_REDD();

	void OP_CPSU();
	void OP_CPSL();

	void OP_PPSU();
	void OP_PPSL();

	void OP_BSNR();
	void OP_BSNA();

	// // 0x08
	void OP_ADDZ();
	void OP_ADDI();
	void OP_ADDR();
	void OP_ADDA();

	// // 0x09
	void OP_LPSU();
	void OP_LPSL();

	void OP_DAR();

	void OP_BCFR();
	void OP_ZBRR();

	void OP_BCFA();
	void OP_BXA();

	// // 0x0A
	void OP_SUBZ();
	void OP_SUBI();
	void OP_SUBR();
	void OP_SUBA();

	// // 0x0B
	void OP_WRTC();

	void OP_TPSU();
	void OP_TPSL();

	void OP_BSFR();
	void OP_ZBSR();

	void OP_BSFA();
	void OP_BSXA();

	// // 0x0C
	void OP_NOP();
	void OP_STRZ();

	void OP_STRR();
	void OP_STRA();


	// // 0x0D
	void OP_RRL();
	void OP_WRTE();

	void OP_BIRR();
	void OP_BIRA();

	// // 0x0E
	void OP_COMZ();
	void OP_COMI();
	void OP_COMR();
	void OP_COMA();

	// // 0x0F
	void OP_WRTD();
	void OP_TMI();

	void OP_BDRR();
	void OP_BDRA();

	// misc
	void OP_ILLEGAL();
	void Table10();
	void Table40();
	void Table74();
	void Table90();
	void Table98();
	void Table9C();
	void TableB4();
	void TableB8();
	void TableBC();
	void TableC0();

	void setCCBit(int8_t value);
	void setIDCBit(bool idc);
	void setOVFBit(bool overflow);
	void setRSBit(bool rs);
	void setWCBit(bool wc);
	void setCBit(bool carry);
	void setCOMBit(bool com);
	
	void ADD(reg_t reg, val_t val);
	void SUB(reg_t reg, val_t val);
	void AND(reg_t reg,val_t val);
	void STR(reg_t reg,val_t val);
	void IOR(reg_t reg,val_t val);
	void EOR(reg_t reg,val_t val);
	void COM(reg_t reg,val_t val);

	val_t fetchFromRegister();
	val_t fetchFromRegisterZero();
	val_t immediateAddressing();
	addr_t indirectAddressing(addr_t pc);
	val_t relativeAddressing();
	val_t memoryAddressing(addr_t addr);
	val_t absoluteAddressing();
	addr_t relativeBranchAddressing();
	addr_t absoluteBranchAddressing();
	void clearInterrupt();
	void pushRAS();
	addr_t popRAS();
	val_t readDBUS();
	void writeDBUS(val_t val);
	void selectADR(val_t val);
	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

	typedef void (S2650::*S2650Func)();
	S2650Func table[0xFF + 1]{};
	S2650Func table10[0x3 + 1]{};
	S2650Func table40[0x3 + 1]{};
	S2650Func table74[0x3 + 1]{};
	S2650Func table90[0x3 + 1]{};
	S2650Func table98[0xF + 1]{};
	S2650Func table9C[0x3 + 1]{};
	S2650Func tableB4[0x3 + 1]{};
	S2650Func tableB8[0x3 + 1]{};
	S2650Func tableBC[0x3 + 1]{};
	S2650Func tableC0[0x3 + 1]{};

};
