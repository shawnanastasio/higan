Justifiers::Justifiers(Node::Port parent, Node::Peripheral with) {
  node = Node::append<Node::Peripheral>(parent, with, "Justifiers");

  x1       = Node::append<Node::Axis  >(node, with, "Player 1: X");
  y1       = Node::append<Node::Axis  >(node, with, "Player 1: Y");
  trigger1 = Node::append<Node::Button>(node, with, "Player 1: Trigger");
  start1   = Node::append<Node::Button>(node, with, "Player 1: Start");
  x2       = Node::append<Node::Axis  >(node, with, "Player 2: X");
  y2       = Node::append<Node::Axis  >(node, with, "Player 2: Y");
  trigger2 = Node::append<Node::Button>(node, with, "Player 2: Trigger");
  start2   = Node::append<Node::Button>(node, with, "Player 2: Start");

  sprite1.create(32, 32);
  sprite1.setPixels(Resource::Sprite::CrosshairGreen);
  ppu.display.append(sprite1);

  sprite2.create(32, 32);
  sprite2.setPixels(Resource::Sprite::CrosshairRed);
  ppu.display.append(sprite2);

  Thread::create(system.cpuFrequency(), {&Justifiers::main, this});
  cpu.peripherals.append(this);
}

Justifiers::~Justifiers() {
  cpu.peripherals.removeByValue(this);
  ppu.display.remove(sprite1);
  ppu.display.remove(sprite2);
}

auto Justifiers::main() -> void {
  uint next = cpu.vcounter() * 1364 + cpu.hcounter();

  int px = active == 0 ? x1->value : x2->value;
  int py = active == 0 ? y1->value : y2->value;
  bool offscreen = px < 0 || py < 0 || px >= 256 || py >= ppu.vdisp();

  if(!offscreen) {
    uint target = py * 1364 + (px + 24) * 4;
    if(next >= target && previous < target) {
      //CRT raster detected, strobe iobit to latch counters
      iobit(0);
      iobit(1);
    }
  }

  if(next < previous) {
    platform->input(x1);
    platform->input(y1);
    int nx1 = x1->value + cx1;
    int ny1 = y1->value + cy1;
    cx1 = max(-16, min(256 + 16, nx1));
    cy1 = max(-16, min(240 + 16, ny1));
    sprite1.setPosition(cx1 * 2 - 16, cy1 * 2 - 16);
    sprite1.setVisible(true);

    platform->input(x2);
    platform->input(y2);
    int nx2 = x2->value + cx2;
    int ny2 = y2->value + cy2;
    cx2 = max(-16, min(256 + 16, nx2));
    cy2 = max(-16, min(240 + 16, ny2));
    sprite2.setPosition(cx2 * 2 - 16, cy2 * 2 - 16);
    sprite2.setVisible(true);
  }

  previous = next;
  step(2);
  synchronize(cpu);
}

auto Justifiers::data() -> uint2 {
  if(counter == 0) {
    platform->input(trigger1);
    platform->input(start1);

    platform->input(trigger2);
    platform->input(start2);
  }

  switch(counter++) {
  case  0: return 0;
  case  1: return 0;
  case  2: return 0;
  case  3: return 0;
  case  4: return 0;
  case  5: return 0;
  case  6: return 0;
  case  7: return 0;
  case  8: return 0;
  case  9: return 0;
  case 10: return 0;
  case 11: return 0;

  case 12: return 1;  //4-bit device signature
  case 13: return 1;
  case 14: return 1;
  case 15: return 0;

  case 16: return 0;
  case 17: return 1;
  case 18: return 0;
  case 19: return 1;
  case 20: return 0;
  case 21: return 1;
  case 22: return 0;
  case 23: return 1;

  case 24: return trigger1->value;
  case 25: return trigger2->value;
  case 26: return start1->value;
  case 27: return start2->value;
  case 28: return active;
  case 29: return 0;
  case 30: return 0;
  case 31: return 0;
  }

  if(counter > 32) counter = 32;
  return 1;
}

auto Justifiers::latch(bool data) -> void {
  if(latched != data) {
    latched = data;
    counter = 0;
    if(!latched) active = !active;
  }
}
