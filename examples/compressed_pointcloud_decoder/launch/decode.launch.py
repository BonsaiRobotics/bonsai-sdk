# SPDX-License-Identifier: Apache-2.0

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='compressed_pointcloud_decoder',
            executable='compressed_pointcloud_decoder_node',
            name='compressed_pointcloud_decoder',
            output='screen',
        ),
    ])
