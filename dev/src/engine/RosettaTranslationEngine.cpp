#include "engine/RosettaTranslationEngine.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "utils/Macros.hpp"
#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/LiefBinaryLoader.hpp"
#include "frontend/ZydisDecoder.hpp"
#include "frontend/Cli11OptionParser.hpp"
#include "frontend/CfgBuilder.hpp"
#include "frontend/SsaVariables.hpp"
#include "frontend/SsaStatement.hpp"

namespace FrontEnd = rosetta::frontend;
using OptionParser = rosetta::frontend::Cli11OptionParser;
using rosetta::frontend::loader::LiefBinaryParser;
using rosetta::frontend::loader::Architecture;
using rosetta::frontend::decoder::ZydisInstructionDecoder;
using rosetta::frontend::instruction::Instruction;


void RosettaTranslationEngine::ParseConfigurations(
    int argc, char* argv[]) {

    std::unique_ptr<FrontEnd::OptionParserInterface> parser =
        std::make_unique<OptionParser>();

    std::string stage;

    parser->AddOption("-i,--input", cnf_.InputFile, true, "Input file");
    parser->AddOption("-o,--output", cnf_.OutputFile, true, "Output file");

    parser->AddOption(
        "-d,--dump-input-instructions",
        cnf_.DumpInputInstructions,
        false,
        "Dump input instructions");

    parser->AddOption(
        "-s,--stop-after",
        stage,
        false,
        "Pipeline stage. stop after {loader, decoder, ssa, all}");

    parser->AddOption(
        "--dump-ssa",
        cnf_.DumpSsa,
        false,
        "Dump SSA");

    parser->Parse(argc, argv);

    if (stage.empty()) {
        cnf_.PipelineStage = FrontEnd::PipeLineStage::kAll;
    } else if (stage == "loader") {
        cnf_.PipelineStage = FrontEnd::PipeLineStage::kLoader;
    } else if (stage == "decoder") {
        cnf_.PipelineStage = FrontEnd::PipeLineStage::kDecoder;
    } else if (stage == "ssa") {
        cnf_.PipelineStage = FrontEnd::PipeLineStage::kSsa;
    } else {
        parser->PrintHelp();
        exit(1);
    }
}

int RosettaTranslationEngine::RunFrontEnd() {
  int status = Load();

  if (status != 0) {
    return status;
  }

  if (cnf_.PipelineStage ==
      FrontEnd::PipeLineStage::kLoader) {
    std::cout
        << "[Loader] Successfully loaded executable section at 0x"
        << std::hex
        << section_->VirtualAddress
        << std::dec
        << " ("
        << section_->Data.size()
        << " bytes)\n";

    return 0;
  }

  std::vector<Instruction> instructions = Decode();

  if (cnf_.PipelineStage ==
      FrontEnd::PipeLineStage::kDecoder) {
    return 0;
  }

  FrontEnd::cfg::CfgBuilder cfg_builder(instructions);
  FrontEnd::cfg::ControlFlowGraph graph =
      cfg_builder.Build();

  FrontEnd::ssa::SsaVariables ssa(graph);
  ssa.Build();

  if (cnf_.DumpSsa) {
    DumpSsa(ssa);
  }

  if (cnf_.PipelineStage ==
      FrontEnd::PipeLineStage::kSsa) {
    return 0;
  }

  return 0;
}

int RosettaTranslationEngine::Load() {
    std::unique_ptr<FrontEnd::loader::IBinaryParser> parser =
        std::make_unique<LiefBinaryParser>(cnf_.InputFile);

    if (parser->GetArchitecture() != Architecture::kX86_64) {
        std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
        return 1;
    }

    section_ = parser->GetExecutableCode();

    if (section_->Data.empty()) {
        std::cerr << "Error: No executable section found.\n";
        return 1;
    }

    return 0;
}

std::vector<Instruction> RosettaTranslationEngine::Decode() {
  std::unique_ptr<FrontEnd::decoder::IDecoder> decoder =
      std::make_unique<ZydisInstructionDecoder>();

  std::vector<Instruction> instructions = decoder->DecodeAll(
      section_->VirtualAddress, section_->Data.data(), section_->Data.size());

  const uint64_t decoded_bytes = instructions.empty() ? 0 :
      (instructions.back().Address() + instructions.back().Size()
       - section_->VirtualAddress);

  if (decoded_bytes != section_->Data.size()) {
    std::cerr << "Error: Unable to decode instruction at VMA 0x"
               << std::hex << (section_->VirtualAddress + decoded_bytes) << "\n";
    UNREACHABLE("Decode failed to consume entire section");
  }

  if (cnf_.DumpInputInstructions) {
    for (const auto& instruction : instructions) {
      std::cout << instruction.AssemblyText() << "\n";
    }
  }

  return instructions;
}

