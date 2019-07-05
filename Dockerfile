FROM fedora:latest 
LABEL maintainer khoidinhtrinh@gmail.com 

RUN dnf install spdlog libasan libubsan -y -q # libs
RUN dnf install ninja-build git cmake gcc-c++ clang -y -q # build tools
RUN dnf install cppcheck gtest-devel gtest valgrind -y -q # other tools
RUN pip3 install cpplint -q # pip tools
RUN dnf install curl jq ripgrep dnf-plugins-core dbus -y -q # run time tool
RUN dnf install npm -y -q