// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "compressed_image_decoder/compressed_image_decoder.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  // Intra-process comms let a subscriber composed into this process read the
  // `Image` without a copy.
  rclcpp::NodeOptions options;
  options.use_intra_process_comms(true);
  rclcpp::spin(std::make_shared<compressed_image_decoder::CompressedImageDecoder>(options));
  rclcpp::shutdown();
  return 0;
}
