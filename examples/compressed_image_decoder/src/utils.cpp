// SPDX-License-Identifier: Apache-2.0

#include "compressed_image_decoder/utils.hpp"

#include <algorithm>
#include <cctype>

#include <ffmpeg_encoder_decoder/decoder.hpp>
#include <ffmpeg_encoder_decoder/utils.hpp>

namespace compressed_image_decoder
{

std::string libav_codec_name(const std::string & format)
{
  std::string name = format.substr(0, format.find(';'));
  const auto not_space = [](unsigned char c) { return std::isspace(c) == 0; };
  name.erase(name.begin(), std::find_if(name.begin(), name.end(), not_space));
  name.erase(std::find_if(name.rbegin(), name.rend(), not_space).base(), name.end());
  std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });

  if (name == "h265" || name == "hevc") {
    return "hevc";
  }
  if (name == "h264" || name == "avc" || name == "avc1") {
    return "h264";
  }
  return {};
}

std::vector<std::string> decoder_candidates(
  const std::string & codec, const std::string & preferred)
{
  if (!preferred.empty()) {
    return ffmpeg_encoder_decoder::utils::split_decoders(preferred);
  }

  std::vector<std::string> hardware;
  std::vector<std::string> software;
  ffmpeg_encoder_decoder::Decoder::findDecoders(codec, &hardware, &software);
  hardware.reserve(hardware.size() + software.size());
  hardware.insert(hardware.end(), software.begin(), software.end());
  return hardware;
}

}  // namespace compressed_image_decoder
