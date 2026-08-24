# Bonsai API SDK

Version 0.1.0

The Bonsai robot publishes a set of ROS 2 sensor topics. A [`zenoh-bridge-ros2dds`][zenoh]
instance runs on each side of the direct ethernet link, and the topics appear in your ROS
2 graph. Your machine keeps its own ROS 2 distribution and DDS.

## What's here

| Path                         | Purpose                                              |
| ---------------------------- | ---------------------------------------------------- |
| `install.sh`                 | Installs the bridge and its message dependencies.    |
| `config/bridge.json5`        | Bridge configuration.                                |
| `Dockerfile`, `compose.yaml` | Run the bridge in a container.                       |
| `examples/`                  | Decoders for the compressed camera and lidar topics. |
| `msgs/`                      | The `bonsai_msgs` package.                           |

## Requirements

- ROS 2 with a supported DDS. See [Compatibility](#compatibility).
- A direct ethernet link to the Bonsai robot.
- Docker, if you run the bridge with the bundled [`compose.yaml`](compose.yaml)
  (optional).

## Setup

### 1. Install the dependencies

```
./install.sh
```

The script adds the [Eclipse zenoh apt repository][zenoh-apt] and installs:

| Package                                  | Source                                               |
| ---------------------------------------- | ---------------------------------------------------- |
| `zenoh-bridge-ros2dds` 1.9.0             | [eclipse-zenoh/zenoh-plugin-ros2dds][zenoh]          |
| `ros-$ROS_DISTRO-point-cloud-interfaces` | [ros-perception/point_cloud_transport_plugins][pctp] |
| `ros-$ROS_DISTRO-foxglove-msgs`          | [foxglove/foxglove-sdk][foxglove]                    |

`/vehicle_state` uses the `bonsai_msgs` package. To read it, build `bonsai_msgs` once in
your workspace:

```
cp -r msgs/bonsai_msgs <your_workspace>/src/
colcon build --packages-select bonsai_msgs
```

### 2. Configure the link

Edit [`config/bridge.json5`](config/bridge.json5) and set how the bridge reaches Bonsai,
using one of:

- `interface`: the ethernet device on the direct link, for example `eth0`. The bridge
  discovers Bonsai over multicast on that device.
- `connect`: the brain's address, if you know its static IP. Uncomment
  `connect: { endpoints: ["tcp/<bonsai-ip>:7447"] }` and replace `<bonsai-ip>` with the
  brain's IP. Bonsai's bridge listens on port 7447.

If your local DDS domain is not 0, also set `domain`. Every field is documented in the
file.

### 3. Run the bridge

The bundled compose file builds and runs the bridge:

```
docker compose up --build
```

You can also run the bridge binary directly:

```
zenoh-bridge-ros2dds -c config/bridge.json5
```

### 4. Verify

List the topics:

```
ros2 topic list
```

Check that the link is up:

```
curl -s localhost:18000/@/*/router | jq '.[0].value.sessions'
```

## Topics

Every topic uses a stock ROS 2 type except three: [`point_cloud_interfaces`][pctp]
(`CompressedPointCloud2`), [`foxglove_msgs`][foxglove] (`GeoJSON`), and `bonsai_msgs`. The
first two are installed by `install.sh`, and you build `bonsai_msgs` from `msgs/`.

### Cameras

| Topic                                                  | Type                          | Rate  |
| ------------------------------------------------------ | ----------------------------- | ----- |
| `/camera_front_center/left/image_compressed_raw`       | `sensor_msgs/CompressedImage` | 15 Hz |
| `/camera_front_center/right/image_compressed_raw`      | `sensor_msgs/CompressedImage` | 15 Hz |
| `/camera_rear_center/left/image_compressed_raw`        | `sensor_msgs/CompressedImage` | 15 Hz |
| `/camera_rear_center/right/image_compressed_raw`       | `sensor_msgs/CompressedImage` | 15 Hz |
| `/camera_{front,rear}_center/{left,right}/camera_info` | `sensor_msgs/CameraInfo`      | 15 Hz |

The images are `format="h265"`. Decode them with
[`compressed_image_decoder`](examples/compressed_image_decoder).

### IMU

| Topic                             | Type              | Rate    |
| --------------------------------- | ----------------- | ------- |
| `/imu/data`                       | `sensor_msgs/Imu` | >100 Hz |
| `/lidar/imu`                      | `sensor_msgs/Imu` | ~400 Hz |
| `/camera_{front,rear}_center/imu` | `sensor_msgs/Imu` | >100 Hz |

All IMU topics report linear acceleration and angular velocity only. None report
orientation: the orientation field is an identity quaternion with covariance `[-1, ...]`,
marked unusable by REP 145.

### GNSS

| Topic                  | Type                    | Rate  |
| ---------------------- | ----------------------- | ----- |
| `/gnss_1/llh_position` | `sensor_msgs/NavSatFix` | 10 Hz |
| `/gnss_2/llh_position` | `sensor_msgs/NavSatFix` | 10 Hz |
| `/gnss/heading_raw`    | `sensor_msgs/Imu`       | 10 Hz |

`/gnss/heading_raw` reports the raw dual-antenna heading as an ENU orientation. Yaw and
pitch accuracy are in `orientation_covariance`. Acceleration and angular velocity are
marked unusable by REP 145.

### Lidar

| Topic                | Type                                           | Rate  |
| -------------------- | ---------------------------------------------- | ----- |
| `/lidar/points/zstd` | `point_cloud_interfaces/CompressedPointCloud2` | 10 Hz |

The points are `format="zstd"`, wire-compatible with [`zstd_point_cloud_transport`][pctp].
Decode them with
[`compressed_pointcloud_decoder`](examples/compressed_pointcloud_decoder), and subscribe
reliable (see [Reliable QoS for large topics](#reliable-qos-for-large-topics)).

### Wheel odometry

| Topic         | Type                         | Rate   |
| ------------- | ---------------------------- | ------ |
| `/meas_twist` | `geometry_msgs/TwistStamped` | >30 Hz |

### Vehicle state and navigation

| Topic            | Type                       | Rate      |
| ---------------- | -------------------------- | --------- |
| `/vehicle_state` | `bonsai_msgs/VehicleState` | 10 Hz     |
| `/geopath`       | `foxglove_msgs/GeoJSON`    | on update |

`VehicleState` is defined in `bonsai_msgs` (see Setup). `/geopath` is not latched, so a
subscriber that joins late receives the next update rather than the current path.

## Examples

- [`compressed_image_decoder`](examples/compressed_image_decoder) decodes an h265 camera
  topic to a raw `sensor_msgs/Image`.
- [`compressed_pointcloud_decoder`](examples/compressed_pointcloud_decoder) decodes the
  zstd lidar topic to a `sensor_msgs/PointCloud2`.

## Reliable QoS for large topics

The lidar and camera topics are published reliable, and each message is large enough to
span several network packets. Subscribe reliable so you receive every message; a
best-effort subscriber loses the entire message if a single packet is dropped.
`ros2 topic hz` always subscribes best-effort and has no flag to change that, so it
reports a lower rate than the bridge actually delivers. Measure the real rate with
`ros2 bag record` or a reliable subscriber.

## Compatibility

Tested so far:

| ROS 2   | DDS                   |
| ------- | --------------------- |
| Humble  | [CycloneDDS][cyclone] |
| Jazzy   | [Fast DDS][fastdds]   |
| Kilted  |                       |
| Rolling |                       |

If you use something else, let us know. If [`zenoh-bridge-ros2dds`][zenoh] supports it, it
probably works.

[zenoh]: https://github.com/eclipse-zenoh/zenoh-plugin-ros2dds
[zenoh-apt]: https://download.eclipse.org/zenoh/debian-repo/
[pctp]: https://github.com/ros-perception/point_cloud_transport_plugins
[foxglove]: https://github.com/foxglove/foxglove-sdk/tree/main/schemas/ros2
[cyclone]: https://github.com/eclipse-cyclonedds/cyclonedds
[fastdds]: https://github.com/eProsima/Fast-DDS
