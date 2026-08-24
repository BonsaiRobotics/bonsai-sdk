// SPDX-License-Identifier: Apache-2.0
//
// Helpers for choosing a libav decoder for a `sensor_msgs/CompressedImage` stream.

#ifndef COMPRESSED_IMAGE_DECODER__UTILS_HPP_
#define COMPRESSED_IMAGE_DECODER__UTILS_HPP_

#include <string>
#include <vector>

namespace compressed_image_decoder
{

// Returns the libav decoder name for a `sensor_msgs/CompressedImage` `format`,
// or an empty string when it is not H.265/HEVC or H.264.
std::string libav_codec_name(const std::string & format);

// Returns the libav decoder names to try for `codec`, in order. A non-empty
// `preferred` is used as given, otherwise the hardware decoders come first and
// the software decoders second.
std::vector<std::string> decoder_candidates(
  const std::string & codec, const std::string & preferred);

}  // namespace compressed_image_decoder

#endif  // COMPRESSED_IMAGE_DECODER__UTILS_HPP_
