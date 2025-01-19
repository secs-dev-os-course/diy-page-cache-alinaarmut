FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    cmake \
    git \
    libc6-dev \
    gdb \
    htop \
    sysstat \
    linux-tools-common


COPY . /workspace


WORKDIR /workspace


CMD ["bash"]
