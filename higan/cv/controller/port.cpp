ControllerPort controllerPort1{"Controller Port 1"};
ControllerPort controllerPort2{"Controller Port 2"};

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
    if(node->name == "Gamepad") device = new Gamepad(port, node);
  }
}

auto ControllerPort::disconnect() -> void {
  device = {};
}

auto ControllerPort::serialize(serializer& s) -> void {
}
