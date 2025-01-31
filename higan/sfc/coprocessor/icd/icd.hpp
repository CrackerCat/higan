#if defined(CORE_GB)

struct ICD : Platform, GameBoy::SuperGameBoyInterface, Thread {
  Stream stream;
  Node::Port port;
  Node::Peripheral node;

  //icd.cpp
  auto name() const -> string;
  auto main() -> void;

  auto load(Node::Peripheral, Node::Peripheral) -> void;
  auto connect(Node::Peripheral) -> void;
  auto disconnect() -> void;
  auto power() -> void;
  auto reset() -> void;  //software reset

  //interface.cpp
  auto ppuScanline() -> void override;
  auto ppuOutput(uint2 color) -> void override;
  auto apuOutput(double left, double right) -> void override;
  auto joypWrite(uint1 p14, uint1 p15) -> void override;

  //io.cpp
  auto readIO(uint24 address, uint8 data) -> uint8;
  auto writeIO(uint24 address, uint8 data) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint Revision = 0;
  uint Frequency = 0;

private:
  struct Packet {
    auto operator[](uint4 address) -> uint8& { return data[address]; }
    uint8 data[16];
  };
  Packet packet[64];
  uint packetSize;

  uint joypID;
  bool joyp15Lock;
  bool joyp14Lock;
  bool pulseLock;
  bool strobeLock;
  bool packetLock;
  Packet joypPacket;
  uint8 packetOffset;
  uint8 bitData, bitOffset;

  uint8 r6003;      //control port
  uint8 r6004;      //joypad 1
  uint8 r6005;      //joypad 2
  uint8 r6006;      //joypad 3
  uint8 r6007;      //joypad 4
  uint8 r7000[16];  //JOYP packet data
  uint8 mltReq;     //number of active joypads

  uint8 output[4 * 512];
  uint readBank;
  uint readAddress;
  uint writeBank;
  uint writeAddress;

  GameBoy::GameBoyInterface gameBoyInterface;
};

#else

struct ICD : Thread {
  Node::Port port;
  Node::Peripheral node;

  auto name() const -> string { return {}; }

  auto load(Node::Peripheral, Node::Peripheral) -> void {}
  auto connect(Node::Peripheral) -> void {}
  auto disconnect() -> void {}
  auto power() -> void {}

  auto readIO(uint24, uint8) -> uint8 { return 0; }
  auto writeIO(uint24, uint8) -> void { return; }

  auto serialize(serializer&) -> void {}

  uint Revision = 0;
  uint Frequency = 0;
};

#endif

extern ICD icd;
