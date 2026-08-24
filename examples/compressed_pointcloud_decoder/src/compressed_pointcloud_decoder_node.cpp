// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include <point_cloud_transport/point_cloud_transport.hpp>
#include <point_cloud_transport/transport_hints.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rmw/qos_profiles.h>
#include <sensor_msgs/msg/point_cloud2.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("compressed_pointcloud_decoder");

  auto pub = node->create_publisher<sensor_msgs::msg::PointCloud2>(
    "/lidar/points", rclcpp::QoS(rclcpp::KeepLast(1)));

  // Reliable QoS: a lidar sweep fragments into many datagrams, so a best-effort
  // subscriber loses the whole sweep when any one fragment drops.
  rmw_qos_profile_t qos = rmw_qos_profile_default;
  qos.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  qos.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
  qos.depth = 5;

  point_cloud_transport::PointCloudTransport pct(node);
  const point_cloud_transport::TransportHints hints("zstd");
  [[maybe_unused]] auto sub = pct.subscribe(
    "/lidar/points", qos,
    [pub](const sensor_msgs::msg::PointCloud2::ConstSharedPtr & msg) { pub->publish(*msg); },
    {}, &hints);

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
