#include <gb/gb.hpp>

namespace higan::GameBoy {

#include "io.cpp"
#include "sequencer.cpp"
#include "square1.cpp"
#include "square2.cpp"
#include "wave.cpp"
#include "noise.cpp"
#include "serialization.cpp"
APU apu;

auto APU::main() -> void {
  square1.run();
  square2.run();
  wave.run();
  noise.run();
  sequencer.run();

  if(!Model::SuperGameBoy()) {
    stream.sample(sequencer.left / 32768.0, sequencer.right / 32768.0);
  } else {
    superGameBoy->apuWrite(sequencer.left / 32768.0, sequencer.right / 32768.0);
  }

  if(cycle == 0) {  //512hz
    if(phase == 0 || phase == 2 || phase == 4 || phase == 6) {  //256hz
      square1.clockLength();
      square2.clockLength();
      wave.clockLength();
      noise.clockLength();
    }
    if(phase == 2 || phase == 6) {  //128hz
      square1.clockSweep();
    }
    if(phase == 7) {  //64hz
      square1.clockEnvelope();
      square2.clockEnvelope();
      noise.clockEnvelope();
    }
    phase++;
  }
  cycle++;

  Thread::step(1);
  Thread::synchronize(cpu);
}

auto APU::power() -> void {
  Thread::create(2 * 1024 * 1024, {&APU::main, this});
  if(!Model::SuperGameBoy()) {
    stream.create(2, frequency());
    stream.addHighPassFilter(20.0, Filter::Order::First);
  }

  square1.power();
  square2.power();
  wave.power();
  noise.power();
  sequencer.power();
  phase = 0;
  cycle = 0;

  PRNG::PCG prng;
  for(auto& n : wave.pattern) n = prng.random();
}

}
