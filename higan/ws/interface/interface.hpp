#if defined(CORE_WS)

namespace higan::WonderSwan {

extern Interface* interface;

struct AbstractInterface : Interface {
  auto game() -> string override;

  auto root() -> Node::Object override;
  auto load(string tree = {}) -> void override;
  auto unload() -> void override;
  auto save() -> void override;
  auto power() -> void override;
  auto run() -> void override;

  auto serialize() -> serializer override;
  auto unserialize(serializer&) -> bool override;
};

struct WonderSwanInterface : AbstractInterface {
  auto name() -> string override { return "WonderSwan"; }
};

struct WonderSwanColorInterface : AbstractInterface {
  auto name() -> string override { return "WonderSwan Color"; }
};

struct SwanCrystalInterface : AbstractInterface {
  auto name() -> string override { return "SwanCrystal"; }
};

struct PocketChallengeV2Interface : AbstractInterface {
  auto name() -> string override { return "Pocket Challenge V2"; }
};

}

#endif
