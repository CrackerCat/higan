SuperFX superfx;
#include "bus.cpp"
#include "core.cpp"
#include "memory.cpp"
#include "io.cpp"
#include "timing.cpp"
#include "serialization.cpp"

auto SuperFX::main() -> void {
  if(regs.sfr.g == 0) return step(6);

  instruction(peekpipe());

  if(regs.r[14].modified) {
    regs.r[14].modified = false;
    updateROMBuffer();
  }

  if(regs.r[15].modified) {
    regs.r[15].modified = false;
  } else {
    regs.r[15]++;
  }
}

auto SuperFX::unload() -> void {
  rom.reset();
  ram.reset();
  bram.reset();

  cpu.coprocessors.removeByValue(this);
  Thread::destroy();
}

auto SuperFX::power() -> void {
  GSU::power();

  Thread::create(Frequency, {&SuperFX::main, this});
  cpu.coprocessors.append(this);

  romMask = rom.size() - 1;
  ramMask = ram.size() - 1;
  bramMask = bram.size() - 1;

  for(uint n : range(512)) cache.buffer[n] = 0x00;
  for(uint n : range(32)) cache.valid[n] = false;
  for(uint n : range(2)) {
    pixelcache[n].offset = ~0;
    pixelcache[n].bitpend = 0x00;
  }

  regs.romcl = 0;
  regs.romdr = 0;

  regs.ramcl = 0;
  regs.ramar = 0;
  regs.ramdr = 0;
}
