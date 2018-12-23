#ifndef _MSP430_H_
#define _MSP430_H_

#include "cpu.h"
#include "instructions.h"

class Msp430 :
	public Cpu
{
private:
	bool _use_cg;
	int16_t _const;
	uint16_t _last_PC;

public:
	Msp430();
	virtual ~Msp430();

	virtual int32_t load_fw(const std::string& fw_filename) override;
	virtual bool step(void) override;

protected:
	virtual bool decode(const uint16_t instr) override;
	virtual uint16_t fetch() override;

private:
	inline uint16_t cnz(const uint32_t val, const InstructionLenght_t i_len);

	inline uint16_t* get_reg_ptr(const uint8_t reg);

	inline bool run_cg(const uint8_t reg, const uint8_t as);

	inline bool formatI(DualOperand_t);
	inline bool formatII(SingleOperand_t);
	inline bool formatJ(Jumps_t);

	inline bool MOV(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool ADD(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool ADDC(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool SUB(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool SUBC(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool CMP(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool DADD(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool BIT(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool BIC(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool BIS(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool XOR(uint16_t *src, uint16_t *dst, const InstructionLenght_t);
	inline bool AND(uint16_t *src, uint16_t *dst, const InstructionLenght_t);

	inline bool RRC(uint16_t *src, const InstructionLenght_t);
	inline bool RRA(uint16_t *src, const InstructionLenght_t);
	inline bool PUSH(uint16_t *src, const InstructionLenght_t);
	inline bool SWPB(uint16_t *src, const InstructionLenght_t);
	inline bool CALL(uint16_t *src, const InstructionLenght_t);
	inline bool RETI(uint16_t *src, const InstructionLenght_t);
	inline bool SXT(uint16_t *src, const InstructionLenght_t);

	inline bool JEQ_JZ(const int16_t offset);
	inline bool JNE_JNZ(const int16_t offset);
	inline bool JC(const int16_t offset);
	inline bool JNC(const int16_t offset);
	inline bool JN(const int16_t offset);
	inline bool JGE(const int16_t offset);
	inline bool JL(const int16_t offset);
	inline bool JMP(const int16_t offset);
};

#endif
