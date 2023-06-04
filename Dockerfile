FROM alpine:3.17.0 AS builder

RUN apk update && \
    apk add --no-cache \
    build-base=0.5-r3 \
    cmake=3.24.4-r0 \
    jsoncpp-dev=1.9.5-r0

WORKDIR /app

COPY src/ ./src/
COPY include/ ./include/
COPY makefile .

RUN make client.exe
