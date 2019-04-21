#!/bin/bash

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# rm -rf ${currDir}/build
mkdir -p ${currDir}/build/bin/build/logs
ln -sfv ~/hatter/example/example_settings.toml ~/hatter/src/cli/build/bin

cd ${currDir}/build

# cmake .. && make && ./bin/tomlparser ./bin/example_settings.toml
# scan-build cmake -G Ninja .. && scan-build ninja && ./bin/tomlparser ./bin/settings.toml
cmake -G Ninja .. && ninja && ./bin/tomlparser ./bin/example_settings.toml