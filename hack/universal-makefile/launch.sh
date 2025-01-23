#!/bin/bash

mkdir -p .vscode

args=($@)
json_objects=()
for arg in "${args[@]}"; do
    json_objects+=("{
        \"name\": \"(gdb) ./$arg\",
        \"type\": \"cppdbg\",
        \"request\": \"launch\",
        \"program\": \"\${workspaceFolder}/$arg\",
        \"args\": [],
        \"stopAtEntry\": false,
        \"cwd\": \"\${workspaceFolder}\",
        \"environment\": [],
        \"externalConsole\": false,
        \"MIMode\": \"gdb\",
        \"setupCommands\": [
            {
                \"description\": \"Enable pretty-printing for gdb\",
                \"text\": \"-enable-pretty-printing\",
                \"ignoreFailures\": true
            },
            {
                \"description\": \"Set Disassembly Flavor to Intel\",
                \"text\": \"-gdb-set disassembly-flavor intel\",
                \"ignoreFailures\": true
            }
        ]
    }")
done

jq -n --argjson objects "$(printf '%s\n' "${json_objects[@]}" | jq -s .)" \
    '{configurations: $objects}' > .vscode/launch.json
