#include "msp430.h"

#include "consts.h"

#include <stdexcept>
#include <fstream>


Msp430::Msp430() :
	_use_cg(false),
	_const(0)
{
	_last_PC = 0;
	_cpu_regs.PC = 0xC000;
}


Msp430::~Msp430()
{
}


size_t file_length(std::ifstream& i_fw)
{
	size_t length = 0;

	i_fw.seekg(0, i_fw.end);
	length = (size_t)i_fw.tellg();
	i_fw.seekg(0, i_fw.beg);

	return length;
}


int32_t Msp430::load_fw(const std::string & fw_filename)
{
	size_t length;
	std::ifstream i_fw(fw_filename, std::ios::binary);

	if (!i_fw.is_open())
		throw std::runtime_error("failed to open firmware file");

	length = file_length(i_fw);

	if (length > _memspace.code_size())
	{
		i_fw.close();
		throw std::length_error("firmware size is greater than code section size");
	}

	i_fw.read((char *)_memspace.code_ptr(), length);

	i_fw.close();

	return 0;
}


bool Msp430::step(void)
{
	if (_last_PC == _cpu_regs.PC) return _is_running = false;
	
	return _is_running = decode(fetch());
}


inline bool Msp430::run_cg(const uint8_t reg, const uint8_t as)
{
	switch (reg)
	{
	case 2:
		switch (as)
		{
		case 0b10:
			_const = 4;
			return true;

		case 0b11:
			_const = 8;
			return true;

		default:
			return false;
		}
		break;

	case 3:
		switch (as)
		{
		case 0b00:
			_const = 0;
			return true;

		case 0b01:
			_const = 1;
			return true;

		case 0b10:
			_const = 2;
			return true;

		case 0b11:
			_const = -1;
			return true;

		default:
			return false;
		}
		break;

	default:
		return false;
	}

	return false;
}


uint16_t Msp430::fetch()
{
	uint16_t word, *ptr;

	ptr = _memspace.addr_ptr(_cpu_regs.PC);
	word = *ptr;

	_last_PC = _cpu_regs.PC;
	_cpu_regs.PC += 2;

	return word;
}


bool Msp430::decode(const uint16_t instr)
{
	DualOperand_t dop;
	SingleOperand_t sop;
	Jumps_t jmp;

	dop._instr = instr;
	sop._instr = instr;
	jmp._instr = instr;

	_use_cg = false;
	_const = 0;

	switch (instr & FORMAT_MASK)
	{
	case 0x1000:
		return formatII(sop);

	case 0x2000:
	case 0x3000:
		return formatJ(jmp);

	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
	case 0x8000:
	case 0x9000:
	case 0xA000:
	case 0xB000:
	case 0xC000:
	case 0xD000:
	case 0xE000:
	case 0xF000:
		return formatI(dop);
	}

	return false;
}


inline uint16_t* Msp430::get_reg_ptr(const uint8_t reg)
{
	if (reg >= 0 && reg < _cpu_regs._count)
		return (_cpu_regs.R + reg);

	return nullptr;
}


