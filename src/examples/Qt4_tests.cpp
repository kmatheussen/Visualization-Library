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

#include <unistd.h>

#include <vlCore/DiskDirectory.hpp>
#include <vlQt4/Qt4ThreadedWidget.hpp>
#include "Applets/App_RotatingCube.hpp"
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include "tests.hpp"

using namespace vl;
using namespace vlQt4;

class MyQt4ThreadedWidget : public Qt4ThreadedWidget, public TestBattery {
public:

  int _test_num;

  MyQt4ThreadedWidget(int test_num)
    : _test_num(test_num)
  {}

  void runGUI(const vl::String& title, BaseDemo* applet, vl::OpenGLContextFormat format, int x, int y, int width, int height, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center)
  {
    setupApplet(applet, mythread, bk_color, eye, center);

    /* create the applet to be run */
    //ref<Applet> applet = Create_App_VectorGraphics(); //new App_RotatingCube;
    //ref<Applet> applet = new App_RotatingCube;

    //applet->initialize();

    /* create a native Qt4 window */
    //ref<vlQt4::Qt4Widget> qt4_window = new vlQt4::Qt4Widget;
    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    mythread->addEventListener(applet);

#if 0
    /* target the window so we can render on it */
    applet->rendering()->as<Rendering>()->renderer()->setFramebuffer( this->mythread->framebuffer() );
    /* black background */
    applet->rendering()->as<Rendering>()->camera()->viewport()->setClearColor( black );
    /* define the camera position and orientation */
    vec3 eye    = vec3(0,10,35); // camera position
    vec3 center = vec3(0,0,0);   // point the camera is looking at
    vec3 up     = vec3(0,1,0);   // up direction
    mat4 view_mat = mat4::getLookAt(eye, center, up);
    applet->rendering()->as<Rendering>()->camera()->setViewMatrix( view_mat );
    /* Initialize the OpenGL context and window properties */

    int x = 10;
    int y = 10;
    int width = 512;
    int height= 512;
    

    applet->setAppletName("hello");
#endif

    /* Initialize the OpenGL context and window properties */
    initQt4Widget( "hello2", NULL, x, y, width, height );

  }

