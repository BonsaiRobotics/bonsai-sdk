// SPDX-License-Identifier: Apache-2.0

#include "compressed_image_decoder/compressed_image_decoder.hpp"

#include <algorithm>
#include <string>

#include <ffmpeg_encoder_decoder/utils.hpp>
#include <rclcpp_components/register_node_macro.hpp>

#include "compressed_image_decoder/utils.hpp"

namespace compressed_image_decoder
{

constexpr int kLogThrottleMs = 5000;

CompressedImageDecoder::CompressedImageDecoder(const rclcpp::NodeOptions & options)
: Node("compressed_image_decoder", options)
{
  decoder_name_ = declare_parameter<std::string>("decoder", "");
  output_encoding_ = declare_parameter<std::string>("output_encoding", "bgr8");
  const int depth = declare_parameter<int>("queue_depth", 4);
  const bool reliable = declare_parameter<bool>("reliable_input", true);

  // Validate `output_encoding` now, so a bad value fails at startup and not on the
  // first frame.
  ffmpeg_encoder_decoder::utils::ros_to_av_pix_format(output_encoding_);

  // Reliable by default: a dropped packet corrupts the decode until the next
  // keyframe. Set `reliable_input` false only for a best-effort publisher.
  rclcpp::QoS qos(rclcpp::KeepLast(static_cast<size_t>(std::max(1, depth))));
  if (reliable) {
    qos.reliable();
  } else {
    qos.best_effort();
  }

  pub_ = create_publisher<sensor_msgs::msg::Image>("image_raw", rclcpp::QoS(rclcpp::KeepLast(1)));
  sub_ = create_subscription<sensor_msgs::msg::CompressedImage>(
    "compressed", qos,
    [this](sensor_msgs::msg::CompressedImage::ConstSharedPtr msg) { onImage(msg); });

  decoder_.setLogger(get_logger());
}

CompressedImageDecoder::~CompressedImageDecoder()
{
  // Deliver any frames still buffered in libav.
  if (decoder_.isInitialized()) {
    decoder_.flush();
  }
}

void CompressedImageDecoder::onImage(
  const sensor_msgs::msg::CompressedImage::ConstSharedPtr & msg)
{
  const std::string codec = libav_codec_name(msg->format);
  if (codec.empty()) {
    RCLCPP_WARN_ONCE(get_logger(), "unsupported format '%s'", msg->format.c_str());
    return;
  }

  // Start a decoder only when the codec changes.
  if (codec != active_codec_) {
    active_codec_ = codec;
    decoder_ready_ = startDecoder(codec);
  }
  if (!decoder_ready_) {
    RCLCPP_ERROR_THROTTLE(
      get_logger(), *get_clock(), kLogThrottleMs, "no decoder for '%s'", codec.c_str());
    return;
  }

  // The decoder can emit frames out of order, so `pts_` keys each packet and lets
  // it copy the source header onto the matching frame. Each `Image` keeps the
  // timestamp and frame_id of its `CompressedImage`.
  if (!decoder_.decodePacket(
      codec, msg->data.data(), msg->data.size(), pts_++, msg->header.frame_id, msg->header.stamp))
  {
    RCLCPP_WARN_THROTTLE(
      get_logger(), *get_clock(), kLogThrottleMs, "decode failed for '%s'", codec.c_str());
  }
}

bool CompressedImageDecoder::startDecoder(const std::string & codec)
{
  if (decoder_.isInitialized()) {
    decoder_.reset();
  }
  // `reset()` clears the output encoding, so set it again for each decoder.
  decoder_.setOutputMessageEncoding(output_encoding_);

  for (const std::string & name : decoder_candidates(codec, decoder_name_)) {
    const bool started = decoder_.initialize(
      codec,
      [this](const sensor_msgs::msg::Image::ConstSharedPtr & image, bool, const std::string &) {
        pub_->publish(*image);
      },
      name);
    if (started) {
      RCLCPP_INFO(get_logger(), "decoding '%s' with '%s'", codec.c_str(), name.c_str());
      return true;
    }
  }
  return false;
}

}  // namespace compressed_image_decoder

RCLCPP_COMPONENTS_REGISTER_NODE(compressed_image_decoder::CompressedImageDecoder)
