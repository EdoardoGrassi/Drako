#include "drako/graphics/vulkan_memory_allocator.hpp"

#include <cassert>
#include <vulkan/vulkan.hpp>

namespace drako::vulkan
{
    using _this = host_stack_allocator;

    _this::host_stack_allocator(const context& ctx, const vk::MemoryAllocateInfo& info) noexcept
        : _device{ ctx.logical_device.get() }
        , _memory{ _device.allocateMemoryUnique(info) }
    {
        assert(info.allocationSize > 0);

        _buffer = static_cast<std::byte*>(_device.mapMemory(_memory.get(),
            vk::DeviceSize{ 0 }, vk::DeviceSize{ VK_WHOLE_SIZE }, {}));
    }



    _this::~host_stack_allocator()
    {
        _device.unmapMemory(_memory.get());
    }

    [[nodiscard]] void* _this::allocate(vk::Buffer buffer) noexcept
    {
        assert(buffer != vk::Buffer{ nullptr });
        const auto specs = _device.getBufferMemoryRequirements(buffer);


        _offset += specs.size;
        return _buffer + _offset;
    }

    void _this::deallocate(void* ptr) noexcept
    {
        assert(ptr);
        // TODO: deallocate
    }
} // namespace drako::vulkan