void RosettaTranslationEngine::DumpSsa(
    const FrontEnd::ssa::SsaVariables& ssa) const {

  auto print_operand =
      [](const FrontEnd::ssa::SsaOperand& operand) {
        using OperandType =
            FrontEnd::ssa::SsaOperandType;

        if (operand.Type == OperandType::kVirtualReg) {
          std::cout << "%v" << operand.Value;
        } else if (operand.Type == OperandType::kConstant) {
          std::cout << operand.Value;
        } else if (operand.Type == OperandType::kLabel) {
          std::cout << "label";
        } else {
          std::cout << "?";
        }
      };

  for (const auto& [address, block] : ssa.Blocks()) {
    std::cout << "block 0x"
              << std::hex
              << address
              << std::dec
              << ":\n";

    for (const auto& statement :
         block.StatementList()) {

      std::cout << "  ";

      if (statement.HasResult()) {
        const auto result = statement.Result();

        if (result.Type ==
            FrontEnd::ssa::SsaOperandType::kVirtualReg) {
          std::cout << "%v"
                    << result.Value
                    << " = ";
        }
      }

      switch (statement.Opcode()) {
        case FrontEnd::ssa::SsaOpcode::kConst:
          std::cout << "const ";
          print_operand(statement.Operands()[0]);
          break;

        case FrontEnd::ssa::SsaOpcode::kMove:
          std::cout << "move ";
          print_operand(statement.Operands()[0]);
          break;

        case FrontEnd::ssa::SsaOpcode::kSub:
          std::cout << "sub ";
          print_operand(statement.Operands()[0]);
          std::cout << ", ";
          print_operand(statement.Operands()[1]);
          break;

        case FrontEnd::ssa::SsaOpcode::kMul:
          std::cout << "mul ";
          print_operand(statement.Operands()[0]);
          std::cout << ", ";
          print_operand(statement.Operands()[1]);
          break;

        case FrontEnd::ssa::SsaOpcode::kSle:
          std::cout << "sle ";
          print_operand(statement.Operands()[0]);
          std::cout << ", ";
          print_operand(statement.Operands()[1]);
          break;

        case FrontEnd::ssa::SsaOpcode::kSgt:
          std::cout << "sgt ";
          print_operand(statement.Operands()[0]);
          std::cout << ", ";
          print_operand(statement.Operands()[1]);
          break;

        case FrontEnd::ssa::SsaOpcode::kPhi:
          std::cout << "phi";

          for (const auto& incoming :
               statement.PhiIncomingList()) {
            std::cout << " [0x"
                      << std::hex
                      << incoming.PredecessorBlock
                      << std::dec
                      << ": ";

            print_operand(incoming.Value);

            std::cout << "]";
          }
          break;

        case FrontEnd::ssa::SsaOpcode::kCondBr:
          std::cout << "condbr ";
          print_operand(statement.Operands()[0]);

          std::cout << ", 0x"
                    << std::hex
                    << statement.TrueTarget();

          std::cout << ", 0x"
                    << statement.FalseTarget()
                    << std::dec;
          break;

        case FrontEnd::ssa::SsaOpcode::kBr:
          std::cout << "br 0x"
                    << std::hex
                    << statement.TrueTarget()
                    << std::dec;
          break;

        case FrontEnd::ssa::SsaOpcode::kIndirectBr:
          std::cout << "indirectbr ";
          print_operand(statement.Operands()[0]);
          break;

        case FrontEnd::ssa::SsaOpcode::kLoadGuestStackReturnAddress:
          std::cout << "load_guest_return_address";
          break;

        case FrontEnd::ssa::SsaOpcode::kRet:
          std::cout << "ret";
          break;

        case FrontEnd::ssa::SsaOpcode::kGuestPcMarker:
          std::cout << "guest_pc";
          break;

        default:
          std::cout << "unknown";
          break;
      }

      std::cout << "\n";
    }
  }
}

int RosettaTranslationEngine::Run(int argc, char* argv[]) {
    ParseConfigurations(argc, argv);

    int status = RunFrontEnd();
    if (status != 0) {
        return status;
    }

    if (cnf_.PipelineStage == FrontEnd::PipeLineStage::kLoader ||
        cnf_.PipelineStage == FrontEnd::PipeLineStage::kDecoder ||
        cnf_.PipelineStage == FrontEnd::PipeLineStage::kSsa) {
        return 0;
    }

    status = Optimize();
    if (status != 0) {
        return status;
    }

    return Generate();
}

int RosettaTranslationEngine::Optimize() {
    // TODO(salah): Implement optimization passes
    return 0;
}

int RosettaTranslationEngine::Generate() {
    // TODO(salah): Implement code generation
    return 0;
}
