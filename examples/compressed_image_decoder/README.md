# compressed_image_decoder

Decodes `sensor_msgs/CompressedImage` (H.265/HEVC or H.264) into `sensor_msgs/Image`, with
hardware acceleration when available.

## Install

Decoding needs [`ffmpeg_encoder_decoder`][ffmpeg]:

```
sudo apt install ros-$ROS_DISTRO-ffmpeg-encoder-decoder
```

Then build:

```
colcon build --packages-select compressed_image_decoder
```

## Run

```
ros2 launch compressed_image_decoder decode.launch.py       # one camera
ros2 launch compressed_image_decoder decode_all.launch.py   # all four cameras
```

By default, `decode.launch.py` decodes `/camera_front_center/left/image_compressed_raw`.
To decode a different stream, pass its topic as `compressed:=<topic>`. The decoded images
are published on `image_raw`, and the decoder's parameters are set in
`config/decoder.yaml`.

[ffmpeg]: https://github.com/ros-misc-utilities/ffmpeg_encoder_decoder
