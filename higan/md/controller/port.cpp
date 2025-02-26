ControllerPort controllerPort1{"Controller Port 1"};
ControllerPort controllerPort2{"Controller Port 2"};
ControllerPort extensionPort{"Extension Port"};

ControllerPort::ControllerPort(string_view name) : name(name) {
}

auto ControllerPort::load(Node::Object parent, Node::Object from) -> void {
  port = Node::append<Node::Port>(parent, from, name, "Controller");
  port->hotSwappable = true;
  port->attach = [&](auto node) { connect(node); };
  port->detach = [&](auto node) { disconnect(); };
  port->scan(from);
}

auto ControllerPort::connect(Node::Peripheral node) -> void {
  disconnect();
  if(node) {
    if(node->name == "Control Pad" ) device = new ControlPad(port, node);
    if(node->name == "Fighting Pad") device = new FightingPad(port, node);
  }
}

auto ControllerPort::disconnect() -> void {
  device = {};
}

auto ControllerPort::power() -> void {
  control = 0x00;
}

auto ControllerPort::serialize(serializer& s) -> void {
  s.integer(control);
}
