FROM fedora:latest 
LABEL maintainer khoidinhtrinh@gmail.com 

COPY ./scripts/bootstrap/dev_software_install.sh /tmp
COPY ./scripts/bootstrap/mock_software_install.sh /tmp
RUN /tmp/dev_software_install.sh
RUN /tmp/mock_software_install.sh

ENV IN_HATTER_DOCKER True
COPY . /root
# will be empty in gitlab-ci
ENTRYPOINT [ "/root/build.sh" ]