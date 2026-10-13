# Python 3.12 on Debian bookworm, with the tools to build and debug the C++ version too
FROM mcr.microsoft.com/devcontainers/python:1-3.12-bookworm

RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ gdb make \
    && rm -rf /var/lib/apt/lists/*

# pip installs for the vscode user land in ~/.local; put its scripts (eg. `raytracer`) on the PATH
ENV PATH=/home/vscode/.local/bin:${PATH}
