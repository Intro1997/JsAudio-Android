#include "ChannelMixer.hpp"
#include <cmath>
#include <type_traits>

enum class ChannelType : size_t { Mono = 1, Stereo = 2, Quad = 4, FiveOne = 6 };
enum class ChannelName { Left = 0, Right, SideLeft, SideRight, Center, LFE };

using ChannelBuffer = std::vector<std::vector<float>>;
using Channel = std::vector<float>;
static ChannelBuffer DoDiscreteMix(const ChannelBuffer &buffer,
                                   const size_t &aim_channel_size);
static ChannelBuffer DoSpeakerMix(const ChannelBuffer &buffer,
                                  const size_t &aim_channel_size);
static ChannelBuffer DoMonoSpeakerMix(const ChannelBuffer &buffer,
                                      const size_t &aim_channel_size);
static ChannelBuffer DoStereoSpeakerMix(const ChannelBuffer &buffer,

                                        const size_t &aim_channel_size);
static ChannelBuffer DoQuadSpeakerMix(const ChannelBuffer &buffer,
                                      const size_t &aim_channel_size);
static ChannelBuffer DoFiveOneSpeakerMix(const ChannelBuffer &buffer,
                                         const size_t &aim_channel_size);

template <typename T>
decltype(auto) GetChannel(T &buffer, const ChannelName &name);

static ChannelBuffer GetOutputBuffer(const ChannelType &type);

static bool IsValidBufferSize(const size_t buffer_size);

class MonoMixer {
public:
  static ChannelBuffer ToStereo(const ChannelBuffer &mono_buffer);
  static ChannelBuffer ToQuad(const ChannelBuffer &mono_buffer);
  static ChannelBuffer ToFiveOne(const ChannelBuffer &mono_buffer);
};
class StereoMixer {
public:
  static ChannelBuffer ToMono(const ChannelBuffer &stereo_buffer);
  static ChannelBuffer ToQuad(const ChannelBuffer &stereo_buffer);
  static ChannelBuffer ToFiveOne(const ChannelBuffer &stereo_buffer);
};
class QuadMixer {
public:
  static ChannelBuffer ToMono(const ChannelBuffer &quad_buffer);
  static ChannelBuffer ToStereo(const ChannelBuffer &quad_buffer);
  static ChannelBuffer ToFiveOne(const ChannelBuffer &quad_buffer);
};
class FiveOneMixer {
public:
  static ChannelBuffer ToMono(const ChannelBuffer &five_one_buffer);
  static ChannelBuffer ToStereo(const ChannelBuffer &five_one_buffer);
  static ChannelBuffer ToQuad(const ChannelBuffer &five_one_buffer);
};

namespace js_audio {
using ChannelInterpretation = AudioNode::ChannelInterpretation;

ChannelBuffer ChannelMixer::DoMix(const ChannelBuffer &buffer,
                                  const ChannelInterpretation &interpretation,
                                  const size_t &aim_channel_size) {
  if (IsValidBufferSize(buffer.size()) &&    // these both should valid
      IsValidBufferSize(aim_channel_size) && // these both should valid
      interpretation == ChannelInterpretation::kSpeakers) {
    return DoSpeakerMix(buffer, aim_channel_size);
  } else {
    return DoDiscreteMix(buffer, aim_channel_size);
  }
}
} // namespace js_audio
static ChannelBuffer DoSpeakerMix(const ChannelBuffer &buffer,
                                  const size_t &aim_channel_size) {
  const ChannelType buffer_channel_type = (ChannelType)buffer.size();
  switch (buffer_channel_type) {
  case ChannelType::Mono: {
    return DoMonoSpeakerMix(buffer, aim_channel_size);
  }
  case ChannelType::Stereo: {
    return DoStereoSpeakerMix(buffer, aim_channel_size);
  }
  case ChannelType::Quad: {
    return DoQuadSpeakerMix(buffer, aim_channel_size);
  }
  case ChannelType::FiveOne: {
    return DoFiveOneSpeakerMix(buffer, aim_channel_size);
  }
  default:
    return DoDiscreteMix(buffer, aim_channel_size);
  }
}

