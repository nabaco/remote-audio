FROM arm64v8/ubuntu:22.04

RUN apt-get update
RUN apt-get upgrade -y

# Install dependencies
RUN apt-get install -y \
        autoconf \
        automake \
        build-essential \
        pkg-config \
        libgstreamer1.0-dev \
        libgstreamer-plugins-base1.0-dev \
        checkinstall \
        #crossbuild-essential-arm64 \
        git \
        && \
    rm -rf /var/lib/apt/lists/* /var/cache/apt/packges/*

# Set up the workspace
WORKDIR /workspace
