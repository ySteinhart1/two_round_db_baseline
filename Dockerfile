FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    thrift-compiler \
    librocksdb-dev \
    libsodium-dev \
    libboost-all-dev \
    automake \
    libthrift-dev \
    g++ \
    git \ 
    libevent-dev \
    openjdk-11-jdk \
    vim \
    make \
    curl \
    python2 \
    maven \
    libleveldb-dev

# RUN sudo apt-get install ant
# RUN wget http://www.apache.org/dyn/closer.cgi?path=/thrift/0.15.0/thrift-0.15.0.tar.gz
# RUN tar -xvf thrift-0.15.0.tar.gz
# RUN cd thrift-0.15.0
# RUN ./bootstrap.sh \
# ./configure \
# sudo make \
# sudo make install


RUN ln -s /usr/bin/clang-9 /usr/bin/clang
RUN ln -s /usr/bin/clang++-9 /usr/bin/clang++
# RUN curl -O --location https://github.com/brianfrankcooper/YCSB/releases/download/0.5.0/ycsb-0.5.0.tar.gz
# RUN tar xfvz ycsb-0.5.0.tar.gz
