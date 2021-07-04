#ifndef DRAKO_INTERPRETER_HPP
#define DRAKO_INTERPRETER_HPP

#include <cstdint>
#include <vector>

namespace drako::vm
{
    enum class ExecResult
    {
        SUCCESS,
        COMPILE_ERROR,
        RUNTIME_ERROR
    };

    enum class OpCode : std::underlying_type_t<std::byte>
    {
        NOOP,
        RETURN
    };

    struct Chunk
    {
        std::vector<std::byte> bytecode;
    };

    class Interpreter
    {
    public:

        ExecResult run(const Chunk& c);

    private:
    };

} // namespace drako::vm

#endif // !DRAKO_INTERPRETER_HPP