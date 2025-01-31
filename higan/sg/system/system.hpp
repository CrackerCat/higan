struct System {
  Node::Object node;
  Node::String regionNode;

  struct Controls {
    Node::Object node;
    Node::Button pause;

    //controls.cpp
    auto load(Node::Object, Node::Object) -> void;
    auto poll() -> void;
  } controls;

  struct Video {
    Node::Video node;

    //video.cpp
    auto load(Node::Object, Node::Object) -> void;
    auto color(uint32) -> uint64;
  } video;

  enum class Model : uint { SG1000, SC3000 };
  enum class Region : uint { NTSC, PAL };

  inline auto model() const -> Model { return information.model; }
  inline auto region() const -> Region { return information.region; }
  inline auto colorburst() const -> double { return information.colorburst; }

  //system.cpp
  auto run() -> void;
  auto runToSave() -> void;

  auto load(Node::Object) -> void;
  auto save() -> void;
  auto unload() -> void;
  auto power() -> void;

  //serialization.cpp
  auto serializeInit() -> void;
  auto serialize() -> serializer;
  auto unserialize(serializer&) -> bool;
  auto serializeAll(serializer&) -> void;
  auto serialize(serializer&) -> void;

private:
  struct Information {
    Model model = Model::SG1000;
    Region region = Region::NTSC;
    double colorburst = Constants::Colorburst::NTSC;
    uint serializeSize = 0;
  } information;
};

extern System system;

auto Model::SG1000() -> bool { return system.model() == System::Model::SG1000; }
auto Model::SC3000() -> bool { return system.model() == System::Model::SC3000; }

auto Region::NTSC() -> bool { return system.region() == System::Region::NTSC; }
auto Region::PAL() -> bool { return system.region() == System::Region::PAL; }
