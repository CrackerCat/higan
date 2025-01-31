#include <md/md.hpp>

namespace higan::MegaDrive {

Expansion expansion;
#include "serialization.cpp"

auto Expansion::load(Node::Object parent, Node::Object from) -> void {
  port = Node::append<Node::Port>(parent, from, "Expansion Port", "Cartridge");
  port->allocate = [&] { return Node::Peripheral::create(interface->name()); };
  port->attach = [&](auto node) { connect(node); };
  port->detach = [&](auto node) { disconnect(); };
  port->scan(from);
}

auto Expansion::connect(Node::Peripheral with) -> void {
  node = Node::append<Node::Peripheral>(port, with, interface->name());

  information = {};

  if(auto fp = platform->open(node, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  }

  auto document = BML::unserialize(information.manifest);
  information.name = document["game/label"].text();
  information.regions = document["game/region"].text().split(",").strip();

  mcd.load(node, with);

  power();
}

auto Expansion::disconnect() -> void {
  if(!node) return;
  node = {};
}

auto Expansion::save() -> void {
  if(!node) return;
}

auto Expansion::power() -> void {
}

/* the only existing expansion port device is the Mega CD, which is hard-coded below for now */

auto Expansion::read(uint1 upper, uint1 lower, uint22 address, uint16 data) -> uint16 {
  return mcd.external_read(upper, lower, address, data);
}

auto Expansion::write(uint1 upper, uint1 lower, uint22 address, uint16 data) -> void {
  return mcd.external_write(upper, lower, address, data);
}

auto Expansion::readIO(uint1 upper, uint1 lower, uint24 address, uint16 data) -> uint16 {
  if(!node) return data;
  return mcd.external_readIO(upper, lower, address, data);
}

auto Expansion::writeIO(uint1 upper, uint1 lower, uint24 address, uint16 data) -> void {
  if(!node) return;
  return mcd.external_writeIO(upper, lower, address, data);
}

}
