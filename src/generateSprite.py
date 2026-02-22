val = """
"                        "
"                        "
"_              ,------_ "
"||__    ___   ,| O  ___|"
" |_ '--'   '--'    '__  "
"   |          ,-'-----' "
"    |_________/         "
"      |_] || '--        "
"          |_]           "
"""

x = val.strip().split("\n")
FILLER = "$"

height = len(x)
width = len(x[0]) - 2

ansi = []
plain = []
collisionMatrix = [[0 for _ in range(width)] for _ in range(height)]

for i, line in enumerate(x):
    line = line[1:-1]
    plain.append(line)

    space = 0
    ansi_line = ""
    for ch in line:
        if ch == " ":
            space += 1
        else:
            if space > 0:
                ansi_line += f"\\x1b[{space}C"
                space = 0
            ansi_line += ch
    if space > 0:
        ansi_line += f"\\x1b[{space}C"
        space = 0
    ansi.append(ansi_line)

    for j, c in enumerate(line):
        collisionMatrix[i][j] = 0 if c == " " else 1

# ---- PRINT OUTPUT ----
print(f"static SpriteAsset spriteAssetName{{")
print(f"    {height},")
print(f"    {width},")
print(f"    {{  // collision matrix")
for row in collisionMatrix:
    print("        {" + ", ".join(map(str, row)) + "},")
print(f"    }},")
print(f"    {{")
print(f"        // clang-format off")
for line in ansi:
    print(f'      "{line.replace(FILLER, " ")}",')
print(f"        // clang-format on")
print(f"    }},")
print(f"    {{")
print(f"        // clang-format off")
for line in plain:
    print(f'      "{line.replace(FILLER, " ")}",')
print(f"        // clang-format on")
print(f"    }},")
print(f"}};")