inline bool Msp430::formatI(DualOperand_t dop)
{
	uint16_t *src = nullptr, *dst = nullptr;

	uint16_t *s_reg = nullptr, *d_reg = nullptr;

	int16_t src_val = 0, src_ofst = 0;
	int16_t dst_ofst = 0;
	uint16_t *dst_addr = nullptr;

	s_reg = get_reg_ptr(dop.s_reg);
	d_reg = get_reg_ptr(dop.d_reg);

	if (((dop.s_reg == 2) && (dop.as > 1)) || dop.s_reg == 3)
	{
		_use_cg = true;
		run_cg(dop.s_reg, dop.as);
	}

	switch (dop._instr & AD_AS_MASK)
	{
	case AD_0_AS_00:
		if (_use_cg)
			src_val = _const;
		else
			src_val = *s_reg;

		dst_addr = d_reg;
		break;

	case AD_0_AS_01:
		if (_use_cg)
			src_val = _const;
		else
		{
			src_ofst = fetch();

			if (dop.s_reg == 0)
				src_val = *_memspace.addr_ptr(*s_reg + src_ofst - 2);
			else if (dop.s_reg == 2)
				src_val = *_memspace.addr_ptr(src_ofst);
			else
				src_val = *_memspace.addr_ptr(*s_reg + src_ofst);
		}

		dst_addr = d_reg;
		break;

	case AD_0_AS_10:
		if (_use_cg)
			src_val = _const;
		else
			src_val = *_memspace.addr_ptr(*s_reg);

		dst_addr = d_reg;
		break;

	case AD_0_AS_11:
		if (_use_cg)
			src_val = _const;
		else if (dop.s_reg == 0)
			src_val = fetch();
		else
		{
			src_val = *_memspace.addr_ptr(*s_reg);

			dop.bw ? (*s_reg += 1) : *s_reg += 2;
		}

		dst_addr = d_reg;
		break;

	case AD_1_AS_00:
		if (_use_cg)
			src_val = _const;
		else
			src_val = *s_reg;

		dst_ofst = fetch();

		if (dop.d_reg == 0)
			dst_addr = _memspace.addr_ptr(*d_reg + dst_ofst - 2);
		else if (dop.d_reg == 2)
			dst_addr = _memspace.addr_ptr(dst_ofst);
		else
			dst_addr = _memspace.addr_ptr(*d_reg + dst_ofst);
		break;

	case AD_1_AS_01:
		if (_use_cg)
			src_val = _const;
		else
		{
			src_ofst = fetch();

			if (dop.s_reg == 0)
				src_val = *_memspace.addr_ptr(_cpu_regs.PC + src_ofst - 4);
			else if (dop.s_reg == 2)
				src_val = *_memspace.addr_ptr(src_ofst);
			else
				src_val = *_memspace.addr_ptr(*s_reg + src_ofst);
		}
		
		dst_ofst = fetch();

		if (dop.d_reg == 0)
			dst_addr = _memspace.addr_ptr(_cpu_regs.PC + dst_ofst - 2);
		else if (dop.d_reg == 2)
			dst_addr = _memspace.addr_ptr(dst_ofst);
		else
			dst_addr = _memspace.addr_ptr(*d_reg + dst_ofst);
		break;

	case AD_1_AS_10:
		if (_use_cg)
			src_val = _const;
		else
			src_val = *_memspace.addr_ptr(*s_reg);

		dst_ofst = fetch();

		if (dop.d_reg == 0)
			dst_addr = _memspace.addr_ptr(_cpu_regs.PC + dst_ofst - 2);
		else if (dop.d_reg == 2)
			dst_addr = _memspace.addr_ptr(dst_ofst);
		else
			dst_addr = _memspace.addr_ptr(*d_reg + dst_ofst);
		break;

	case AD_1_AS_11:
		if (_use_cg)
			src_val = _const;
		else if (dop.s_reg == 0)
			src_val = fetch();
		else
		{
			src_val = *_memspace.addr_ptr(*s_reg);

			dop.bw ? (*s_reg += 1) : (*s_reg += 2);
		}

		dst_ofst = fetch();

		if (dop.d_reg == 0)
			dst_addr = _memspace.addr_ptr(_cpu_regs.PC + dst_ofst - 2);
		else if (dop.d_reg == 2)
			dst_addr = _memspace.addr_ptr(dst_ofst);
		else
			dst_addr = _memspace.addr_ptr(*d_reg + dst_ofst);
		break;
	}

	InstructionLenght_t i_len = dop.bw ? BYTE : WORD;

	src = (uint16_t*)&src_val;
	dst = dst_addr;

	switch (dop._instr & FORMAT_1_MASK)
	{

	case OP_CODE_MOV:
		return MOV(src, dst, i_len);

	case OP_CODE_ADD:
		return ADD(src, dst, i_len);

	case OP_CODE_ADDC:
		return ADDC(src, dst, i_len);

	case OP_CODE_SUBC:
		return SUBC(src, dst, i_len);

	case OP_CODE_SUB:
		return SUB(src, dst, i_len);

	case OP_CODE_CMP:
		return CMP(src, dst, i_len);

	case OP_CODE_DADD:
		return DADD(src, dst, i_len);

	case OP_CODE_BIT:
		return BIT(src, dst, i_len);

	case OP_CODE_BIC:
		return BIC(src, dst, i_len);

	case OP_CODE_BIS:
		return BIS(src, dst, i_len);

	case OP_CODE_XOR:
		return XOR(src, dst, i_len);

	case OP_CODE_AND:
		return AND(src, dst, i_len);
	}

	return false;
}


