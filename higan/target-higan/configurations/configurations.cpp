#include "../higan.hpp"
#include "create.cpp"
#include "properties.cpp"

namespace Instances { Instance<ConfigurationManager> configurationManager; }
ConfigurationManager& configurationManager = Instances::configurationManager();

ConfigurationManager::ConfigurationManager() {
  directory::create(Path::data);

  actionMenu.setText("System");
  launchAction.setIcon(Icon::Emblem::Program).setText("Launch ...").onActivate([&] {
    eventActivate();
  });
  createAction.setIcon(Icon::Action::Add).setText("Create ...").onActivate([&] {
    eventCreate();
  });
  renameAction.setIcon(Icon::Application::TextEditor).setText("Rename ...").onActivate([&] {
    eventRename();
  });
  removeAction.setIcon(Icon::Action::Remove).setText("Delete ...").onActivate([&] {
    eventRemove();
  });
  propertiesAction.setIcon(Icon::Action::Properties).setText("Properties ...").onActivate([&] {
    eventProperties();
  });
  quitAction.setIcon(Icon::Action::Quit).setText("Quit").onActivate([&] {
    doClose();
  });

  settingsMenu.setText("Settings");

  pathsMenu.setText("Paths");
  dataPathChange.setIcon(Icon::Action::Settings).setText("Change Data Path ...").onActivate([&] {
    if(auto location = BrowserDialog()
    .setTitle("Select Data Path")
    .setPath(Path::data)
    .setPlacement(Placement::Overlap, *this).selectFolder()
    ) {
      Path::data = location;
      file::write({Path::settings, "paths.bml"}, string{
        "data: ", Path::data, "\n",
        "templates: ", Path::templates, "\n"
      });
      refresh();
    }
  });
  dataPathOpen.setIcon(Icon::Action::Open).setText("Open Data Path ...").onActivate([&] {
    invoke(Path::data);
  });
  templatesPathOpen.setIcon(Icon::Action::Open).setText("Open Templates Path ...").onActivate([&] {
    invoke(Path::templates);
  });
  settingsPathOpen.setIcon(Icon::Action::Open).setText("Open Settings Path ...").onActivate([&] {
    invoke(Path::settings);
  });

  helpMenu.setText("Help");
  documentation.setIcon(Icon::Application::Browser).setText("Documentation ...").onActivate([&] {
    invoke("https://doc.byuu.org/higan/");
  });
  about.setIcon(Icon::Prompt::Question).setText("About ...").onActivate([&] {
    AboutDialog()
    .setLogo(Resource::Logo)
    .setVersion(higan::Version)
    .setAuthor(higan::Author)
    .setLicense(higan::License)
    .setWebsite(higan::Website)
    .setPlacement(Placement::Center, *this).show();
  });

  launchPopup.setIcon(Icon::Emblem::Program).setText("Launch ...").onActivate([&] {
    eventActivate();
  });
  createPopup.setIcon(Icon::Action::Add).setText("Create ...").onActivate([&] {
    eventCreate();
  });
  renamePopup.setIcon(Icon::Application::TextEditor).setText("Rename ...").onActivate([&] {
    eventRename();
  });
  removePopup.setIcon(Icon::Action::Remove).setText("Delete ...").onActivate([&] {
    eventRemove();
  });

  layout.setPadding(5);
  configurationList.setBackgroundColor(Theme::BackgroundColor);
  configurationList.setForegroundColor(Theme::ForegroundColor);
  configurationList.onActivate([&] {
    eventActivate();
  });
  configurationList.onChange([&] {
    eventChange();
  });
  configurationList.onContext([&] {
    eventContext();
  });
  controlLayout.setAlignment(0.5);
  createButton.setText("Create").onActivate([&] {
    eventCreate();
  });
  renameButton.setText("Rename").onActivate([&] {
    eventRename();
  });
  removeButton.setText("Delete").onActivate([&] {
    eventRemove();
  });
  propertiesButton.setText("Properties").onActivate([&] {
    eventProperties();
  });
  launchButton.setText("Launch").onActivate([&] {
    eventActivate();
  });

  onClose([&] { Application::exit(); });

  setTitle({"higan v", higan::Version});
  setSize({640, 360});
  setPlacement(Placement::Center);

  refresh();
}

auto ConfigurationManager::show() -> void {
  setVisible();
  setFocused();
}

auto ConfigurationManager::refresh() -> void {
  configurationList.reset();
  scan(configurationList, Path::data);
  configurationList.expand();
  configurationList.doChange();
}

template<typename T> auto ConfigurationManager::scan(T parent, string location) -> void {
  for(auto& name : directory::folders(location)) {
    TreeViewItem item{&parent};
    item.setProperty("location", {location, name});
    if(auto document = BML::unserialize(file::read({location, name, "identity.bml"}))) {
      item.setProperty("system", document["system"].text());
      item.setIcon(Icon::Place::Server).setText(string{name}.trimRight("/", 1L));
    } else {
      item.setIcon(Icon::Emblem::Folder).setText(string{name}.trimRight("/", 1L));
      scan(item, {location, name});
    }
  }
}

