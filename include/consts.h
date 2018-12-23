#ifndef _CONSTS_H_
#define _CONSTS_H_

#include <cstdint>

constexpr uint16_t FORMAT_MASK			= 0xF000;
constexpr uint16_t FORMAT_1_MASK		= 0xF000;
constexpr uint16_t FORMAT_2_MASK		= 0xFFC0;
constexpr uint16_t FORMAT_J_MASK		= 0xFC00;

//Format 2
constexpr uint16_t OP_CODE_RRC			= 0x1000;
constexpr uint16_t OP_CODE_RRC_B		= 0x1040;
constexpr uint16_t OP_CODE_SWPB			= 0x1080;
constexpr uint16_t OP_CODE_RRA			= 0x1100;
constexpr uint16_t OP_CODE_RRA_B		= 0x1140;
constexpr uint16_t OP_CODE_SXT			= 0x1180;
constexpr uint16_t OP_CODE_PUSH			= 0x1200;
constexpr uint16_t OP_CODE_PUSH_B		= 0x1240;
constexpr uint16_t OP_CODE_CALL			= 0x1280;
constexpr uint16_t OP_CODE_RETI			= 0x1300;

//Format 3
constexpr uint16_t OP_CODE_JNE_JNZ		= 0x2000;
constexpr uint16_t OP_CODE_JEQ_JZ		= 0x2400;
constexpr uint16_t OP_CODE_JNC			= 0x2800;
constexpr uint16_t OP_CODE_JC			= 0x2C00;
constexpr uint16_t OP_CODE_JN			= 0x3000;
constexpr uint16_t OP_CODE_JGE			= 0x3400;
constexpr uint16_t OP_CODE_JL			= 0x3800;
constexpr uint16_t OP_CODE_JMP			= 0x3C00;

//Format 1
constexpr uint16_t OP_CODE_MOV			= 0x4000;
constexpr uint16_t OP_CODE_ADD			= 0x5000;
constexpr uint16_t OP_CODE_ADDC			= 0x6000;
constexpr uint16_t OP_CODE_SUBC			= 0x7000;
constexpr uint16_t OP_CODE_SUB			= 0x8000;
constexpr uint16_t OP_CODE_CMP			= 0x9000;
constexpr uint16_t OP_CODE_DADD			= 0xA000;
constexpr uint16_t OP_CODE_BIT			= 0xB000;
constexpr uint16_t OP_CODE_BIC			= 0xC000;
constexpr uint16_t OP_CODE_BIS			= 0xD000;
constexpr uint16_t OP_CODE_XOR			= 0xE000;
constexpr uint16_t OP_CODE_AND			= 0xF000;

constexpr uint8_t  AD_AS_MASK			= 0xB0;
constexpr uint8_t  AD_0_AS_00			= 0x00;
constexpr uint8_t  AD_0_AS_01			= 0x10;
constexpr uint8_t  AD_0_AS_10			= 0x20;
constexpr uint8_t  AD_0_AS_11			= 0x30;
constexpr uint8_t  AD_1_AS_00			= 0x80;
constexpr uint8_t  AD_1_AS_01			= 0x90;
constexpr uint8_t  AD_1_AS_10			= 0xA0;
constexpr uint8_t  AD_1_AS_11			= 0xB0;

constexpr uint8_t  AD_MASK				= 0x30;
constexpr uint8_t  AD_00				= 0x00;
constexpr uint8_t  AD_01				= 0x10;
constexpr uint8_t  AD_10				= 0x20;
constexpr uint8_t  AD_11				= 0x30;

constexpr uint16_t JMP_OFFSET_MASK		= 0x3FF;
constexpr uint16_t JMP_OFFSET_NEG_MASK	= 0xF800;

#endif
