#!/usr/bin/env bash
# Installs the Bonsai API dependencies. Does not configure or run the bridge.
set -euo pipefail

DISTRO="${ROS_DISTRO:-humble}"
SUDO=""; [ "$(id -u)" -ne 0 ] && SUDO="sudo"   # no-op as root, e.g. in a Docker build

echo "==> zenoh-bridge-ros2dds (Eclipse apt repo)"
$SUDO mkdir -p /etc/apt/keyrings
curl -fsSL https://download.eclipse.org/zenoh/debian-repo/zenoh-public-key \
  | $SUDO gpg --dearmor --yes -o /etc/apt/keyrings/zenoh-public-key.gpg
echo "deb [signed-by=/etc/apt/keyrings/zenoh-public-key.gpg] https://download.eclipse.org/zenoh/debian-repo/ /" \
  | $SUDO tee /etc/apt/sources.list.d/zenoh.list >/dev/null
$SUDO apt-get update
$SUDO apt-get install -y zenoh-bridge-ros2dds=1.9.0

echo "==> point_cloud_interfaces (CompressedPointCloud2, for /lidar/points/zstd)"
$SUDO apt-get install -y "ros-${DISTRO}-point-cloud-interfaces"

echo "==> foxglove_msgs (GeoJSON, for /geopath)"
$SUDO apt-get install -y "ros-${DISTRO}-foxglove-msgs"