static ChannelBuffer DoDiscreteMix(const ChannelBuffer &buffer,
                                   const size_t &aim_channel_size) {
  if (buffer.size() == aim_channel_size) {
    return buffer;
  }
  ChannelBuffer output_buffer;
  const size_t insert_channel_size = std::min(aim_channel_size, buffer.size());
  output_buffer.insert(output_buffer.end(), buffer.begin(),
                       buffer.begin() + insert_channel_size);

  const size_t sample_size = buffer[0].size();
  if (aim_channel_size > insert_channel_size) {
    for (size_t c = aim_channel_size; c < insert_channel_size; c++) {
      output_buffer.push_back({});
      output_buffer[c].resize(sample_size);
    }
  }
  return output_buffer;
}

static ChannelBuffer DoMonoSpeakerMix(const ChannelBuffer &buffer,
                                      const size_t &aim_channel_size) {
  if (buffer.size() != (size_t)ChannelType::Mono &&
      !IsValidBufferSize(aim_channel_size)) {
    return DoDiscreteMix(buffer, aim_channel_size);
  }

  switch (ChannelType(aim_channel_size)) {
  case ChannelType::Mono: {
    return buffer;
  }
  case ChannelType::Stereo: {
    return MonoMixer::ToStereo(buffer);
  }
  case ChannelType::Quad: {
    return MonoMixer::ToQuad(buffer);
  }
  case ChannelType::FiveOne: {
    return MonoMixer::ToFiveOne(buffer);
  }
  }
  return DoDiscreteMix(buffer, aim_channel_size);
}
static ChannelBuffer DoStereoSpeakerMix(const ChannelBuffer &buffer,

                                        const size_t &aim_channel_size) {
  if (buffer.size() != (size_t)ChannelType::Stereo &&
      !IsValidBufferSize(aim_channel_size)) {
    return DoDiscreteMix(buffer, aim_channel_size);
  }

  switch (ChannelType(aim_channel_size)) {
  case ChannelType::Mono: {
    return StereoMixer::ToMono(buffer);
  }
  case ChannelType::Stereo: {
    return buffer;
  }
  case ChannelType::Quad: {
    return StereoMixer::ToQuad(buffer);
  }
  case ChannelType::FiveOne: {
    return StereoMixer::ToFiveOne(buffer);
  }
  }
  return DoDiscreteMix(buffer, aim_channel_size);
}
static ChannelBuffer DoQuadSpeakerMix(const ChannelBuffer &buffer,
                                      const size_t &aim_channel_size) {
  if (buffer.size() != (size_t)ChannelType::Quad &&
      !IsValidBufferSize(aim_channel_size)) {
    return DoDiscreteMix(buffer, aim_channel_size);
  }

  switch (ChannelType(aim_channel_size)) {
  case ChannelType::Mono: {
    return QuadMixer::ToMono(buffer);
  }
  case ChannelType::Stereo: {
    return QuadMixer::ToStereo(buffer);
  }
  case ChannelType::Quad: {
    return buffer;
  }
  case ChannelType::FiveOne: {
    return QuadMixer::ToFiveOne(buffer);
  }
  }
  return DoDiscreteMix(buffer, aim_channel_size);
}
static ChannelBuffer DoFiveOneSpeakerMix(const ChannelBuffer &buffer,
                                         const size_t &aim_channel_size) {
  if (buffer.size() != (size_t)ChannelType::FiveOne &&
      !IsValidBufferSize(aim_channel_size)) {
    return DoDiscreteMix(buffer, aim_channel_size);
  }
  switch (ChannelType(aim_channel_size)) {
  case ChannelType::Mono: {
    return FiveOneMixer::ToMono(buffer);
  }
  case ChannelType::Stereo: {
    return FiveOneMixer::ToStereo(buffer);
  }
  case ChannelType::Quad: {
    return FiveOneMixer::ToQuad(buffer);
  }
  case ChannelType::FiveOne: {
    return buffer;
  }
  }
  return DoDiscreteMix(buffer, aim_channel_size);
}

