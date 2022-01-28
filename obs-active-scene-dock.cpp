#include <QMainWindow>
#include <obs-frontend-api.h>
#include <obs-module.h>

#include "ActiveSceneDock.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_AUTHOR("Tim Cooper")

OBS_MODULE_USE_DEFAULT_LOCALE("obs-active-scene-dock", "en-US")

const char *obs_module_name() { return "Active Scene Dock"; }

const char *obs_module_description() { return obs_module_text("Description"); }

struct program_data {
  ActiveSceneDock *previewDock;
  ActiveSceneDock *programDock;
};

static void frontend_event_preview(enum obs_frontend_event event,
                                   void *private_data) {
  const auto handleEvent = event == OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED ||
                           event == OBS_FRONTEND_EVENT_STUDIO_MODE_DISABLED ||
                           event == OBS_FRONTEND_EVENT_STUDIO_MODE_ENABLED;
  if (handleEvent) {
    auto *dock = static_cast<ActiveSceneDock *>(private_data);
    auto *previewScene = obs_frontend_get_current_preview_scene();
    dock->updateCurrentScene(previewScene,
                             obs_frontend_preview_program_mode_active());
    obs_source_release(previewScene);
  }
}

static void frontend_event_program(enum obs_frontend_event event,
                                   void *private_data) {
  if (event == OBS_FRONTEND_EVENT_SCENE_CHANGED) {
    auto *dock = static_cast<ActiveSceneDock *>(private_data);
    auto *programScene = obs_frontend_get_current_scene();
    dock->updateCurrentScene(programScene);
    obs_source_release(programScene);
  }
}

bool obs_module_load() {
  auto *mainWindow = static_cast<QMainWindow *>(obs_frontend_get_main_window());

  auto *previewDock = new ActiveSceneDock(mainWindow);
  previewDock->setWindowTitle(obs_module_text("PreviewDockTitle"));
  previewDock->setObjectName("ActiveSceneDockPreview");
  obs_frontend_add_event_callback(frontend_event_preview, previewDock);
  obs_frontend_add_dock(previewDock);

  auto *programDock = new ActiveSceneDock(mainWindow);
  programDock->setWindowTitle(obs_module_text("ProgramDockTitle"));
  programDock->setObjectName("ActiveSceneDockProgram");
  obs_frontend_add_event_callback(frontend_event_program, programDock);
  obs_frontend_add_dock(programDock);

  return true;
}
