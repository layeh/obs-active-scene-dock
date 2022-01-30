#include <QTimerEvent>
#include <QVBoxLayout>

#include "ActiveSceneDock.hpp"

ActiveSceneDock::ActiveSceneDock(QWidget *parent)
    : QDockWidget(parent), currentScene(nullptr), timerId(-1) {
  setFeatures(DockWidgetMovable | DockWidgetFloatable);
  hide();

  sceneLabel = new QLabel(this);
  sceneLabel->setAlignment(Qt::AlignCenter);

  timerLabel = new QLabel(this);
  timerLabel->setAlignment(Qt::AlignCenter);

  auto *layout = new QVBoxLayout(this);
  layout->addStretch(1);
  layout->addWidget(sceneLabel);
  layout->addWidget(timerLabel);
  layout->addStretch(1);

  auto *widget = new QWidget(this);
  widget->setLayout(layout);
  setWidget(widget);
}

ActiveSceneDock::~ActiveSceneDock() {
  if (currentScene) {
    auto *signalHandler = obs_source_get_signal_handler(currentScene);
    if (signalHandler) {
      signal_handler_disconnect(signalHandler, "rename",
                                &ActiveSceneDock::currentSceneRenamed, this);
    }
    obs_source_release(currentScene);
  }
}

void ActiveSceneDock::updateCurrentScene(obs_source_t *newScene,
                                         bool isActive) {
  if (currentScene == newScene) {
    return;
  }
  if (currentScene) {
    auto *signalHandler = obs_source_get_signal_handler(currentScene);
    if (signalHandler) {
      signal_handler_disconnect(signalHandler, "rename",
                                &ActiveSceneDock::currentSceneRenamed, this);
    }
    obs_source_release(currentScene);
    currentScene = nullptr;
  }

  if (isActive && newScene) {
    auto *name = obs_source_get_name(newScene);
    sceneLabel->setText(QString::fromUtf8(name));
    obs_source_addref(newScene);
    currentScene = newScene;

    auto *signalHandler = obs_source_get_signal_handler(newScene);
    if (signalHandler) {
      signal_handler_connect(signalHandler, "rename",
                             &ActiveSceneDock::currentSceneRenamed, this);
    }

    currentSceneTimer.start();
    if (timerId < 0) {
      timerId = startTimer(75);
    }
  } else {
    sceneLabel->setText(obs_module_text("Unknown"));
    if (timerId >= 0) {
      killTimer(timerId);
    }
    timerId = -1;
  }

  updateTimerText();
}

void ActiveSceneDock::timerEvent(QTimerEvent *event) {
  if (event->timerId() == timerId) {
    updateTimerText();
  } else {
    QDockWidget::timerEvent(event);
  }
}

void ActiveSceneDock::currentSceneRenamed(void *data, calldata_t *cd) {
  auto *dock = static_cast<ActiveSceneDock *>(data);
  const auto *newName = calldata_string(cd, "new_name");
  if (newName) {
    dock->sceneLabel->setText(QString::fromUtf8(newName));
  }
}

void ActiveSceneDock::updateTimerText() {
  qint64 elapsed = 0;
  if (currentScene) {
    elapsed = currentSceneTimer.elapsed();
  }
  auto seconds = elapsed / 1000 % 60;
  auto minutes = elapsed / 1000 / 60 % 60;
  auto hours = elapsed / 1000 / 60 / 60;

  QLatin1Char fill('0');

  QString text = QString("%3:%2:%1")
                     .arg(seconds, 2, 10, fill)
                     .arg(minutes, 2, 10, fill)
                     .arg(hours, 2, 10, fill);

  timerLabel->setText(text);
}
