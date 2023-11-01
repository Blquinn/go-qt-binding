#include "lib.hpp"

#include <QApplication>
#include <QLabel>
#include <QDebug>
#include <QMetaObject>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QtWidgets>
#include <QTimer>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include "util.hpp"

/* Application */

AppStruct *Application_new(int argc, char **argv) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  auto queue = std::make_unique<SafeQueue<QApplication *>>();

  // The QApplication must be started on its own (non QThread) thread,
  // See
  // https://forum.qt.io/topic/124878/running-qapplication-exec-from-another-thread-qcoreapplication-qguiapplication
  auto appThread = new std::thread([&]() {
    auto app = new QApplication(argc, argv);
    queue->enqueue(app);
    auto ret = app->exec();
    appExited(ret);
  });

  auto qtApp = queue->dequeue();

  // Ensure that app has started executing before returning.
  QTimer::singleShot(0, qtApp, [&]() {
    queue->enqueue(nullptr);
  });

  queue->dequeue();

  return new AppStruct{
      .app_thread = appThread,
      .qt_application = qtApp,
  };
}

/* End Application */

/* Window */

void *Window_new(void *app_ptr) {
  auto app = static_cast<QApplication *>(app_ptr);

  // qInfo() << "qt here2";

  QWidget *win;
  runOnAppThread(
      app,
      [=]() -> QWidget * {
        auto w = new QWidget();
        w->resize(320, 240);
        w->setMinimumSize(320, 240);
        w->show();
        return w;
      },
      &win);

  return win;
}

void Window_set_title(void *win_ptr, char *title) {
  auto win = static_cast<QWidget *>(win_ptr);
  runOnAppThread(win, [=]() { win->setWindowTitle(title); });
}

void Window_set_minimum_size(void *win_ptr, int width, int height) {
  auto win = static_cast<QWidget *>(win_ptr);
  runOnAppThread(win, [=]() { win->setMinimumSize(width, height); });
}

void Window_resize(void *win_ptr, int width, int height) {
  auto win = static_cast<QWidget *>(win_ptr);
  runOnAppThread(win, [=]() { win->resize(width, height); });
}

/* End Window */

/* Window Layout */

void *WindowLayout_new(void *win_ptr) {
  auto win = static_cast<QWidget *>(win_ptr);

  QVBoxLayout *layout;
  runOnAppThread(
      win,
      [=]() -> QVBoxLayout * {
        auto layout = new QVBoxLayout(win);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        return layout;
      },
      &layout);

  return layout;
}

/* End Window Layout */

/* WebEngineView */

void *WebEngineView_new(void *win_ptr, void *win_layout_ptr) {
  auto win = static_cast<QWidget *>(win_ptr);
  auto layout = static_cast<QVBoxLayout *>(win_layout_ptr);

  QWebEngineView *view;
  runOnAppThread(
      win,
      [=]() -> QWebEngineView * {
        auto view = new QWebEngineView(win);
        layout->addWidget(view);
        view->load(QUrl("https://example.org"));
        return view;
      },
      &view);

  return view;
}

void WebEngineView_load_url(void *web_engine_ptr, char *url) {
  auto eng = static_cast<QWebEngineView *>(web_engine_ptr);

  runOnAppThread(eng, [=]() { eng->load(QUrl(url)); });
}

/* End WebEngineView */
