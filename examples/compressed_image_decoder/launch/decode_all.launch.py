# SPDX-License-Identifier: Apache-2.0

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


CAMERAS = [
    'camera_front_center/left',
    'camera_front_center/right',
    'camera_rear_center/left',
    'camera_rear_center/right',
]


def generate_launch_description():
    pkg_compressed_image_decoder = get_package_share_directory('compressed_image_decoder')

    params_arg = DeclareLaunchArgument(
        'params_file',
        default_value=PathJoinSubstitution(
            [pkg_compressed_image_decoder, 'config', 'decoder.yaml']),
        description='Decoder parameters, applied to every stream')
    params_file = LaunchConfiguration('params_file')

    decoders = [
        ComposableNode(
            package='compressed_image_decoder',
            plugin='compressed_image_decoder::CompressedImageDecoder',
            name=camera.replace('/', '_') + '_decoder',
            parameters=[params_file],
            remappings=[
                ('compressed', f'/{camera}/image_compressed_raw'),
                ('image_raw', f'/{camera}/image_raw'),
            ],
            extra_arguments=[{'use_intra_process_comms': True}],
        )
        for camera in CAMERAS
    ]

    container = ComposableNodeContainer(
        name='compressed_image_decoder_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=decoders,
        output='screen',
    )

    ld = LaunchDescription([params_arg])
    ld.add_action(container)
    return ld