template <typename T>
decltype(auto) GetChannel(T &buffer, const ChannelName &name) {
  decltype(&buffer[0]) kNullPtr = nullptr;
  if (buffer.empty()) {
    return kNullPtr;
  }

  switch (name) {
  case ChannelName::Left: {
    if (buffer.size() >= 1) {
      return &buffer[0];
    }
    break;
  }
  case ChannelName::Right: {
    if (buffer.size() >= 2) {
      return &buffer[1];
    }
    break;
  }
  case ChannelName::SideLeft: {
    if (buffer.size() >= 6) {
      return &buffer[4];
    } else if (buffer.size() >= 4) {
      return &buffer[2];
    }
    break;
  }
  case ChannelName::SideRight: {
    if (buffer.size() >= 6) {
      return &buffer[5];
    } else if (buffer.size() >= 4) {
      return &buffer[3];
    }
    break;
  }
  case ChannelName::Center: {
    if (buffer.size() >= 6) {
      return &buffer[2];
    }
    break;
  }
  case ChannelName::LFE: {
    if (buffer.size() >= 6) {
      return &buffer[3];
    }
    break;
  }
  default:
    break;
  }
  return kNullPtr;
}

static ChannelBuffer GetOutputBuffer(const ChannelType &type) {
  const size_t size = (size_t)type;
  ChannelBuffer output_buffer(size);
  return output_buffer;
}

static bool IsValidBufferSize(const size_t buffer_size) {
  return !(buffer_size ^ (size_t)ChannelType::Mono) ||
         !(buffer_size ^ (size_t)ChannelType::Stereo) ||
         !(buffer_size ^ (size_t)ChannelType::Quad) ||
         !(buffer_size ^ (size_t)ChannelType::FiveOne);
}

ChannelBuffer MonoMixer::ToStereo(const ChannelBuffer &mono_buffer) {
  const Channel *mono_buffer_left_channel =
      GetChannel(mono_buffer, ChannelName::Left);
  if (!mono_buffer_left_channel) {
    return DoDiscreteMix(mono_buffer, (size_t)ChannelType::Stereo);
  }

  ChannelBuffer output_buffer((size_t)ChannelType::Stereo);
  Channel *output_buffer_left_channel =
      GetChannel(output_buffer, ChannelName::Left);
  Channel *output_buffer_right_channel =
      GetChannel(output_buffer, ChannelName::Right);
  *output_buffer_left_channel = *mono_buffer_left_channel;
  *output_buffer_right_channel = *mono_buffer_left_channel;

  return output_buffer;
}
ChannelBuffer MonoMixer::ToQuad(const ChannelBuffer &mono_buffer) {
  const Channel *mono_buffer_left_channel =
      GetChannel(mono_buffer, ChannelName::Left);
  if (!mono_buffer_left_channel) {
    return DoDiscreteMix(mono_buffer, (size_t)ChannelType::Quad);
  }

  ChannelBuffer output_buffer((size_t)ChannelType::Quad);
  Channel *output_buffer_left_channel =
      GetChannel(output_buffer, ChannelName::Left);
  Channel *output_buffer_right_channel =
      GetChannel(output_buffer, ChannelName::Right);
  *output_buffer_left_channel = *mono_buffer_left_channel;
  *output_buffer_right_channel = *mono_buffer_left_channel;

  return output_buffer;
}
ChannelBuffer MonoMixer::ToFiveOne(const ChannelBuffer &mono_buffer) {
  const Channel *mono_buffer_left_channel =
      GetChannel(mono_buffer, ChannelName::Left);
  if (!mono_buffer_left_channel) {
    return DoDiscreteMix(mono_buffer, (size_t)ChannelType::FiveOne);
  }

  ChannelBuffer output_buffer((size_t)ChannelType::FiveOne);
  Channel *output_buffer_center_channel =
      GetChannel(output_buffer, ChannelName::Center);
  *output_buffer_center_channel = *mono_buffer_left_channel;

  return output_buffer;
}

