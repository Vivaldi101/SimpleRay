#if !defined(BMP_H)
#define BMP_H

#pragma pack(push, 1)
typedef struct bmp_t
{
    u16 file_type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 bmp_offset;
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bpp;
    u32 compression;
    u32 size_of_bmp;
    s32 horz_res;
    s32 vert_hes;
    u32 colors_used;
    u32 colors_important;
} bmp_t;
#pragma pack(pop)

typedef struct image_t
{
    u32 width;
    u32 height;
    u32 num_bytes;
    u32 *pixels;
} image_t;

#endif   // BMP_H
