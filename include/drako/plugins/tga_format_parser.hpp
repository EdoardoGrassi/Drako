#pragma once
#ifndef DRAKO_TGA_FORMAT_PARSER_HPP
#define DRAKO_TGA_FORMAT_PARSER_HPP

#include <cstddef>
#include <filesystem>
#include <fstream>

namespace drako::plugins
{
    struct tga_header
    {
        uint8_t  IDLength;        /* 00h  Size of Image ID field */
        uint8_t  ColorMapType;    /* 01h  Color map type */
        uint8_t  ImageType;       /* 02h  Image type code */
        uint16_t ColorMapStart;   /* 03h  Color map origin */
        uint16_t ColorMapLength;  /* 05h  Color map length */
        uint8_t  ColorMapDepth;   /* 07h  Depth of color map entries */
        uint16_t XOffset;         /* 08h  X origin of image */
        uint16_t YOffset;         /* 0Ah  Y origin of image */
        uint16_t Width;           /* 0Ch  Width of image */
        uint16_t Height;          /* 0Eh  Height of image */
        uint8_t  PixelDepth;      /* 10h  Image pixel size */
        uint8_t  ImageDescriptor; /* 11h  Image descriptor byte */
    };

    void parse(const std::filesystem::path& src)
    {
        const auto data = std::ifstream(src);
        tga_header header;
    }
} // namespace drako::plugins

#endif // !DRAKO_DRAKO_TGA_FORMAT_PARSER_HPP