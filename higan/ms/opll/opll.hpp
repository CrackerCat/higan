struct OPLL : YM2413, Thread {
  Stream stream;

  //opll.cpp
  auto main() -> void;
  auto step(uint clocks) -> void;
  auto power() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;
};

extern OPLL opll;
