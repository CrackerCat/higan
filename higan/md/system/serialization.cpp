auto System::serializeInit() -> void {
  serializer s;

  uint signature = 0;
  char version[16] = {0};
  char description[512] = {0};

  s.integer(signature);
  s.array(version);
  s.array(description);

  serializeAll(s);
  information.serializeSize = s.size();
}

auto System::serialize() -> serializer {
  serializer s{information.serializeSize};

  uint signature = 0x31545342;
  char version[16] = {0};
  char description[512] = {0};
  memory::copy(&version, (const char*)SerializerVersion, SerializerVersion.size());

  s.integer(signature);
  s.array(version);
  s.array(description);

  serializeAll(s);
  return s;
}

auto System::unserialize(serializer& s) -> bool {
  uint signature = 0;
  char version[16] = {0};
  char description[512] = {0};

  s.integer(signature);
  s.array(version);
  s.array(description);

  if(signature != 0x31545342) return false;
  if(string{version} != SerializerVersion) return false;

  power(/* reset = */ false);
  serializeAll(s);
  return true;
}

auto System::serializeAll(serializer& s) -> void {
  system.serialize(s);
  cartridge.serialize(s);
  expansion.serialize(s);
  cpu.serialize(s);
  apu.serialize(s);
  vdp.serialize(s);
  psg.serialize(s);
  ym2612.serialize(s);
  if(MegaCD()) mcd.serialize(s);
  controllerPort1.serialize(s);
  controllerPort2.serialize(s);
  extensionPort.serialize(s);
}

auto System::serialize(serializer& s) -> void {
}
