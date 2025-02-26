struct System {
  Node::Object node;
  Node::String regionNode;

  struct Controls {
    Node::Object node;

    //Master System
    Node::Button pause;
    Node::Button reset;

    //Game Gear
    Node::Button up;
    Node::Button down;
    Node::Button left;
    Node::Button right;
    Node::Button one;
    Node::Button two;
    Node::Button start;

    auto load(Node::Object, Node::Object) -> void;
    auto poll() -> void;

    bool yHold = 0;
    bool upLatch = 0;
    bool downLatch = 0;
    bool xHold = 0;
    bool leftLatch = 0;
    bool rightLatch = 0;
  } controls;

  struct Video {
    Node::Video node;
    Node::Boolean interframeBlending;  //Game Gear only

    //video.cpp
    auto load(Node::Object, Node::Object) -> void;
    auto colorMasterSystem(uint32) -> uint64;
    auto colorGameGear(uint32) -> uint64;
  } video;

  enum class Model : uint { MasterSystem, GameGear };
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
    Model model = Model::MasterSystem;
    Region region = Region::NTSC;
    double colorburst = Constants::Colorburst::NTSC;
    uint serializeSize = 0;
  } information;
};

extern System system;

auto Model::MasterSystem() -> bool { return system.model() == System::Model::MasterSystem; }
auto Model::GameGear() -> bool { return system.model() == System::Model::GameGear; }

auto Region::NTSC() -> bool { return system.region() == System::Region::NTSC; }
auto Region::PAL() -> bool { return system.region() == System::Region::PAL; }
