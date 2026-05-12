FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential cmake pkg-config git \
    libssl-dev libsasl2-dev libzstd-dev \
    libfmt-dev \
    nlohmann-json3-dev \
    librdkafka-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && \
    cmake .. && \
    cmake --build . --target fitness-api && \
    cmake --build . --target statistics_consumer

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl-dev libfmt-dev nlohmann-json3-dev librdkafka-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/fitness-api .
COPY --from=builder /app/build/statistics_consumer .

COPY configs/ ./configs/

EXPOSE 8080

CMD ["./fitness-api", "--config", "configs/config.yaml"]