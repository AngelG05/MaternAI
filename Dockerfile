# Production-Grade Multi-stage Build
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++-12 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy headers and source
COPY include/ ./include/
COPY src/ ./src/
COPY CMakeLists.txt ./

# Build optimized Release binary
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# Final Runtime Image
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y ca-certificates && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/MaternAI /usr/local/bin/maternai

# Hardware optimization environment variables
ENV OMP_NUM_THREADS=4
ENV MALLOC_CONF="background_thread:true,metadata_thp:auto,dirty_decay_ms:30000,muzzy_decay_ms:30000"

EXPOSE 18080

# Start MaternAI
CMD ["maternai"]