inline bool Msp430::formatII(SingleOperand_t sop)
{
	uint16_t *src = nullptr;

	uint16_t *reg = nullptr;

	int16_t src_val = 0, src_ofst = 0;
	uint16_t *src_addr = nullptr;

	reg = _memspace.addr_ptr(sop.ds_reg);

	if (((sop.ds_reg == 2) && (sop.ad > 1)) || sop.ds_reg == 3)
	{
		_use_cg = true;
		run_cg(sop.ds_reg, sop.ad);
	}

	switch (sop._instr & AD_MASK)
	{
	case AD_00:
		if (_use_cg)
		{
			src_val = _const;
			src_addr = (uint16_t*)&src_val;
		}
		else
		{
			src_val = *reg;
			src_addr = reg;
		}
		break;

	case AD_01:
		src_ofst = fetch();

		if (_use_cg)
		{
			src_val = _const;
			src_addr = (uint16_t*)&src_val;
		}
		else if (sop.ds_reg == 0)
			src_addr = _memspace.addr_ptr(_cpu_regs.PC + src_ofst);
		else if (sop.ds_reg == 2)
		{
			src_addr = _memspace.addr_ptr(src_ofst);
			src_val = *src_addr;
		}
		else
		{
			src_addr = _memspace.addr_ptr(*reg + src_ofst);
			src_val = *src_addr;
		}
		break;

	case AD_10:
		if (_use_cg)
		{
			src_val = _const;
			src_addr = (uint16_t*)&src_val;
		}
		else
		{
			src_addr = _memspace.addr_ptr(*reg);
			src_val = *src_addr;
		}
		break;

	case AD_11:
		if (_use_cg)
		{
			src_val = _const;
			src_addr = (uint16_t*)&src_val;
		}
		else if (sop.ds_reg == 0)
		{
			src_val = fetch();
			src_addr = (uint16_t*)&src_val;
		}
		else
		{
			src_addr = _memspace.addr_ptr(*reg);
			src_val = *src_addr;
			
			sop.bw ? (*reg += 1) : *reg += 2;
		}
		break;
	}

	InstructionLenght_t i_len = sop.bw ? BYTE : WORD;
	src = src_addr;

	switch (sop._instr & FORMAT_2_MASK)
	{
	case OP_CODE_RRC:
	case OP_CODE_RRC_B:
		return RRC(src, i_len);

	case OP_CODE_SWPB:
		return SWPB(src, i_len);

	case OP_CODE_RRA:
	case OP_CODE_RRA_B:
		return RRA(src, i_len);

	case OP_CODE_SXT:
		return SXT(src, i_len);

	case OP_CODE_PUSH:
	case OP_CODE_PUSH_B:
		return PUSH(src, i_len);

	case OP_CODE_CALL:
		return CALL(src, i_len);

	case OP_CODE_RETI:
		return RETI(src, i_len);
	}

	return false;
}


inline bool Msp430::formatJ(Jumps_t jmp)
{
	int16_t offset = 0;
	bool is_neg = false;

	offset = jmp.pc_offset * 2;

	is_neg = static_cast<bool>((offset >> 9) & 1);
	if (is_neg)
		offset |= JMP_OFFSET_NEG_MASK;

	switch (jmp._instr & FORMAT_J_MASK)
	{
	case OP_CODE_JNE_JNZ:
		return JNE_JNZ(offset);

	case OP_CODE_JEQ_JZ:
		return JEQ_JZ(offset);

	case OP_CODE_JNC:
		return JNC(offset);

	case OP_CODE_JC:
		return JC(offset);

	case OP_CODE_JN:
		return JN(offset);

	case OP_CODE_JGE:
		return JGE(offset);

	case OP_CODE_JL:
		return JL(offset);

	case OP_CODE_JMP:
		return JMP(offset);
	}

	return false;
}


