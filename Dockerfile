# This file is intended for development usage, particularly when fucking around with the install
# interface, to avoid accidents in a real system.
# This does NOT mean upm works with virtualisation! This is not the intended way to run upm,
# and will most likely mean upm won't work, even if you make hacks to tie your system to the container.
# 
# TL;DR: don't use this if your plan is to either verify that upm does what it claims, or if you're
# making changes in areas that could have bad consequences if handled poorly.

# Copy-pasta command for dev use: sudo docker build -t upm-dev . && sudo docker run -ti --rm upm-dev
FROM ubuntu:latest
# This includes a subset of dependencies. Various apps have various dependencies, but this aims to cover
# some core dependencies. Any other deps need to be installed in the container on-demand
RUN apt update && apt install -y libxt6 libgtk-3-0 libpango-1.0-0 libcairo2 libatk1.0-0 git python3 python3-dev gcc g++ libncurses-dev make cmake libssl-dev python3 python3-pip

# Copy the cwd state
COPY . /app
WORKDIR /app
ENV SPDLOG_LEVEL=debug
