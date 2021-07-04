#include "drako/vm/interpreter.hpp"

#include <vector>

namespace drako::vm
{
    ExecResult Interpreter::run(const Chunk& c)
    {
        auto ip = std::data(c.bytecode);
        for (;;)
        {
            const OpCode current = static_cast<OpCode>(*ip);
            switch (current)
            {
                case OpCode::RETURN:
                    return ExecResult::SUCCESS;

                default: // unknown operand code
                    return ExecResult::RUNTIME_ERROR;
            }
        }
    }
}