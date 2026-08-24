// SPDX-License-Identifier: Apache-2.0

#ifndef COMPRESSED_IMAGE_DECODER__COMPRESSED_IMAGE_DECODER_HPP_
#define COMPRESSED_IMAGE_DECODER__COMPRESSED_IMAGE_DECODER_HPP_

#include <cstdint>
#include <string>

#include <ffmpeg_encoder_decoder/decoder.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/image.hpp>

namespace compressed_image_decoder
{

// Decodes `sensor_msgs/CompressedImage` messages into `sensor_msgs/Image`
// messages. Subscribes to `compressed`, publishes on `image_raw`, and gives each
// `Image` the timestamp and frame_id of the `CompressedImage` it decoded.
// See README.md for the parameters and the launch files.
class CompressedImageDecoder : public rclcpp::Node
{
public:
  explicit CompressedImageDecoder(const rclcpp::NodeOptions & options);
  ~CompressedImageDecoder() override;

private:
  void onImage(const sensor_msgs::msg::CompressedImage::ConstSharedPtr & msg);
  bool startDecoder(const std::string & codec);

  ffmpeg_encoder_decoder::Decoder decoder_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
  rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr sub_;
  std::string decoder_name_;    // Preferred libav decoder. Empty selects one automatically.
  std::string output_encoding_;
  std::string active_codec_;    // Codec of the running decoder. Empty until one starts.
  bool decoder_ready_{false};   // True once a decoder started for `active_codec_`.
  uint64_t pts_{0};             // Per-packet key the decoder uses to match a frame to its source.
};

}  // namespace compressed_image_decoder

#endif  // COMPRESSED_IMAGE_DECODER__COMPRESSED_IMAGE_DECODER_HPP_
