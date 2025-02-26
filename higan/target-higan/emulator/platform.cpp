auto Emulator::attach(higan::Node::Object node) -> void {
}

auto Emulator::detach(higan::Node::Object node) -> void {
  if(auto location = node->property("location")) {
    file::write({location, "settings.bml"}, node->save());
  }
}

auto Emulator::open(higan::Node::Object node, string name, vfs::file::mode mode, bool required) -> vfs::shared::file {
  auto location = node->property("location");

  if(name == "manifest.bml") {
    if(!file::exists({location, name}) && directory::exists(location)) {
      if(auto manifest = execute("icarus", "--system", node->name, "--manifest", location).output) {
        return vfs::memory::file::open(manifest.data<uint8_t>(), manifest.size());
      }
    }
  }

  if(auto result = vfs::fs::file::open({location, name}, mode)) return result;

  if(required) {
    //attempt to pull required system firmware (boot ROMs, etc) from system template
    if(location == emulator.system.data) {
      if(file::exists({emulator.system.templates, name})) {
        file::copy({emulator.system.templates, name}, {emulator.system.data, name});
        if(auto result = vfs::fs::file::open({location, name}, mode)) return result;
      }
    }

    if(MessageDialog()
    .setTitle("Warning")
    .setText({"Missing required file:\n",
              location, name, "\n\n",
              "Would you like to browse for this file now?"})
    .setAlignment(Alignment::Center)
    .question() == "No") return {};

    if(auto source = BrowserDialog()
    .setTitle({"Load ", name})
    .setPath(location)
    .setAlignment(Alignment::Center)
    .openFile()
    ) {
      if(auto input = vfs::memory::file::open(source, true)) {
        if(auto output = file::open({location, name}, file::mode::write)) {
          output.write({input->data(), (uint)input->size()});
        }
      }
      if(auto result = vfs::fs::file::open({location, name}, mode)) return result;
    }
  }

  return {};
}

auto Emulator::video(higan::Node::Video node, const uint32_t* data, uint pitch, uint width, uint height) -> void {
  uint videoWidth = node->width * node->scaleX;
  uint videoHeight = node->height * node->scaleY;

  if(settings.video.aspectCorrection) {
    videoWidth = videoWidth * node->aspectX / node->aspectY;
  }

  auto [viewportWidth, viewportHeight] = videoInstance.size();

  uint multiplierX = viewportWidth / videoWidth;
  uint multiplierY = viewportHeight / videoHeight;
  uint multiplier = min(multiplierX, multiplierY);

  uint outputWidth = videoWidth * multiplier;
  uint outputHeight = videoHeight * multiplier;

  if(multiplier == 0 || settings.video.output == "Scale") {
    float multiplierX = (float)viewportWidth / (float)videoWidth;
    float multiplierY = (float)viewportHeight / (float)videoHeight;
    float multiplier = min(multiplierX, multiplierY);

    outputWidth = videoWidth * multiplier;
    outputHeight = videoHeight * multiplier;
  }

  if(settings.video.output == "Stretch") {
    outputWidth = viewportWidth;
    outputHeight = viewportHeight;
  }

  pitch >>= 2;
  if(auto [output, length] = videoInstance.acquire(width, height); output) {
    length >>= 2;
    for(auto y : range(height)) {
      memory::copy<uint32>(output + y * length, data + y * pitch, width);
    }
    videoInstance.release();
    videoInstance.output(outputWidth, outputHeight);
  }

  static uint frameCounter = 0;
  static uint64_t previous, current;
  frameCounter++;

  current = chrono::timestamp();
  if(current != previous) {
    previous = current;
    setCaption({frameCounter, " fps"});
    frameCounter = 0;
  }
}

auto Emulator::audio(higan::Node::Audio node, const double* samples, uint channels) -> void {
  if(channels == 1) {
    double stereo[] = {samples[0], samples[0]};
    audioInstance.output(stereo);
  } else {
    audioInstance.output(samples);
  }
}

auto Emulator::input(higan::Node::Input input) -> void {
  inputManager.poll();

  bool allow = programWindow.viewport.focused();
  if(settings.input.unfocused == "Allow") allow = true;
  if(videoInstance.exclusive()) allow = true;

  if(auto button = input->cast<higan::Node::Button>()) {
    button->value = 0;
    if(auto instance = button->property<shared_pointer<InputButton>>("instance")) {
      button->value = allow ? instance->value() : 0;
    }
  }
}
