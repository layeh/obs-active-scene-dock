#include <QDockWidget>
#include <QElapsedTimer>
#include <QLabel>
#include <obs-module.h>
#include <obs-source.h>

class ActiveSceneDock : public QDockWidget {

  QLabel *sceneLabel;
  QLabel *timerLabel;
  obs_source_t *currentScene;
  QElapsedTimer currentSceneTimer;
  int timerId;

public:
  explicit ActiveSceneDock(QWidget *parent = nullptr);
  ~ActiveSceneDock();

  void updateCurrentScene(obs_source_t *newScene, bool isActive = true);

protected:
  void timerEvent(QTimerEvent *event) override;
  void updateTimerText();
};
