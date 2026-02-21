FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libeigen3-dev \
    nlohmann-json3-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

RUN mkdir build && cd build && \
    cmake .. && make -j$(nproc) && \
    ctest --output-on-failure

ENTRYPOINT ["/workspace/build/benchmark"]
CMD ["--config", "/workspace/experiments/configs/simple_grid.json"]
