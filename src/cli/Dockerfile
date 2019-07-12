FROM fedora:latest 
LABEL maintainer khoidinhtrinh@gmail.com 

# libs
RUN dnf install spdlog libasan libubsan -y -q
# build tools
RUN dnf install ninja-build git cmake gcc-c++ clang -y -q
# other tools
RUN dnf install findutils cppcheck gtest-devel gtest valgrind -y -q
RUN pip3 install cpplint -q
# runtime tools
RUN dnf install curl jq ripgrep fedora-workstation-repositories dnf-plugins-core -y -q
# publishing tools
RUN dnf install npm -y -q
RUN npm install semantic-release @semantic-release/github @semantic-release/commit-analyzer @semantic-release/release-notes-generator -D

ENV IN_HATTER_DOCKER True
COPY . /root
# will be empty in gitlab-ci
ENTRYPOINT [ "/root/build.sh" ]