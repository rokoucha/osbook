FROM docker.io/library/debian:bookworm

RUN apt update && apt install -y \
    build-essential \
    ca-certificates \
    clang \
    curl \
    dosfstools \
    git \
    lld \
    llvm-dev \
    nasm \
    qemu-utils \
    sudo \
    unzip \
    uuid-dev

ENV HOME=/osdev
WORKDIR ${HOME}

RUN git clone --depth 1 --recurse-submodules --shallow-submodules https://github.com/uchan-nos/mikanos-build.git ${HOME}/osbook

RUN git clone --depth 1 --recurse-submodules --shallow-submodules https://github.com/tianocore/edk2.git ${HOME}/edk2
RUN make -C ${HOME}/edk2/BaseTools/Source/C

WORKDIR ${HOME}/edk2
RUN bash -c "source edksetup.sh"

RUN sed -i '/ACTIVE_PLATFORM/ s:= .*$:= MikanLoaderPkg/MikanLoaderPkg.dsc:' Conf/target.txt \
    && sed -i '/TARGET_ARCH/ s:= .*$:= X64:' Conf/target.txt \
    && sed -i '/TOOL_CHAIN_TAG/ s:= .*$:= CLANGPDB:' Conf/target.txt

WORKDIR ${HOME}/osbook/devenv
RUN curl -Lo x86_64-elf.tar.gz https://github.com/uchan-nos/mikanos-build/releases/download/v2.0/x86_64-elf.tar.gz \
    && tar -xzvf ./x86_64-elf.tar.gz \
    && rm x86_64-elf.tar.gz

COPY build.sh ${HOME}/build.sh

WORKDIR ${HOME}
CMD ["/bin/bash", "build.sh"]
