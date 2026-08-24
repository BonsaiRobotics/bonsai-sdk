# Bonsai API bridge image. Easiest path is compose.yaml (docker compose up --build).
# Build:  docker build -t bonsai-api-bridge .   (on Jetson add --network host)
# Run:    docker run --rm --network host bonsai-api-bridge
FROM ros:humble
COPY install.sh /tmp/install.sh
RUN bash /tmp/install.sh && rm -rf /var/lib/apt/lists/*
COPY config/bridge.json5 /bridge.json5
CMD ["zenoh-bridge-ros2dds", "-c", "/bridge.json5"]
