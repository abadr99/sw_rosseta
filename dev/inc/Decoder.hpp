#pragma once

#include "Instructions.hpp"
#include <cstdint>
#include <cstddef>
#include <Zydis/Zydis.h>

class Decoder {
private:
    ZydisDecoder z_decoder;

public:
    Decoder(); 
    
    // Returns true if successful, false if the bytes were invalid/garbage
    // The decoded data is pushed into 'out_inst'
    bool decode_instruction(const uint8_t* data, size_t length, X86Instruction& out_inst);
};