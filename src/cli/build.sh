#!/bin/bash

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# rm -rf ${currDir}/build
mkdir -p ${currDir}/build/bin/build/logs
ln -sfv ~/hatter/example/example_settings.toml ~/hatter/src/cli/build/bin
ln -sfv ~/hatter/example/child_setting_1.toml ~/hatter/src/cli/build/bin
mkdir -p ~/hatter/src/cli/build/bin/child_path
ln -sfv ~/hatter/example/child_path/child_setting_2.toml ~/hatter/src/cli/build/bin/child_path

cd ${currDir}/build

# cmake .. && make && ./bin/tomlparser ./bin/example_settings.toml
# scan-build cmake -G Ninja .. && scan-build ninja && ./bin/tomlparser ./bin/settings.toml
cmake -G Ninja .. && ninja 
cd bin
./tomlparser example_settings.toml
# cmake -G Ninja .. && ninja && ./bin/tomlparser ./bin/random.toml