struct InputMapper : Panel {
  InputMapper(View*);
  auto show() -> void override;
  auto hide() -> void override;
  auto refresh(higan::Node::Object) -> void;
  auto update() -> void;

  auto eventAssign() -> void;
  auto eventAssignNext() -> void;
  auto eventClear() -> void;
  auto eventChange() -> void;

  auto eventInput(shared_pointer<HID::Device>, uint group, uint input, int16_t oldValue, int16_t newValue) -> void;

  Label nameLabel{this, Size{~0, 0}};
  TableView inputList{this, Size{~0, ~0}};
  HorizontalLayout controlLayout{this, Size{~0, 0}};
    Label message{&controlLayout, Size{~0, 0}};
    Button assignButton{&controlLayout, Size{80_sx, 0}};
    Button clearButton{&controlLayout, Size{80_sx, 0}};

  higan::Node::Object node;
  higan::Node::Input assigning;
  vector<TableViewItem> assigningQueue;
};
