#include <fc/fc.hpp>

namespace higan::Famicom {

CPU cpu;
#include "memory.cpp"
#include "timing.cpp"
#include "serialization.cpp"

auto CPU::main() -> void {
  if(io.interruptPending) return interrupt();
  instruction();
}

auto CPU::step(uint clocks) -> void {
  Thread::step(clocks);
  Thread::synchronize();
}

auto CPU::power(bool reset) -> void {
  MOS6502::BCD = 0;
  MOS6502::power();
  Thread::create(system.frequency(), {&CPU::main, this});

  if(!reset) for(auto& data : ram) data = 0xff;
  ram[0x008] = 0xf7;  //todo: what is this about?
  ram[0x009] = 0xef;
  ram[0x00a] = 0xdf;
  ram[0x00f] = 0xbf;

  r.pc.byte(0) = bus.read(0xfffc);
  r.pc.byte(1) = bus.read(0xfffd);

  io = {};
}

}
