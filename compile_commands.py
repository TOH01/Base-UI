import os, json, glob

# Adjust these to match your project
project_root = "C:/Users/Tobi/Github/Base-UI"
compiler = "C:/MinGW/bin/gcc.exe"
build_dir = "build"
flags = "-g -O0 -DDEBUG -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components -Wall -Wextra -Werror -Wstrict-prototypes -D_WIN32_WINNT=0x0A00 -IC:/msys64/ucrt64/include"

entries = []

# Recursively find all .c files in src/
for root, dirs, files in os.walk(os.path.join(project_root, "src")):
    for file in files:
        if file.endswith(".c"):
            file_path = os.path.join(root, file)
            rel_file = os.path.relpath(file_path, project_root).replace("\\", "/")
            obj_file = os.path.join(build_dir, os.path.splitext(rel_file)[0] + ".o").replace("\\", "/")
            cmd = f"{compiler} {flags} -c {rel_file} -o {obj_file}"
            entries.append({
                "directory": project_root.replace("\\", "/"),
                "command": cmd,
                "file": rel_file
            })

# Write JSON
with open(os.path.join(project_root, "compile_commands.json"), "w") as f:
    json.dump(entries, f, indent=2)

print(f"Generated compile_commands.json with {len(entries)} entries.")
