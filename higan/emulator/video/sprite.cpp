auto Sprite::create(uint width, uint height) -> void {
  this->width = width;
  this->height = height;
  pixels = new uint32[width * height]();
}

auto Sprite::destroy() -> void {
  pixels = {};
}

auto Sprite::setPixels(const nall::image& image) -> void {
  memory::copy(this->pixels.data(), width * height * sizeof(uint32), image.data(), image.size());
}

auto Sprite::setVisible(bool visible) -> void {
  this->visible = visible;
}

auto Sprite::setPosition(int x, int y) -> void {
  this->x = x;
  this->y = y;
}
