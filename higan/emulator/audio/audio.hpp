#pragma once

#include <nall/dsp/iir/one-pole.hpp>
#include <nall/dsp/iir/biquad.hpp>
#include <nall/dsp/resampler/cubic.hpp>

namespace higan {

struct Audio;
struct Filter;
struct Stream;

struct Audio {
  Audio() = default;
  Audio(const Audio&) = delete;
  auto operator=(const Audio&) = delete;

  auto reset(Interface* interface) -> void;
  auto append(Stream& stream) -> void;
  auto remove(Stream& stream) -> void;

  auto setFrequency(double frequency) -> void;
  auto setVolume(double volume) -> void;
  auto setBalance(double balance) -> void;

  vector<Stream*> streams;

private:
  auto synchronize() -> void;
  auto process() -> void;

  Interface* interface = nullptr;

  uint channels = 0;
  double frequency = 48000.0;

  double volume = 1.0;
  double balance = 0.0;

  friend class Stream;
};

struct Filter {
  enum class Mode : uint { OnePole, Biquad } mode;
  enum class Type : uint { None, LowPass, HighPass, LowShelf, HighShelf } type;
  enum class Order : uint { None, First, Second } order;

  DSP::IIR::OnePole onePole;
  DSP::IIR::Biquad biquad;
};

struct Stream {
  Stream() = default;
  Stream(const Stream&) = delete;
  auto operator=(const Stream&) = delete;

  auto create(uint channels, double frequency) -> void;
  auto destroy() -> void;

  auto setChannels(uint channels) -> void;
  auto setFrequency(double inputFrequency, maybe<double> outputFrequency = {}) -> void;

  auto resetFilters() -> void;
  auto addLowPassFilter(double cutoffFrequency, Filter::Order, uint passes = 1) -> void;
  auto addHighPassFilter(double cutoffFrequency, Filter::Order, uint passes = 1) -> void;
  auto addLowShelfFilter(double cutoffFrequency, Filter::Order, double gain, double slope) -> void;
  auto addHighShelfFilter(double cutoffFrequency, Filter::Order, double gain, double slope) -> void;

  auto pending() const -> bool;
  auto read(double samples[]) -> uint;
  auto write(const double samples[]) -> void;

  template<typename... P>
  auto sample(P&&... p) -> void {
    double samples[sizeof...(P)] = {forward<P>(p)...};
    write(samples);
  }

private:
  struct Channel {
    vector<Filter> filters;
    vector<DSP::IIR::Biquad> nyquist;
    DSP::Resampler::Cubic resampler;
  };
  vector<Channel> channels;
  double inputFrequency;
  double outputFrequency;

  friend class Audio;
};

extern Audio audio;

}