ChannelBuffer StereoMixer::ToMono(const ChannelBuffer &stereo_buffer) {
  const Channel *stereo_buffer_left_channel_ptr =
      GetChannel(stereo_buffer, ChannelName::Left);
  const Channel *stereo_buffer_right_channel_ptr =
      GetChannel(stereo_buffer, ChannelName::Right);
  if (!stereo_buffer_left_channel_ptr || !stereo_buffer_right_channel_ptr) {
    return DoDiscreteMix(stereo_buffer, (size_t)ChannelType::Mono);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Mono);
  Channel &output_buffer_left_channel_ptr =
      *GetChannel(output_buffer, ChannelName::Left);
  const Channel &stereo_buffer_left_channel = *stereo_buffer_left_channel_ptr;
  const Channel &stereo_buffer_right_channel = *stereo_buffer_right_channel_ptr;
  const size_t loop_size = std::min(stereo_buffer_left_channel.size(),
                                    stereo_buffer_right_channel_ptr->size());

  for (size_t i = 0; i < loop_size; i++) {
    output_buffer_left_channel_ptr.push_back(
        0.5 * (stereo_buffer_left_channel[i] + stereo_buffer_right_channel[i]));
  }
  return output_buffer;
}
ChannelBuffer StereoMixer::ToQuad(const ChannelBuffer &stereo_buffer) {
  const Channel *stereo_buffer_left_channel =
      GetChannel(stereo_buffer, ChannelName::Left);
  const Channel *stereo_buffer_right_channel =
      GetChannel(stereo_buffer, ChannelName::Right);
  if (!stereo_buffer_left_channel || !stereo_buffer_right_channel) {
    return DoDiscreteMix(stereo_buffer, (size_t)ChannelType::Quad);
  }
  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Quad);
  Channel *output_buffer_left_channel =
      GetChannel(output_buffer, ChannelName::Left);
  Channel *output_buffer_right_channel =
      GetChannel(output_buffer, ChannelName::Right);

  *output_buffer_left_channel = *stereo_buffer_left_channel;
  *output_buffer_right_channel = *stereo_buffer_right_channel;

  return output_buffer;
}
ChannelBuffer StereoMixer::ToFiveOne(const ChannelBuffer &stereo_buffer) {
  const Channel *stereo_buffer_left_channel =
      GetChannel(stereo_buffer, ChannelName::Left);
  const Channel *stereo_buffer_right_channel =
      GetChannel(stereo_buffer, ChannelName::Right);
  if (!stereo_buffer_left_channel || !stereo_buffer_right_channel) {
    return DoDiscreteMix(stereo_buffer, (size_t)ChannelType::FiveOne);
  }
  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::FiveOne);
  Channel *output_buffer_left_channel =
      GetChannel(output_buffer, ChannelName::Left);
  Channel *output_buffer_right_channel =
      GetChannel(output_buffer, ChannelName::Right);
  *output_buffer_left_channel = *stereo_buffer_left_channel;
  *output_buffer_right_channel = *stereo_buffer_right_channel;
  return output_buffer;
}

