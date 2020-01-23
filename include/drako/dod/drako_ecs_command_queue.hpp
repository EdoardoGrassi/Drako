#ifndef DRAKO_ECS_COMMAND_QUEUE_HPP_
#define DRAKO_ECS_COMMAND_QUEUE_HPP_

#include <cstdlib>

namespace drako::dod
{
    /// <summary>Fixed size allocator that implements</summary>
    template <typename TCommand, std::size_t Size>
    class DkCommandQueue
    {
    public:

        explicit DkCommandQueue();
        ~DkCommandQueue() = default;

        void queue_command();

        void deque_command();

        void execute_queue();

    private:

        TCommand[Size] m_buffer;
    };
}

#endif // !DRAKO_ECS_COMMAND_QUEUE_HPP_
