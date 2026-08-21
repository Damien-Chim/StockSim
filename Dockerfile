# ---------- Build stage ----------
FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

RUN git clone https://github.com/microsoft/vcpkg.git

WORKDIR /opt/vcpkg

RUN ./bootstrap-vcpkg.sh

WORKDIR /app

COPY vcpkg.json .
COPY CMakeLists.txt .

COPY *.cpp .
COPY *.hpp .

RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake

RUN cmake --build build --config Release


# ---------- Runtime stage ----------
FROM ubuntu:24.04

WORKDIR /app

COPY --from=build /app/build/StockSim ./StockSim

CMD ["./StockSim", "1"]