inline bool Msp430::MOV(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	const uint16_t mask = (1 << i_len) - 1;
	
	*dst = *src & mask;

	return true;
}


static inline bool is_v(const uint32_t val, const InstructionLenght_t i_len, uint16_t *src, uint16_t *dst)
{
	const uint16_t OF_MASK = (1 << (i_len - 1));

	return ((*src & OF_MASK) == (*dst & OF_MASK)) && ((val & OF_MASK) != (*dst & OF_MASK));
}


inline uint16_t Msp430::cnz(const uint32_t val, const InstructionLenght_t i_len)
{
	uint16_t res = val & ((1 << i_len) - 1);

	_cpu_regs.SR.C = val >> i_len;
	_cpu_regs.SR.N = (val >> (i_len - 1)) & 1;
	_cpu_regs.SR.Z = res == 0;

	return res;
}


inline bool Msp430::ADD(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	_cpu_regs.SR.C = 0;

	return ADDC(src, dst, i_len);
}


inline bool Msp430::ADDC(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	uint8_t *dst8 = (uint8_t*)dst, *src8 = (uint8_t*)src;

	uint32_t val;
	uint16_t mask = (1 << i_len) - 1;

	val = (*dst & mask) + (*src & mask) + _cpu_regs.SR.C;
	_cpu_regs.SR.V = is_v(val, i_len, src, dst);
	*dst = cnz(val, i_len);

	return true;
}


inline bool Msp430::SUB(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	_cpu_regs.SR.C = 1;

	return SUBC(src, dst, i_len);
}


inline bool Msp430::SUBC(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	uint16_t src_val = ~(*src);

	uint32_t val;
	uint16_t mask = (1 << i_len) - 1;
	
	val = (*dst & mask) + (src_val & mask) + _cpu_regs.SR.C;
	_cpu_regs.SR.V = is_v(val, i_len, src, dst);
	*dst = cnz(val, i_len);

	return true;
}


inline bool Msp430::CMP(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	uint16_t dst_ = *dst;

	_cpu_regs.SR.C = 1;

	return SUBC(src, &dst_, i_len);
}


inline bool Msp430::DADD(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	uint32_t val;

	val = (*src & 0xF) + (*dst & 0xF);
	if (val > 9) val += 6;

	val += (*src & 0xF0) + (*dst & 0xF0);
	if (val > 0x90) val += 0x60;

	if (i_len == WORD)
	{
		val += (*src & 0xF00) + (*dst & 0xF00);
		if (val > 0x900) val += 0x600;

		val += (*src & 0xF000) + (*dst & 0xF000);
		if (val > 0x9000) val += 0x6000;
	}

	*dst = cnz(val, i_len);

	return true;
}


inline bool Msp430::BIT(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	const uint16_t mask = (1 << i_len) - 1;
	
	uint16_t res = *dst & *src & mask;
	
	_cpu_regs.SR.N = res >> (i_len - 1);
	_cpu_regs.SR.Z = res == 0;
	_cpu_regs.SR.C = !_cpu_regs.SR.Z;
	_cpu_regs.SR.V = 0;

	return true;
}


inline bool Msp430::BIC(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	const uint16_t mask = (1 << i_len) - 1;
	
	*dst &= ~(*src) & mask;

	return true;
}


inline bool Msp430::BIS(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	const uint16_t mask = (1 << i_len) - 1;

	*dst |= *src;
	*dst &= mask;

	return true;
}


