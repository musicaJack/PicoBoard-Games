#!/usr/bin/env python3
"""
从 demo/Chess_Pico 的 chess_pieces.h（64x64 1bpp）生成 28x28 1bpp 头文件。
用法（在项目根目录或 tools/chess_piece_scale 下）：
  python tools/chess_piece_scale/scale_pieces.py
  python scale_pieces.py  # 若在 tools/chess_piece_scale 下，需指定 demo 路径

输出：src/game/chess_pieces_small.h（12 枚 28x28，索引 0..11 与 chess_types 一致）
"""

import re
import os

# 棋子顺序与 chess_types/CHESS_INITIAL_BOARD 一致：黑象王马兵后车，白象王马兵后车
PIECE_NAMES = [
    "black_bishop", "black_king", "black_knight", "black_pawn", "black_queen", "black_rook",
    "white_bishop", "white_king", "white_knight", "white_pawn", "white_queen", "white_rook",
]

SRC_SIZE = 64
DST_SIZE = 28
BYTES_64 = 512   # 64*64/8
BYTES_28 = 98    # 28*28/8


def get_pixel_64(data: list, r: int, c: int) -> int:
    """从 64x64 1bpp 数据取像素 (r,c)，MSB 为左。"""
    idx = r * 8 + (c // 8)
    bit = 7 - (c % 8)
    return (data[idx] >> bit) & 1


def set_pixel_28(data: bytearray, r: int, c: int, val: int) -> None:
    """向 28x28 1bpp 数据写像素 (r,c)。"""
    pos = r * DST_SIZE + c
    idx = pos // 8
    bit = 7 - (pos % 8)
    if val:
        data[idx] |= 1 << bit
    else:
        data[idx] &= ~(1 << bit)


def downsample_64_to_28(src: list) -> bytearray:
    out = bytearray(BYTES_28)
    for dr in range(DST_SIZE):
        for dc in range(DST_SIZE):
            sr = (dr * SRC_SIZE) // DST_SIZE
            sc = (dc * SRC_SIZE) // DST_SIZE
            if sr >= SRC_SIZE:
                sr = SRC_SIZE - 1
            if sc >= SRC_SIZE:
                sc = SRC_SIZE - 1
            p = get_pixel_64(src, sr, sc)
            set_pixel_28(out, dr, dc, p)
    return out


def parse_bitmap_array(text: str) -> list:
    """从 '0x00, 0x01, ...' 解析出字节列表。"""
    hexes = re.findall(r"0x[0-9a-fA-F]+", text)
    return [int(h, 16) for h in hexes]


def extract_bitmaps(h_content: str) -> list:
    """从 .h 文件内容提取 12 个 512 字节的 bitmap。"""
    bitmaps = []
    # 匹配 const uint8_t xxx_bitmap[512] = { ... };
    pattern = r"const uint8_t \w+_bitmap\[512\]\s*=\s*\{([^}]+)\};"
    for m in re.finditer(pattern, h_content, re.DOTALL):
        arr = parse_bitmap_array(m.group(1))
        if len(arr) == 512:
            bitmaps.append(arr)
    return bitmaps


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root = os.path.normpath(os.path.join(script_dir, "..", ".."))
    demo_h = os.path.join(
        repo_root,
        "demo", "Chess_Pico", "lib", "chess_assets", "include", "chess", "chess_pieces.h"
    )
    out_h = os.path.join(repo_root, "src", "game", "chess_pieces_small.h")

    if not os.path.isfile(demo_h):
        print("Error: demo header not found:", demo_h)
        return 1

    with open(demo_h, "r", encoding="utf-8") as f:
        content = f.read()

    bitmaps_64 = extract_bitmaps(content)
    if len(bitmaps_64) != 12:
        print("Warning: expected 12 bitmaps, got", len(bitmaps_64))

    lines = [
        "/* 28x28 1bpp 棋子位图，由 tools/chess_piece_scale/scale_pieces.py 从 demo chess_pieces.h 生成 */",
        "#ifndef PICO_CODE_CHESS_PIECES_SMALL_H",
        "#define PICO_CODE_CHESS_PIECES_SMALL_H",
        "#include <stdint.h>",
        "",
        "#define CHESS_PIECE_SMALL_W 28",
        "#define CHESS_PIECE_SMALL_H 28",
        "#define CHESS_PIECE_SMALL_BYTES 98",
        "",
    ]

    for i, name in enumerate(PIECE_NAMES):
        if i < len(bitmaps_64):
            small = downsample_64_to_28(bitmaps_64[i])
        else:
            small = bytearray(BYTES_28)
        lines.append(f"/* {name} 28x28 */")
        lines.append(f"static const uint8_t chess_piece_small_{i}[{BYTES_28}] = {{")
        for row in range(0, BYTES_28, 16):
            chunk = small[row:row+16]
            hexs = ", ".join(f"0x{b:02x}" for b in chunk)
            lines.append("    " + hexs + ",")
        lines.append("};")
        lines.append("")

    lines.append("/* 索引 0..11 与 chess_types 棋子索引一致 */")
    lines.append("static const uint8_t* const chess_pieces_small[12] = {")
    for i in range(12):
        lines.append(f"    chess_piece_small_{i},")
    lines.append("};")
    lines.append("")
    lines.append("#endif /* PICO_CODE_CHESS_PIECES_SMALL_H */")

    os.makedirs(os.path.dirname(out_h), exist_ok=True)
    with open(out_h, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

    print("Written:", out_h)
    return 0


if __name__ == "__main__":
    exit(main())
