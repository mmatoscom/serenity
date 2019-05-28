FROM golang
    
    RUN apt update && \ 
        apt install patch \
        rsync \
        libmpfr-dev \
        libmpc-dev \
        libgmp-dev -y
    ADD . .
    RUN ./Toolchain/BuildIt.sh
    RUN . ./Toolchain/UseIt.sh
    WORKDIR Kernel
    RUN ./makeall.sh 