#include <fc/fc.hpp>

namespace higan::Famicom {

PPU ppu;
#include "memory.cpp"
#include "render.cpp"
#include "serialization.cpp"

auto PPU::load(Node::Object parent, Node::Object from) -> void {
  screen.create(system.video.node);
}

auto PPU::unload() -> void {
  screen.destroy();
}

auto PPU::main() -> void {
  renderScanline();
}

auto PPU::step(uint clocks) -> void {
  uint L = vlines();

  while(clocks--) {
    if(io.ly == 240 && io.lx == 340) io.nmiHold = 1;
    if(io.ly == 241 && io.lx ==   0) io.nmiFlag = io.nmiHold;
    if(io.ly == 241 && io.lx ==   2) cpu.nmiLine(io.nmiEnable && io.nmiFlag);

    if(io.ly == L-2 && io.lx == 340) io.spriteZeroHit = 0, io.spriteOverflow = 0;

    if(io.ly == L-2 && io.lx == 340) io.nmiHold = 0;
    if(io.ly == L-1 && io.lx ==   0) io.nmiFlag = io.nmiHold;
    if(io.ly == L-1 && io.lx ==   2) cpu.nmiLine(io.nmiEnable && io.nmiFlag);

    Thread::step(rate());
    Thread::synchronize(cpu);

    io.lx++;
  }
}

auto PPU::scanline() -> void {
  io.lx = 0;
  if(++io.ly == vlines()) {
    io.ly = 0;
    frame();
  }
  cartridge.scanline(io.ly);
}

auto PPU::frame() -> void {
  io.field++;
  scheduler.exit(Scheduler::Event::Frame);
}

auto PPU::refresh() -> void {
  if(system.video.display->value() == "NTSC") {
    screen.refresh(buffer + 8 * 256, 256 * sizeof(uint32), 256, 224);
  }

  if(system.video.display->value() == "PAL") {
    screen.refresh(buffer, 256 * sizeof(uint32), 256, 240);
  }
}

auto PPU::power(bool reset) -> void {
  Thread::create(system.frequency(), {&PPU::main, this});

  io = {};
  latch = {};

  if(!reset) {
    for(auto& data : ciram ) data = 0;
    for(auto& data : cgram ) data = 0;
    for(auto& data : oam   ) data = 0;
  }

  for(auto& data : buffer) data = 0;
}

}
