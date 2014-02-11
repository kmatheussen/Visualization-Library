/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#ifndef Qt4ThreadedWindow_INCLUDE_ONCE
#define Qt4ThreadedWindow_INCLUDE_ONCE

#include <vlQt4/link_config.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlCore/Log.hpp>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>
#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFormat>

namespace vlQt4
{

  class VLQT4_EXPORT Qt4ThreadedWidget : public QGLWidget {
    //    Q_OBJECT

    struct Event{
      Event()
        :button(vl::LeftButton),x(0),y(0),wheel(0),unicode_ch(0),key(vl::Key_None)
      {}
        
      enum{
        MOUSEPRESS,
        MOUSERELEASE,
        MOUSEMOVE,
        WHEEL,
        KEYPRESS,
        KEYRELEASE
      } type;
      vl::EMouseButton button;
      int x;
      int y;
      float wheel;
      unsigned short unicode_ch;
      vl::EKey key;
    };


  public:

    struct MyThread : public QThread, vl::OpenGLContext {
      Qt4ThreadedWidget *_widget;

      QMutex mutex;
      QQueue<Event> queue;

      volatile int widget_width;
      volatile int widget_height;

      QTime time;

      MyThread(Qt4ThreadedWidget* widget)
        : _widget(widget)
        , widget_width(10)
        , widget_height(10)
      { }

      void handle_events(){
        QMutexLocker locker(&mutex);

        while (!queue.isEmpty()){
          Event e = queue.dequeue();
          switch(e.type){
          case Event::MOUSEPRESS:
            dispatchMouseDownEvent(e.button, e.x, e.y);
            break;
          case Event::MOUSERELEASE:
            dispatchMouseUpEvent(e.button, e.x, e.y);
            break;
          case Event::MOUSEMOVE:
            dispatchMouseMoveEvent(e.x,e.y);
          case Event::WHEEL:
            dispatchMouseWheelEvent(e.wheel);
            break;
          case Event::KEYPRESS:
            dispatchKeyPressEvent(e.unicode_ch,e.key);
            break;
          case Event::KEYRELEASE:
            dispatchKeyReleaseEvent(e.unicode_ch,e.key);
            break;
          default:
            abort();
          }
        }
      }

      void push_event(Event e){
        QMutexLocker locker(&mutex);
        queue.enqueue(e);
      }

      void update(){
        //printf("update callled\n");
      }

      void swapBuffers()
      {
        _widget->swapBuffers();
      }

      void makeCurrent()
      {
        _widget->makeCurrent();
      }

      void run() {
        int width = 0;
        int height = 0;

        Qt4ThreadedWidget *widget = _widget;

        makeCurrent();

        widget->init_vl();

        //dispatchResizeEvent(2000,1024);

        for(;;){

          if(widget_height!=height || widget_width!=width){
            width = widget_width;
            height = widget_height;
            printf("resizing to %d/%d\n",width,height);
            dispatchResizeEvent(width,height);
          }

          handle_events();

          //widget->swapBuffers();    
          
          dispatchRunEvent();

          //printf("width/height: %d/%d\n",width(),height());
          
          //if(time.elapsed()>18)
          //  printf("hepp %d\n",(int)time.elapsed());
          time.restart();
          //QApplication::processEvents(QEventLoop::AllEvents, 1);
        }
      }

      bool initQt4Widget(const vl::String& title, const QGLContext* =0, int x=0, int y=0, int width=640, int height=480)
      {
        initGLContext();
        //dispatchInitEvent();
        dispatchResizeEvent(width, height);

        framebuffer()->setWidth(width);
        framebuffer()->setHeight(height);
        

        setWindowTitle(title);
        printf("move to %d %d %d %d\n",x,y,width,height);
        //move(x,y);
        //resize(width,height);
        
        //if (info.fullscreen())
        //  setFullscreen(true);
        
        return true;
      }
    };

    MyThread *mythread;

    void 	paintEvent ( QPaintEvent *  ){}
    void 	resizeEvent ( QResizeEvent *  ) {
      mythread->widget_width = width();
      mythread->widget_height = height();
    }
    //virtual void 	updateGL () {}
    //virtual void 	paintGL () {}
    //virtual bool 	event ( QEvent * e ){ return true;    }

    void mouseMoveEvent(QMouseEvent* ev)
    {
      Event e;
      e.type=Event::MOUSEMOVE;
      e.x=ev->x();
      e.y=ev->y();
      mythread->push_event(e);
    }

    void mousePressEvent(QMouseEvent* ev)
    {
      vl::EMouseButton bt = vl::NoButton;
      switch(ev->button())
      {
      case Qt::LeftButton:  bt = vl::LeftButton; break;
      case Qt::RightButton: bt = vl::RightButton; break;
      case Qt::MidButton:   bt = vl::MiddleButton; break;
      default:
        bt = vl::UnknownButton; break;
      }

      Event e;
      e.type=Event::MOUSEPRESS;
      e.button = bt;
      e.x=ev->x();
      e.y=ev->y();
      mythread->push_event(e);
    }

    void mouseReleaseEvent(QMouseEvent* ev)
    {
      vl::EMouseButton bt = vl::NoButton;
      switch(ev->button())
      {
      case Qt::LeftButton:  bt = vl::LeftButton; break;
      case Qt::RightButton: bt = vl::RightButton; break;
      case Qt::MidButton:   bt = vl::MiddleButton; break;
      default:
        bt = vl::UnknownButton; break;
      }
      Event e;
      e.type=Event::MOUSERELEASE;
      e.button = bt;
      e.x=ev->x();
      e.y=ev->y();
      mythread->push_event(e);
    }

    void wheelEvent(QWheelEvent* ev)
    {
      Event e;
      e.type=Event::WHEEL;
      e.wheel = (double)ev->delta() / 120.0;
      mythread->push_event(e);
    }

    void keyPressEvent(QKeyEvent*)
    {
      unsigned short unicode_ch = 0;
      vl::EKey key = vl::Key_None;
      //translateKeyEvent(ev, unicode_ch, key);
      Event e;
      e.type = Event::KEYPRESS;
      e.key = key;
      e.unicode_ch = unicode_ch;
      mythread->push_event(e);
    }

    void keyReleaseEvent(QKeyEvent*)
    {
      unsigned short unicode_ch = 0;
      vl::EKey key = vl::Key_None;
      //translateKeyEvent(ev, unicode_ch, key);
      Event e;
      e.type = Event::KEYRELEASE;
      e.key = key;
      e.unicode_ch = unicode_ch;

      mythread->push_event(e); 
    }

    QGLFormat desiredFormat(){
      QGLFormat fmt;
      fmt.setSwapInterval(1);
      return fmt;
    }

    Qt4ThreadedWidget() //(QWidget *parent=0, const char *name="no name" )
      //: QGLWidget //(desiredFormat())
      : mythread(new MyThread(this))
      {
      }


    ~Qt4ThreadedWidget()
    {
      //dispatchDestroyEvent();
    }


    virtual void init_qt()  = 0;
    virtual void init_vl()  = 0;

    
    void start_thread(){
      mythread->start();
    }


    bool initQt4Widget(const vl::String& title, const QGLContext* shareContext=0, int x=0, int y=0, int width=640, int height=480)
    {
      return mythread->initQt4Widget(title, shareContext, x, y, width, height);
    }
  };
}

#endif