inline bool Msp430::XOR(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{	
	_cpu_regs.SR.V = ((*src & *dst) >> (i_len - 1)) & 1;
	*dst ^= *src;
	*dst &= (1 << i_len) - 1;
	_cpu_regs.SR.N = *dst >> (i_len - 1);
	_cpu_regs.SR.Z = dst == 0;
	_cpu_regs.SR.C = !_cpu_regs.SR.Z;

	return true;
}


inline bool Msp430::AND(uint16_t *src, uint16_t *dst, const InstructionLenght_t i_len)
{
	_cpu_regs.SR.V = 0;
	*dst &= *src;
	*dst &= (1 << i_len) - 1;
	_cpu_regs.SR.N = *dst >> (i_len - 1);
	_cpu_regs.SR.Z = dst == 0;
	_cpu_regs.SR.C = !_cpu_regs.SR.Z;

	return true;
}


inline bool Msp430::RRC(uint16_t *src, const InstructionLenght_t i_len)
{
	uint8_t nC = *src & 1;

	*src = (_cpu_regs.SR.C << (i_len - 1)) | (*src >> 1);
	_cpu_regs.SR.C = nC;
	_cpu_regs.SR.N = *src >> (i_len - 1);
	_cpu_regs.SR.Z = *src == 0;
	_cpu_regs.SR.V = 0;

	return true;
}


inline bool Msp430::RRA(uint16_t *src, const InstructionLenght_t i_len)
{
	_cpu_regs.SR.C = *src & 1;

	*src = (*src & (1 << (i_len - 1))) | (*src >> 1);
	_cpu_regs.SR.N = *src >> (i_len - 1);
	_cpu_regs.SR.Z = *src == 0;
	_cpu_regs.SR.V = 0;

	return true;
}


inline bool Msp430::PUSH(uint16_t *src, const InstructionLenght_t i_len)
{
	_cpu_regs.SP -= 2;
	
	*_memspace.stack_ptr(_cpu_regs.SP) = *src & ((1 << i_len) - 1);

	return true;
}


inline bool Msp430::SWPB(uint16_t *src, const InstructionLenght_t i_len)
{
	*src = (*src << 8) | (*src >> 8);

	return true;
}


inline bool Msp430::CALL(uint16_t *src, const InstructionLenght_t i_len)
{
	_cpu_regs.SP -= 2;
	*_memspace.stack_ptr(_cpu_regs.SP) = _cpu_regs.PC;
	_cpu_regs.PC = *src;

	return true;
}


inline bool Msp430::RETI(uint16_t *src, const InstructionLenght_t i_len)
{
	_cpu_regs.SR_ = *_memspace.stack_ptr(_cpu_regs.SP);
	_cpu_regs.SP += 2;

	_cpu_regs.PC = *_memspace.stack_ptr(_cpu_regs.SP);
	_cpu_regs.SP += 2;

	return true;
}


inline bool Msp430::SXT(uint16_t *src, const InstructionLenght_t i_len)
{
	*src |= ((*src & 0x80) ? 0xFF00 : 0x0000);

	_cpu_regs.SR.N = *src >> 15;
	_cpu_regs.SR.Z = *src == 0;
	_cpu_regs.SR.C = !_cpu_regs.SR.Z;
	_cpu_regs.SR.V = 0;

	return true;
}


inline bool Msp430::JEQ_JZ(const int16_t offset)
{
	if (_cpu_regs.SR.Z)
		_cpu_regs.PC += offset;
	return true;
}


inline bool Msp430::JNE_JNZ(const int16_t offset)
{
	if (!_cpu_regs.SR.Z)
		_cpu_regs.PC += offset;

	return true;
}


inline bool Msp430::JC(const int16_t offset)
{
	if(_cpu_regs.SR.C)
		_cpu_regs.PC += offset;

	return true;
}


inline bool Msp430::JNC(const int16_t offset)
{
	if(!_cpu_regs.SR.C)
		_cpu_regs.PC += offset;

	return true;
}


inline bool Msp430::JN(const int16_t offset)
{
	if(_cpu_regs.SR.N)
		_cpu_regs.PC += offset;

	return true;
}


inline bool Msp430::JGE(const int16_t offset)
{
	if((_cpu_regs.SR.N ^ _cpu_regs.SR.V) == 0)
		_cpu_regs.PC += offset;

	return true;
}


inline bool Msp430::JL(const int16_t offset)
{
	if((_cpu_regs.SR.N ^ _cpu_regs.SR.V) == 1)
		_cpu_regs.PC += offset;

	return true;
}


inline bool Msp430::JMP(const int16_t offset)
{
	_cpu_regs.PC += offset;

	return true;
}