ChannelBuffer QuadMixer::ToMono(const ChannelBuffer &quad_buffer) {
  const Channel *src_left_ptr = GetChannel(quad_buffer, ChannelName::Left);
  const Channel *src_right_ptr = GetChannel(quad_buffer, ChannelName::Right);
  const Channel *src_side_left_ptr =
      GetChannel(quad_buffer, ChannelName::SideLeft);
  const Channel *src_side_right_ptr =
      GetChannel(quad_buffer, ChannelName::SideRight);
  if (!src_left_ptr || !src_right_ptr || !src_side_left_ptr ||
      !src_side_right_ptr) {
    return DoDiscreteMix(quad_buffer, 1);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Mono);
  Channel &dst_left_ref = *GetChannel(output_buffer, ChannelName::Left);
  const Channel &src_left_ref = *src_left_ptr;
  const Channel &src_right_ref = *src_right_ptr;
  const Channel &src_side_left_ref = *src_side_left_ptr;
  const Channel &src_side_right_ref = *src_side_right_ptr;
  const size_t loop_size =
      std::min({src_left_ref.size(), src_right_ref.size(),
                src_side_left_ref.size(), src_side_right_ref.size()});
  for (size_t i = 0; i < loop_size; i++) {
    dst_left_ref.push_back(0.25 *
                           (src_left_ref[i] + src_right_ref[i] +
                            src_side_left_ref[i] + src_side_right_ref[i]));
  }
  return output_buffer;
}
ChannelBuffer QuadMixer::ToStereo(const ChannelBuffer &quad_buffer) {
  const Channel *src_left_ptr = GetChannel(quad_buffer, ChannelName::Left);
  const Channel *src_right_ptr = GetChannel(quad_buffer, ChannelName::Right);
  const Channel *src_side_left_ptr =
      GetChannel(quad_buffer, ChannelName::SideLeft);
  const Channel *src_side_right_ptr =
      GetChannel(quad_buffer, ChannelName::SideRight);
  if (!src_left_ptr || !src_right_ptr || !src_side_left_ptr ||
      !src_side_right_ptr) {
    return DoDiscreteMix(quad_buffer, 1);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Stereo);
  Channel &dst_left_ref = *GetChannel(output_buffer, ChannelName::Left);
  Channel &dst_right_ref = *GetChannel(output_buffer, ChannelName::Right);
  const Channel &src_left_ref = *src_left_ptr;
  const Channel &src_right_ref = *src_right_ptr;
  const Channel &src_side_left_ref = *src_side_left_ptr;
  const Channel &src_side_right_ref = *src_side_right_ptr;
  const size_t loop_size =
      std::min({src_left_ref.size(), src_right_ref.size(),
                src_side_left_ref.size(), src_side_right_ref.size()});
  for (size_t i = 0; i < loop_size; i++) {
    dst_left_ref.push_back(0.5 * (src_left_ref[i] + src_side_left_ref[i]));
    dst_right_ref.push_back(0.5 * (src_right_ref[i] + src_side_right_ref[i]));
  }
  return output_buffer;
}
ChannelBuffer QuadMixer::ToFiveOne(const ChannelBuffer &quad_buffer) {
  const Channel *quad_buffer_left_channel =
      GetChannel(quad_buffer, ChannelName::Left);
  const Channel *quad_buffer_right_channel =
      GetChannel(quad_buffer, ChannelName::Right);
  const Channel *quad_buffer_side_left_channel =
      GetChannel(quad_buffer, ChannelName::SideLeft);
  const Channel *quad_buffer_side_right_channel =
      GetChannel(quad_buffer, ChannelName::SideRight);
  if (!quad_buffer_left_channel || !quad_buffer_right_channel ||
      !quad_buffer_side_left_channel || !quad_buffer_side_right_channel) {
    return DoDiscreteMix(quad_buffer, (size_t)ChannelType::FiveOne);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::FiveOne);
  Channel *output_buffer_left_channel =
      GetChannel(output_buffer, ChannelName::Left);
  Channel *output_buffer_right_channel =
      GetChannel(output_buffer, ChannelName::Right);
  Channel *output_buffer_side_left_channel =
      GetChannel(output_buffer, ChannelName::SideLeft);
  Channel *output_buffer_side_right_channel =
      GetChannel(output_buffer, ChannelName::SideRight);
  *output_buffer_left_channel = *quad_buffer_left_channel;
  *output_buffer_right_channel = *quad_buffer_right_channel;
  *output_buffer_side_left_channel = *quad_buffer_side_left_channel;
  *output_buffer_side_right_channel = *quad_buffer_side_right_channel;
  return output_buffer;
}

