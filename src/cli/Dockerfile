FROM fedora:latest 
LABEL maintainer khoidinhtrinh@gmail.com 

RUN ./scripts/dev_bootstrap.sh
RUN ./scripts/mock_bootstrap.sh

ENV IN_HATTER_DOCKER True
COPY . /root
# will be empty in gitlab-ci
ENTRYPOINT [ "/root/build.sh" ]