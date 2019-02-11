struct PSG : SN76489, Thread {
  shared_pointer<Stream> stream;

  //psg.cpp
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;
  auto power() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;
};

extern PSG psg;