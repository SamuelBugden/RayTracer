# Python 3.14 on Debian 13 (trixie), with the tools to build and debug the C++ version too.
# Image tag: <image version 3>-<python 3.14>-<debian trixie>
FROM mcr.microsoft.com/devcontainers/python:3-3.14-trixie

RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ gdb make \
    && rm -rf /var/lib/apt/lists/*

# pip installs for the vscode user land in ~/.local; put its scripts (eg. `raytracer`) on the PATH
ENV PATH=/home/vscode/.local/bin:${PATH}
