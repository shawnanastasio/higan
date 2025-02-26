auto CPU::keypadRead() -> uint4 {
  uint4 data = 0;

  if(r.ypadEnable) {
    if(system.video.orientation->value() == "Horizontal") {
      data.bit(0) = system.controls.y1->value;
      data.bit(1) = system.controls.y2->value;
      data.bit(2) = system.controls.y3->value;
      data.bit(3) = system.controls.y4->value;
    } else {
      data.bit(0) = system.controls.x4->value;
      data.bit(1) = system.controls.x1->value;
      data.bit(2) = system.controls.x2->value;
      data.bit(3) = system.controls.x3->value;
    }
  }

  if(r.xpadEnable) {
    if(system.video.orientation->value() == "Horizontal") {
      data.bit(0) = system.controls.x1->value;
      data.bit(1) = system.controls.x2->value;
      data.bit(2) = system.controls.x3->value;
      data.bit(3) = system.controls.x4->value;
    } else {
      data.bit(0) = system.controls.y4->value;
      data.bit(1) = system.controls.y1->value;
      data.bit(2) = system.controls.y2->value;
      data.bit(3) = system.controls.y3->value;
    }
  }

  if(r.buttonEnable) {
    data.bit(1) = system.controls.start->value;
    data.bit(2) = system.controls.a->value;
    data.bit(3) = system.controls.b->value;
  }

  return data;
}

auto CPU::portRead(uint16 addr) -> uint8 {
  //DMA_SRC
  if(addr == 0x0040) return r.dmaSource.byte(0);
  if(addr == 0x0041) return r.dmaSource.byte(1);
  if(addr == 0x0042) return r.dmaSource.byte(2);

  //DMA_DST
  if(addr == 0x0044) return r.dmaTarget.byte(0);
  if(addr == 0x0045) return r.dmaTarget.byte(1);

  //DMA_LEN
  if(addr == 0x0046) return r.dmaLength.byte(0);
  if(addr == 0x0047) return r.dmaLength.byte(1);

  //DMA_CTRL
  if(addr == 0x0048) return r.dmaMode << 0 | r.dmaEnable << 7;

  //WSC_SYSTEM
  if(addr == 0x0062) return (
    Model::SwanCrystal() << 7
  );

  //HW_FLAGS
  if(addr == 0x00a0) {
    bool color = Model::WonderSwanColor() || Model::SwanCrystal();
    return (
      1     << 0  //0 = BIOS mapped; 1 = cartridge mapped
    | color << 1  //0 = WonderSwan or Pocket Challenge V2; 1 = WonderSwan Color or SwanCrystal
    | 1     << 2  //0 = 8-bit bus width; 1 = 16-bit bus width
    | 1     << 7  //1 = built-in self-test passed
    );
  }

  //INT_BASE
  if(addr == 0x00b0) return (
    r.interruptBase | (Model::WonderSwan() || Model::PocketChallengeV2() ? 3 : 0)
  );

  //SER_DATA
  if(addr == 0x00b1) return r.serialData;

  //INT_ENABLE
  if(addr == 0x00b2) return r.interruptEnable;

  //SER_STATUS
  if(addr == 0x00b3) return (
    1                << 2  //hack: always report send buffer as empty
  | r.serialBaudRate << 6
  | r.serialEnable   << 7
  );

  //INT_STATUS
  if(addr == 0x00b4) return r.interruptStatus;

  //KEYPAD
  if(addr == 0x00b5) return (
    keypadRead()   << 0
  | r.ypadEnable   << 4
  | r.xpadEnable   << 5
  | r.buttonEnable << 6
  );

  return 0x00;
}

auto CPU::portWrite(uint16 addr, uint8 data) -> void {
  //DMA_SRC
  if(addr == 0x0040) r.dmaSource.byte(0) = data & ~1;
  if(addr == 0x0041) r.dmaSource.byte(1) = data;
  if(addr == 0x0042) r.dmaSource.byte(2) = data;

  //DMA_DST
  if(addr == 0x0044) r.dmaTarget.byte(0) = data & ~1;
  if(addr == 0x0045) r.dmaTarget.byte(1) = data;

  //DMA_LEN
  if(addr == 0x0046) r.dmaLength.byte(0) = data & ~1;
  if(addr == 0x0047) r.dmaLength.byte(1) = data;

  //DMA_CTRL
  if(addr == 0x0048) {
    r.dmaMode   = data.bit(6);
    r.dmaEnable = data.bit(7);
    if(r.dmaEnable) dmaTransfer();
  }

  //WSC_SYSTEM
  if(addr == 0x0062) {
    //todo: d0 = 1 powers off system
  }

  //HW_FLAGS
  if(addr == 0x00a0) {
    //todo: d2 (bus width) bit is writable; but ... it will do very bad things
  }

  //INT_BASE
  if(addr == 0x00b0) {
    r.interruptBase = Model::WonderSwan() || Model::PocketChallengeV2() ? data & ~7 : data & ~1;
  }

  //SER_DATA
  if(addr == 0x00b1) r.serialData = data;

  //INT_ENABLE
  if(addr == 0x00b2) {
    r.interruptEnable = data;
    r.interruptStatus &= ~r.interruptEnable;
  }

  //SER_STATUS
  if(addr == 0x00b3) {
    r.serialBaudRate = data.bit(6);
    r.serialEnable   = data.bit(7);
  }

  //KEYPAD
  if(addr == 0x00b5) {
    r.ypadEnable   = data.bit(4);
    r.xpadEnable   = data.bit(5);
    r.buttonEnable = data.bit(6);
  }

  //INT_ACK
  if(addr == 0x00b6) {
    //acknowledge only edge-sensitive interrupts
    r.interruptStatus &= ~(data & 0b11110010);
  }
}
