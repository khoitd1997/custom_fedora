#!/bin/bash
# used for profiling hatter c++

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd ${currDir}/build/bin
perf record -g ./hatter_config_builder_parser example_setting.toml
perf report -g 'graph,0.1,caller'