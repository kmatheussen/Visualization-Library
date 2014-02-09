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
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>
#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFormat>

namespace vlQt4
{

  class VLQT4_EXPORT Qt4ThreadedWidget : public QGLWidget {
    //    Q_OBJECT

  public:


    struct MyThread : public QThread, vl::OpenGLContext {
      Qt4ThreadedWidget *_widget;

      QTime time;

      MyThread(Qt4ThreadedWidget* widget)
        : _widget(widget)
      { }

      void update(){
        printf("update callled\n");
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
        Qt4ThreadedWidget *widget = _widget;

        makeCurrent();

        widget->init_vl();

        dispatchResizeEvent(2000,1024);

        for(;;){

          //widget->swapBuffers();    
          
          dispatchRunEvent();

          //printf("width/height: %d/%d\n",width(),height());
          
          if(time.elapsed()>18)
            printf("hepp %d\n",(int)time.elapsed());
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
    void 	resizeEvent ( QResizeEvent *  ) {}
    //virtual void 	updateGL () {}
    //virtual void 	paintGL () {}
    //virtual bool 	event ( QEvent * e ){ return true;    }

    void mousePressEvent(QMouseEvent*)
    {
      printf("hello\n");
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
