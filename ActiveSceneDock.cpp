#include <QTimerEvent>
#include <QVBoxLayout>

#include "ActiveSceneDock.hpp"

ActiveSceneDock::ActiveSceneDock(QWidget *parent)
    : QDockWidget(parent), currentScene(nullptr), timerId(-1) {
  setFeatures(DockWidgetMovable | DockWidgetFloatable);

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
    obs_source_release(currentScene);
  }
}

void ActiveSceneDock::updateCurrentScene(obs_source_t *newScene,
                                         bool isActive) {
  if (currentScene == newScene) {
    return;
  }
  if (currentScene) {
    obs_source_release(currentScene);
    currentScene = nullptr;
  }

  if (isActive && newScene) {
    auto *name = obs_source_get_name(newScene);
    sceneLabel->setText(QString::fromUtf8(name));
    obs_source_addref(newScene);
    currentScene = newScene;

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
