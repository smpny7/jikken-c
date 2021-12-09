FROM debian:stretch
LABEL Kaito Ikeda
WORKDIR /home/jikken-c

ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64

RUN apt-get update && apt-get install -y \
    gcc \
    g++-multilib \
    vim \
    make \
    flex \
    bison \
    libfl-dev \
    libbison-dev \
    gdb \
    git \
    curl \
    dnsutils \
    telnet \
    unzip \
    man \
    manpages-ja \
    zip \
    default-jre  \
    default-jdk \
    openjdk-8-jre \
    && apt-get update \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN echo 'export PATH=$PATH:/home/jikken-c/bin/maps' >> ~/.bash_profile
RUN echo 'export LANGUAGE=ja_JP.utf8' >> ~/.bash_profile
RUN echo 'export LC_ALL=ja_JP.utf8' >> ~/.bash_profile