# Use a lightweight Debian image with build tools
FROM debian:12

# Install required packages: build-essential, cmake, git, bash
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    bash \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside container
WORKDIR /docker

# Copy entire project into container
COPY . .

# Build the project with CMake
RUN mkdir -p build && cd build
RUN cmake -S . -B build && cmake --build build -j$(nproc)

# Expose port if your server listens (adjust as needed)
EXPOSE 9090

# Run your server script
CMD ["./bin/Server"]