ChannelBuffer FiveOneMixer::ToMono(const ChannelBuffer &five_one_buffer) {
  const Channel *src_left_ptr = GetChannel(five_one_buffer, ChannelName::Left);
  const Channel *src_right_ptr =
      GetChannel(five_one_buffer, ChannelName::Right);
  const Channel *src_center_ptr =
      GetChannel(five_one_buffer, ChannelName::Center);
  const Channel *src_side_left_ptr =
      GetChannel(five_one_buffer, ChannelName::SideLeft);
  const Channel *src_side_right_ptr =
      GetChannel(five_one_buffer, ChannelName::SideRight);
  if (!src_left_ptr || !src_right_ptr || !src_center_ptr ||
      !src_side_left_ptr || !src_side_right_ptr) {
    return DoDiscreteMix(five_one_buffer, 1);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Mono);
  Channel &dst_left_channel_ref = *GetChannel(output_buffer, ChannelName::Left);

  const Channel &src_left_ref = *src_left_ptr;
  const Channel &src_right_ref = *src_right_ptr;
  const Channel &src_center_ref = *src_center_ptr;
  const Channel &src_side_left_ref = *src_side_left_ptr;
  const Channel &src_side_right_ref = *src_side_right_ptr;
  const size_t loop_size = std::min(
      {src_left_ref.size(), src_right_ref.size(), src_center_ref.size(),
       src_side_left_ref.size(), src_side_right_ref.size()});
  for (size_t i = 0; i < loop_size; i++) {
    dst_left_channel_ref.push_back(
        std::sqrt(0.5) * (src_left_ref[i] + src_right_ref[i]) +
        src_center_ref[i] +
        0.5 * (src_side_left_ref[i] + src_side_right_ref[i]));
  }

  return output_buffer;
}
ChannelBuffer FiveOneMixer::ToStereo(const ChannelBuffer &five_one_buffer) {
  const Channel *src_left_ptr = GetChannel(five_one_buffer, ChannelName::Left);
  const Channel *src_right_ptr =
      GetChannel(five_one_buffer, ChannelName::Right);
  const Channel *src_center_ptr =
      GetChannel(five_one_buffer, ChannelName::Center);
  const Channel *src_side_left_ptr =
      GetChannel(five_one_buffer, ChannelName::SideLeft);
  const Channel *src_side_right_ptr =
      GetChannel(five_one_buffer, ChannelName::SideRight);
  if (!src_left_ptr || !src_right_ptr || !src_center_ptr ||
      !src_side_left_ptr || !src_side_right_ptr) {
    return DoDiscreteMix(five_one_buffer, 1);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Stereo);
  Channel &dst_left_channel_ref = *GetChannel(output_buffer, ChannelName::Left);
  Channel &dst_right_channel_ref =
      *GetChannel(output_buffer, ChannelName::Right);

  const Channel &src_left_ref = *src_left_ptr;
  const Channel &src_right_ref = *src_right_ptr;
  const Channel &src_center_ref = *src_center_ptr;
  const Channel &src_side_left_ref = *src_side_left_ptr;
  const Channel &src_side_right_ref = *src_side_right_ptr;
  const size_t loop_size = std::min(
      {src_left_ref.size(), src_right_ref.size(), src_center_ref.size(),
       src_side_left_ref.size(), src_side_right_ref.size()});
  for (size_t i = 0; i < loop_size; i++) {
    dst_left_channel_ref.push_back(
        src_left_ref[i] +
        std::sqrt(0.5) * (src_center_ref[i] + src_side_left_ref[i]));
    dst_right_channel_ref.push_back(
        src_right_ref[i] +
        std::sqrt(0.5) * (src_center_ref[i] + src_side_right_ref[i]));
  }
  return output_buffer;
}
ChannelBuffer FiveOneMixer::ToQuad(const ChannelBuffer &five_one_buffer) {
  const Channel *src_left_ptr = GetChannel(five_one_buffer, ChannelName::Left);
  const Channel *src_right_ptr =
      GetChannel(five_one_buffer, ChannelName::Right);
  const Channel *src_center_ptr =
      GetChannel(five_one_buffer, ChannelName::Center);
  const Channel *src_side_left_ptr =
      GetChannel(five_one_buffer, ChannelName::SideLeft);
  const Channel *src_side_right_ptr =
      GetChannel(five_one_buffer, ChannelName::SideRight);
  if (!src_left_ptr || !src_right_ptr || !src_center_ptr ||
      !src_side_left_ptr || !src_side_right_ptr) {
    return DoDiscreteMix(five_one_buffer, 1);
  }

  ChannelBuffer output_buffer = GetOutputBuffer(ChannelType::Quad);
  Channel &dst_left_channel_ref = *GetChannel(output_buffer, ChannelName::Left);
  Channel &dst_right_channel_ref =
      *GetChannel(output_buffer, ChannelName::Right);
  Channel &dst_side_left_channel_ref =
      *GetChannel(output_buffer, ChannelName::SideLeft);
  Channel &dst_side_right_channel_ref =
      *GetChannel(output_buffer, ChannelName::SideRight);

  const Channel &src_left_ref = *src_left_ptr;
  const Channel &src_right_ref = *src_right_ptr;
  const Channel &src_center_ref = *src_center_ptr;
  const Channel &src_side_left_ref = *src_side_left_ptr;
  const Channel &src_side_right_ref = *src_side_right_ptr;
  const size_t loop_size = std::min(
      {src_left_ref.size(), src_right_ref.size(), src_center_ref.size(),
       src_side_left_ref.size(), src_side_right_ref.size()});
  for (size_t i = 0; i < loop_size; i++) {
    dst_left_channel_ref.push_back(src_left_ref[i] +
                                   std::sqrt(0.5) * src_center_ref[i]);
    dst_right_channel_ref.push_back(src_right_ref[i] +
                                    std::sqrt(0.5) * src_center_ref[i]);
    dst_side_left_channel_ref.push_back(src_side_left_ref[i]);
    dst_side_right_channel_ref.push_back(src_side_right_ref[i]);
  }
  return output_buffer;
}