  virtual void init_qt() {

    // setFormat(fmt) is marked as deprecated so we use this other method
    QGLContext* glctx = new QGLContext(context()->format(), this);
    QGLFormat fmt = context()->format();
        
    /* setup the OpenGL context format */
    vl::OpenGLContextFormat info;
    //info.setDoubleBuffer(true);
    info.setDoubleBuffer(true);
    info.setRGBABits( 8,8,8,8 );
    info.setDepthBufferBits(24);
    info.setFullscreen(false);
    info.setMultisampleSamples(8);
    info.setMultisample(true);
    //info.setMultisample(false);
    
    // double buffer
    fmt.setDoubleBuffer( info.doubleBuffer() );
        
    // color buffer
    fmt.setRedBufferSize( info.rgbaBits().r() );
    fmt.setGreenBufferSize( info.rgbaBits().g() );
    fmt.setBlueBufferSize( info.rgbaBits().b() );
    // setAlpha == true makes the create() function alway fail
    // even if the returned format has the requested alpha channel
    fmt.setAlphaBufferSize( info.rgbaBits().a() );
    fmt.setAlpha( info.rgbaBits().a() != 0 );
        
    // accumulation buffer
    int accum = vl::max( info.accumRGBABits().r(), info.accumRGBABits().g() );
    accum = vl::max( accum, info.accumRGBABits().b() );
    accum = vl::max( accum, info.accumRGBABits().a() );
    fmt.setAccumBufferSize( accum );
    fmt.setAccum( accum != 0 );
        
    // multisampling
    if (info.multisample())
      fmt.setSamples( info.multisampleSamples() );
    fmt.setSampleBuffers( info.multisample() );
        
    // depth buffer
    fmt.setDepthBufferSize( info.depthBufferBits() );
    fmt.setDepth( info.depthBufferBits() != 0 );

    // stencil buffer
    fmt.setStencilBufferSize( info.stencilBufferBits() );
    fmt.setStencil( info.stencilBufferBits() != 0 );
        
    // stereo
    fmt.setStereo( info.stereo() );
        
    // swap interval / v-sync
    fmt.setSwapInterval( info.vSync() ? 1 : 0 );
        
    glctx->setFormat(fmt);
    // this function returns false when we request an alpha buffer
    // even if the created context seem to have the alpha buffer
    /*bool ok = */glctx->create(NULL);
    setContext(glctx);

#ifndef NDEBUG
    printf("--------------------------------------------\n");
    printf("REQUESTED OpenGL Format:\n");
    printf("--------------------------------------------\n");
    printf("rgba = %d %d %d %d\n", fmt.redBufferSize(), fmt.greenBufferSize(), fmt.blueBufferSize(), fmt.alphaBufferSize() );
    printf("double buffer = %d\n", (int)fmt.doubleBuffer() );
    printf("depth buffer size = %d\n", fmt.depthBufferSize() );
    printf("depth buffer = %d\n", fmt.depth() );
    printf("stencil buffer size = %d\n", fmt.stencilBufferSize() );
    printf("stencil buffer = %d\n", fmt.stencil() );
    printf("accum buffer size %d\n", fmt.accumBufferSize() );
    printf("accum buffer %d\n", fmt.accum() );
    printf("stereo = %d\n", (int)fmt.stereo() );
    printf("swap interval = %d\n", fmt.swapInterval() );
    printf("multisample = %d\n", (int)fmt.sampleBuffers() );
    printf("multisample samples = %d\n", (int)fmt.samples() );

    fmt = format();

    printf("--------------------------------------------\n");
    printf("OBTAINED OpenGL Format:\n");
    printf("--------------------------------------------\n");
    printf("rgba = %d %d %d %d\n", fmt.redBufferSize(), fmt.greenBufferSize(), fmt.blueBufferSize(), fmt.alphaBufferSize() );
    printf("double buffer = %d\n", (int)fmt.doubleBuffer() );
    printf("depth buffer size = %d\n", fmt.depthBufferSize() );
    printf("depth buffer = %d\n", fmt.depth() );
    printf("stencil buffer size = %d\n", fmt.stencilBufferSize() );
    printf("stencil buffer = %d\n", fmt.stencil() );
    printf("accum buffer size %d\n", fmt.accumBufferSize() );
    printf("accum buffer %d\n", fmt.accum() );
    printf("stereo = %d\n", (int)fmt.stereo() );
    printf("swap interval = %d\n", fmt.swapInterval() );
    printf("multisample = %d\n", (int)fmt.sampleBuffers() );
    printf("multisample samples = %d\n", (int)fmt.samples() );
    printf("--------------------------------------------\n");
#endif

    QGLWidget::setWindowState(QGLWidget::windowState() & (~Qt::WindowFullScreen));
    //QGLWidget::setWindowState(QGLWidget::windowState() | Qt::WindowFullScreen);        
  }

  virtual void init_vl() {
    mythread->initGLContext();
    /* parse command line arguments */
    //int   test = 42;
        
    vl::OpenGLContextFormat format;
    run(_test_num, "nope", format);

//applet->initEvent();
  }
};

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

  QApplication app(argc, argv);

  /* init Visualization Library */
  //VisualizationLibrary::init();

  if(argc==1){
    printf("usage: %s <test-num>\n",argv[0]);
    exit(-1);
  }

  MyQt4ThreadedWidget widget(atoi(argv[1]));
  widget.init_qt();
  widget.show();
  QApplication::processEvents(QEventLoop::AllEvents, 30);

  widget.start_thread();

  //sleep(5);

  app.exec();

  return 0;
}
