# SPDX-License-Identifier: Apache-2.0

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node


ARGUMENTS = [
    DeclareLaunchArgument('compressed',
                          default_value='/camera_front_center/left/image_compressed_raw',
                          description='CompressedImage topic to decode'),
    DeclareLaunchArgument('image_raw',
                          default_value='/image_raw',
                          description='Decoded Image topic to publish'),
]


def generate_launch_description():
    pkg_compressed_image_decoder = get_package_share_directory('compressed_image_decoder')

    params_arg = DeclareLaunchArgument(
        'params_file',
        default_value=PathJoinSubstitution(
            [pkg_compressed_image_decoder, 'config', 'decoder.yaml']),
        description='Decoder parameters')

    decoder = Node(
        package='compressed_image_decoder',
        executable='compressed_image_decoder_node',
        name='compressed_image_decoder',
        output='screen',
        parameters=[LaunchConfiguration('params_file')],
        remappings=[
            ('compressed', LaunchConfiguration('compressed')),
            ('image_raw', LaunchConfiguration('image_raw')),
        ],
    )

    ld = LaunchDescription(ARGUMENTS)
    ld.add_action(params_arg)
    ld.add_action(decoder)
    return ld
