#include "Decoder.hpp"

Decoder::Decoder() {
    // Upgraded to 64-bit machine mode and 64-bit stack width
    ZydisDecoderInit(&z_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
}

bool Decoder::decode_instruction(const uint8_t* data, size_t length, X86Instruction& out_inst) {
    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    // Decode the 64-bit bytes
    if (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&z_decoder, data, length, &instruction, operands))) {
        // If successful, populate the passed-in object and return true
        out_inst.populate(instruction, operands);
        return true;
    } 
    
    // If it fails (e.g. invalid opcode), gracefully return false
    return false;
}