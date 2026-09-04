#include "Decoder.hpp"

using rosetta::frontend::decode::Decoder;
using rosetta::frontend::decode::X86Instruction;

Decoder::Decoder() {
    // Decode 64-bit x86 instructions and render them in Intel syntax.
    ZydisDecoderInit(&z_decoder, ZYDIS_MACHINE_MODE_LONG_64,
                     ZYDIS_STACK_WIDTH_64);
    ZydisFormatterInit(&z_formatter,
                       ZYDIS_FORMATTER_STYLE_INTEL);
}

std::optional<X86Instruction> Decoder::decode(
    uint64_t vma, const uint8_t* buffer, size_t length) {
    if (!buffer || !length) {
        return std::nullopt;
    }

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    if (!ZYAN_SUCCESS(
            ZydisDecoderDecodeFull(&z_decoder, buffer, length, &instruction,
                                   operands))) {
        return std::nullopt;
    }

    char formatted[256];
    if (!ZYAN_SUCCESS(ZydisFormatterFormatInstruction(
            &z_formatter, &instruction, operands,
            instruction.operand_count_visible, formatted, sizeof(formatted),
            vma, ZYAN_NULL))) {
        return std::nullopt;
    }

    return X86Instruction(vma, instruction, operands, formatted);
}
