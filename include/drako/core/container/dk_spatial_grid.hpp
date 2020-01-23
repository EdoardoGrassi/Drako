#pragma once
#ifndef DRAKO_SPATIAL_GRID_HPP
#define DRAKO_SPATIAL_GRID_HPP

#include <vector>

#include "core/memory/unsync_pool_allocator.hpp"
#include "math/dk_aarect.hpp"
#include "math/dk_vector2.hpp"
#include "development/dk_assertion.hpp"

namespace drako
{
    using ObjHandle = uint32_t;


    // CLASS TEMPLATE
    // Uniform implicit 2D grid.
    //
    template <typename T, size_t Width, size_t Height, size_t NumObjects>
    class implicit_grid_2d
    {
    public:

        explicit implicit_grid_2d(size_t cell_width, size_t cell_height) noexcept
            : _cell_width(cell_height), _cell_height(cell_height)
        {
            DRAKO_PRECON(cell_width > 0);
            DRAKO_PRECON(cell_height > 0);

            memset(_row, 0, GRID_HEIGHT * NUM_OBJECTS_DIV_32 * sizeof(uint32_t));
            memset(_col, 0, GRID_WIDTH * NUM_OBJECTS_DIV_32 * sizeof(uint32_t));
        }


        // Inserts an object into the grid.
        //  Complexity: O(1)
        //
        bool insert(const T& obj, Point2D pos) noexcept;


        // Removes an object from the grid.
        //  Complexity: O(N), where N is the amount of objects inside the grid.
        //
        bool remove(const T& obj) noexcept;


    private:

        const size_t GRID_WIDTH = Width;
        const size_t DRIG_HEIGHT = Height;
        const size_t NUM_OBJECTS_DIV_32 = (NumObjects + 31) / 32; // round up

        unsync_pool_allocator<T> _obj;
        uint32_t        _row[GRID_HEIGHT][NUM_OBJECTS_DIV_32];
        uint32_t        _col[GRID_WIDTH][NUM_OBJECTS_DIV_32];
        const size_t    _cell_width;
        const size_t    _cell_height;
    };






    // CLASS TEMPLATE
    // Uniform layer grid.
    //
    template <typename Ty, size_t Width, size_t Height, size_t LayerCount>
    class layer_grid_2d
    {
        using float2 = float_t[2];
        using uint2 = uint32_t[2];

    public:

        struct Layer
        {
            uint32_t idx[];
        };

        explicit layer_grid_2d(float2 origin, float2 cell_size) noexcept
            : _origin(origin), _cell_size(cell_size)
        {
            DRAKO_PRECON(cell_size[0] > 0f, "Cell size must be greater than 0");
            DRAKO_PRECON(cell_size[1] > 0f, "Cell size must be greater than 0");
        }

        void insert(Ty handle, float2 position, Layer layer) noexcept
        {
            DRAKO_PRECON(); // TODO: Check layer is valid
            auto target_cell = this->compute_cell_index(position);
        }

        void update(Ty const handle, float2 const position) noexcept;

        void remove(Ty const handle, Layer const layer) noexcept;

        bool test() const noexcept
        {

        }

    private:

        constexpr const size_t NumLayers = (LayerCount + 31) / 32;

        uint32_t            _x[Width][NumLayers];       // Bit array for X axis
        uint32_t            _y[Height][NumLayers];      // Bit array for Y axis
        std::vector<Ty>     _handles[NumLayers];        // Stored objects
        float2 const        _origin;                    // Origin of the grid
        float2 const        _cell_size;                 // Size of a single grid cell

        uint2 compute_cell_index(float2 const pos) const noexcept
    };

} // namespace drako

#endif // !DRAKO_SPATIAL_GRID_HPP