auto ConfigurationManager::eventActivate() -> void {
  if(auto item = configurationList.selected()) {
    if(auto system = item.property("system")) {
      if(auto index = interfaces.find([&](auto emulator) { return emulator->name() == system; })) {
        setVisible(false);
        emulator.create(interfaces[*index], item.property("location"));
      }
    }
  }
}

auto ConfigurationManager::eventChange() -> void {
  auto item = configurationList.selected();
  auto system = item.property("system");
  createAction.setEnabled(!(bool)system);
  createButton.setEnabled(!(bool)system);
  renameAction.setEnabled((bool)item);
  renameButton.setEnabled((bool)item);
  removeAction.setEnabled((bool)item);
  removeButton.setEnabled((bool)item);
  propertiesAction.setEnabled((bool)system);
  propertiesButton.setEnabled((bool)system);
  launchAction.setEnabled((bool)system);
  launchButton.setEnabled((bool)system);
}

auto ConfigurationManager::eventContext() -> void {
  auto item = configurationList.selected();
  auto system = item.property("system");
  launchPopup.setVisible((bool)system);
  launchPopupSeparator.setVisible((bool)system);
  createPopup.setVisible(!(bool)system);
  renamePopup.setVisible((bool)item);
  removePopup.setVisible((bool)item);
  contextMenu.setVisible();
}

auto ConfigurationManager::eventCreate() -> void {
  auto [system, name] = createDialog.run();
  if(!name) return;  //user cancelled the operation
  name.append("/");
  auto location = Path::data;
  if(auto item = configurationList.selected()) location = item.property("location");
  if(directory::exists({location, name})) {
    if(MessageDialog()
      .setTitle("Warning")
      .setText("A directory by this name already exists.\n"
               "Do you wish to delete the existing directory and create a new one?")
      .setPlacement(Placement::Center, *this).question() == "No"
    ) return;
    if(!directory::remove({location, name})) return (void)MessageDialog()
      .setTitle("Error")
      .setText({"Failed to remove previous directory. The location may be read-only.\n",
                "Location: ", location,
                "Name: ", name})
      .setPlacement(Placement::Center, *this).error();
  }
  if(!directory::create({location, name})) return (void)MessageDialog()
    .setTitle("Error")
    .setText({"Failed to create directory. Either the location is read-only, or the name contains invalid characters.\n",
              "Location: ", location, "\n",
              "Name: ", name})
    .setPlacement(Placement::Center, *this).error();
  if(system) {
    if(auto index = interfaces.find([&](auto emulator) { return emulator->name() == system; })) {
      file::write({location, name, "identity.bml"}, string{
        "system: ", system, "\n"
      });
    }
  }
  refresh();
}

auto ConfigurationManager::eventRename() -> void {
  auto item = configurationList.selected();
  if(!item) return;
  auto location = item.property("location");
  auto originalName = Location::base(location);
  auto name = NameDialog()
  .setTitle({"Rename ", item.text()})
  .setText("Enter a new name:")
  .setPlacement(Placement::Overlap, *this)
  .rename(item.text());
  if(!name) return;  //user cancelled the operation
  name.append("/");
  if(name == originalName) return;
  location = Location::dir(location);
  if(directory::exists({location, name})) {
    if(MessageDialog()
      .setTitle("Warning")
      .setText("A directory by this name already exists.\n"
               "Do you wish to delete the existing directory and replace it with this one?")
      .setPlacement(Placement::Center, *this).question() == "No"
    ) return;
    if(!directory::remove({location, name})) return (void)MessageDialog()
      .setTitle("Error")
      .setText({"Failed to remove previous directory. The location may be read-only.\n",
                "Location: ", location, "\n",
                "Name: ", name})
      .setPlacement(Placement::Center, *this).error();
  }
  if(!directory::rename({location, originalName}, {location, name})) return (void)MessageDialog()
    .setTitle("Error")
    .setText({"Failed to rename directory. Either the location is read-only, or the name contains invalid characters.\n",
              "Location: ", location, "\n",
              "Name: ", name})
    .setPlacement(Placement::Center, *this).error();
  refresh();
}

auto ConfigurationManager::eventRemove() -> void {
  auto item = configurationList.selected();
  if(!item) return;
  auto location = item.property("location");
  if(MessageDialog()
    .setTitle("Warning")
    .setText({"This will permanently destroy this directory, and all of its contents recursively.\n",
              "All related settings and content will be lost. Are you really sure you want to do this?\n",
              "Location: ", location})
    .setPlacement(Placement::Center, *this).question() == "No"
  ) return;
  if(!directory::remove(location)) return (void)MessageDialog()
    .setTitle("Error")
    .setText({"Failed to remove directory. The location may be read-only.\n",
              "Location: ", location})
    .setPlacement(Placement::Center, *this).error();
  refresh();
}

auto ConfigurationManager::eventProperties() -> void {
  auto item = configurationList.selected();
  if(!item || !item.property("system")) return;
  auto location = item.property("location");
  auto properties = propertiesDialog.run(item.text(), file::read({location, "properties.bml"}));
  if(!properties) return;  //user cancelled the operation
  if(!file::write({location, "properties.bml"}, properties)) return (void)MessageDialog()
    .setTitle("Error")
    .setText({"Failed to write properties.bml file. The location may be read-only.\n"
              "Location: ", location})
    .setPlacement(Placement::Center, *this).error();
}
