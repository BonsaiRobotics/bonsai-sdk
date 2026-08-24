cmake_minimum_required(VERSION 3.16)
project(compressed_pointcloud_decoder)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(point_cloud_transport REQUIRED)

add_executable(compressed_pointcloud_decoder_node src/compressed_pointcloud_decoder_node.cpp)
ament_target_dependencies(compressed_pointcloud_decoder_node rclcpp sensor_msgs point_cloud_transport)

install(TARGETS compressed_pointcloud_decoder_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

ament_package()
