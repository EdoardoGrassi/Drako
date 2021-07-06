#include "drako/devel/mesh_importers.hpp"

#include <obj-cpp/obj.hpp>

#include <vector>

namespace drako::editor
{
    /*const dson::DOM& operator>>(const dson::DOM& dom, MeshImportInfo& info)
    {
        info.guid = uuid::Uuid{ dom.get("guid") };
        info.name = dom.get("name");
        info.path = dom.get("path");
        return dom;
    }

    dson::DOM& operator<<(dson::DOM& dom, const MeshImportInfo& info)
    {
        dom.set("guid", to_string(info.guid));
        dom.set("name", info.name);
        dom.set("path", info.path.string());
        return dom;
    }*/


    //[[nodiscard]] Mesh compile(const obj::MeshData& md, const ObjImportConfig& config)

    [[nodiscard]] Mesh compile(const obj::MeshData& md)
    {
        std::vector<std::byte> verts_data;
        verts_data.resize(std::size(md.v) * sizeof(float) * 3);

        for (const auto& v : md.v)
        {
            for (const auto& c : { v.x, v.y, v.z })
            {
                const auto bytes = std::bit_cast<std::array<std::byte, 4>>(c);
                verts_data.insert(std::end(verts_data),
                    std::cbegin(bytes), std::cend(bytes));
            }
        }

        std::vector<std::byte> index_data;
        index_data.resize(std::size(md.faces) * sizeof(std::uint32_t) * 3);

        for (const auto& f : md.faces)
        {
            for (const auto& c : { f.triplets[0].v, f.triplets[1].v, f.triplets[2].v })
            {
                const auto bytes = std::bit_cast<std::array<std::byte, 4>>(c);
                index_data.insert(std::end(verts_data),
                    std::cbegin(bytes), std::cend(bytes));
            }
        }

        using _verts_count_t = decltype(MeshMetaInfo::vertex_count);
        using _index_count_t = decltype(MeshMetaInfo::index_count);
        const MeshMetaInfo meta{
            .vertex_count      = (std::size(md.v) <= std::numeric_limits<_verts_count_t>::max())
                                     ? static_cast<_verts_count_t>(std::size(md.v))
                                     : throw std::runtime_error{ "Too many vertices" },
            .index_count       = (std::size(md.v) * 3 <= std::numeric_limits<_index_count_t>::max())
                                     ? static_cast<_index_count_t>(std::size(md.v) * 3)
                                     : throw std::runtime_error{ "Too many indices" },
            .vertex_size_bytes = sizeof(float) * 3,
            .index_size_bytes  = sizeof(std::uint32_t),
            .topology          = MeshMetaInfo::Topology::triangle_list
        };

        return Mesh{ meta, std::move(verts_data), std::move(index_data) };
        //throw std::runtime_error{ "Not implemented" };
    }

} // namespace drako::editor