#include <pce/pce.hpp>

namespace higan::PCEngine {

Cartridge cartridge;

auto Cartridge::load(Node::Object parent, Node::Object from) -> void {
  port = Node::append<Node::Port>(parent, from, "Cartridge Slot", "Cartridge");
  port->allocate = [&] { return Node::Peripheral::create(interface->name()); };
  port->attach = [&](auto node) { connect(node); };
  port->detach = [&](auto node) { disconnect(); };
  port->scan(from);
}

auto Cartridge::connect(Node::Peripheral with) -> void {
  node = Node::append<Node::Peripheral>(port, with, interface->name());

  information = {};

  if(auto fp = platform->open(node, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  }

  auto document = BML::unserialize(information.manifest);
  information.name = document["game/label"].text();

  if(auto memory = document["game/board/memory(type=ROM,content=Program)"]) {
    rom.size = memory["size"].natural();
    rom.data = new uint8[rom.size]();
    if(auto fp = platform->open(node, "program.rom", File::Read, File::Required)) {
      fp->read(rom.data, rom.size);
    }
  }

  if(auto fp = platform->open(node, "save.ram", File::Read)) {
    cpu.bram.load(fp);
  }

  power();
}

auto Cartridge::disconnect() -> void {
  if(!node) return;
  delete[] rom.data;
  rom = {};
  node = {};
}

//PC Engine HuCards lack save RAM on them due to the card size and cost savings.
//The PC Engine CD adds 2KB of backup RAM that most HuCard games can use for saves.
//However, all games must share this small amount of RAM.
//Since this is an emulator, we can make this process nicer by storing BRAM per-game.

auto Cartridge::save() -> void {
  if(!node) return;
  auto document = BML::unserialize(information.manifest);

  if(auto fp = platform->open(node, "save.ram", File::Write)) {
    cpu.bram.save(fp);
  }
}

auto Cartridge::power() -> void {
}

auto Cartridge::read(uint20 addr) -> uint8 {
  if(!rom.size) return 0x00;
  return rom.data[mirror(addr, rom.size)];
}

auto Cartridge::write(uint20 addr, uint8 data) -> void {
}

auto Cartridge::mirror(uint addr, uint size) -> uint {
  //384KB games have unusual mirroring (only second ROM repeats)
  if(size == 0x60000) {
    if(addr <= 0x3ffff) return addr;
    return 0x40000 + (addr & 0x1ffff);
  }

  uint base = 0;
  uint mask = 1 << 20;
  while(addr >= size) {
    while(!(addr & mask)) mask >>= 1;
    addr -= mask;
    if(size > mask) {
      size -= mask;
      base += mask;
    }
    mask >>= 1;
  }
  return base + addr;
}

}
