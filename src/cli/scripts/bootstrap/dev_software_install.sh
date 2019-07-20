#!/bin/bash
# scripts used for installing dev tools inside docker
set -e

base_dnf_cmd="dnf install -y -q"
# libs
${base_dnf_cmd} spdlog libasan libubsan

# build tools
${base_dnf_cmd} ninja-build git cmake gcc-c++ clang

# other tools
${base_dnf_cmd} cppcheck gtest-devel gtest valgrind
pip3 install cpplint

# publishing tools
${base_dnf_cmd} npm
npm install semantic-release @semantic-release/github @semantic-release/commit-analyzer @semantic-release/release-notes-generator -D
