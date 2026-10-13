# Python 3.14 on Debian 13 (trixie), with the tools to build and debug the C++ version too.
# Image tag: <image version 3>-<python 3.14>-<debian trixie>
FROM mcr.microsoft.com/devcontainers/python:3-3.14-trixie

RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ gdb make \
    && rm -rf /var/lib/apt/lists/*

# Claude Code's config and login live here, on a named volume (see devcontainer.json).
# Creating it owned by vscode means a new, empty volume starts with that ownership.
RUN mkdir -p /home/vscode/.claude && chown vscode:vscode /home/vscode/.claude

# pip installs for the vscode user land in ~/.local; put its scripts (eg. `raytracer`) on the PATH
ENV PATH=/home/vscode/.local/bin:${PATH}
