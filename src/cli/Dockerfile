FROM fedora:latest 
LABEL maintainer khoidinhtrinh@gmail.com 

COPY ./scripts/dev_bootstrap.sh /tmp
COPY ./scripts/mock_bootstrap.sh /tmp
RUN /tmp/dev_bootstrap.sh
RUN /tmp/mock_bootstrap.sh

ENV IN_HATTER_DOCKER True
COPY . /root
# will be empty in gitlab-ci
ENTRYPOINT [ "/root/build.sh" ]