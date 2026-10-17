import json
import pathlib

home = pathlib.Path(__file__).parent
build = home / "build"
src = home / "src"

C_FLAGS = [
    "arm-none-eabi-gcc",
    "-mthumb",
    "-O0",
    "-mabi=aapcs",
    "-mtune=arm7tdmi",
    "-march=armv4t",
    "-Wno-pointer-to-int-cast",
    "-std=gnu23",
    "-Wall",
    "-W-enum-conversion",
    "-W-strict-aliasing",
    "-Wno-attribute-alias",
    "-Wno-trigraphs",
    "-Woverride-init",
    "-Wno-gnu",
    "-include global.h",
    "-D__INTELLISENSE__",
    "-DTESTING",
    "-DMODERN=1",
]

PER_FILE_FLAGS = {
    "src/field_mugshot.c": [
        "-include constants/field_mugshots.h",
    ],
    "src/move.c": [
        "-include move.h",
    ],
    "src/battle_message.c": [
        "-Wno-unknown-escape-sequence"
    ],
    "src/battle_main.c": [
        "-Wno-excess-initializers",
        "-Wno-macro-redefined",
        "-Wno-incompatible-pointer-types",
        "-Wno-int-conversion",
        "-Wno-implicit-int-conversion",
        "-Wno-sign-conversion",
        "-Wno-shorten-64-to-32"
    ],
    "src/pokemon.c": [
        "-include data.h",
        "-Isrc/data/graphics"
    ]
}

compile_commands = []

for file in src.rglob("*.c"):
    rel = file.resolve().relative_to(home).as_posix()
    extra = PER_FILE_FLAGS.get(rel, [])

    compile_commands.append(
        {
            "directory": str(build),
            "arguments": (
                C_FLAGS
                + extra
                + [
                    f"-I{home}/include",
                    f"-I{home}/gflib",
                    "-o",
                    str(file.with_suffix(".o")),
                    str(file.resolve()),
                ]
            ),
            "file": str(file.resolve()),
        }
    )

with open("compile_commands.json", "w") as outfile:
    json.dump(compile_commands, outfile, default=str, indent=4)
