# compressed_pointcloud_decoder

Decodes `/lidar/points/zstd` into `sensor_msgs/PointCloud2` on `/lidar/points`, using the
[`point_cloud_transport`][pct] zstd plugin with reliable QoS.

Use this node instead of `ros2 run point_cloud_transport republish`, which subscribes
best-effort. The point cloud is a large message, and a best-effort subscriber can drop it.
This node subscribes reliable and receives every message.

## Install

[`point_cloud_transport`][pct] and its [zstd plugin][pct-plugins] decode the stream.
`point_cloud_interfaces`, in the same repo as the plugin, defines the
`CompressedPointCloud2` message type:

```
sudo apt install ros-$ROS_DISTRO-point-cloud-interfaces ros-$ROS_DISTRO-point-cloud-transport ros-$ROS_DISTRO-zstd-point-cloud-transport
```

Then build:

```
colcon build --packages-select compressed_pointcloud_decoder
```

## Run

```
ros2 launch compressed_pointcloud_decoder decode.launch.py
```

The node publishes the decoded cloud on `/lidar/points`.

[pct]: https://github.com/ros-perception/point_cloud_transport
[pct-plugins]: https://github.com/ros-perception/point_cloud_transport_plugins
