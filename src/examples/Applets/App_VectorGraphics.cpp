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

#include <stdlib.h>
#include <unistd.h>

#include "BaseDemo.hpp"
#include <vlCore/Colors.hpp>
#include <vlVG/VectorGraphics.hpp>
#include <vlVG/SceneManagerVectorGraphics.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Array.hpp>
#include <vlGraphics/Font.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/Text.hpp>

static inline float scale(float x, float x1, float x2, float y1, float y2){
  return y1 + ( ((x-x1)*(y2-y1))
                /
                (x2-x1)
                );
}

static float rnd(float min, float max){
  return scale(rand(), 0, RAND_MAX, min, max);
}

class App_VectorGraphics: public BaseDemo
{
public:
  virtual void initEvent()
  //virtual void initialize()
  {
    printf("******************************** HEPPPPPPPPPPPPPPPP \n");

    vl::Log::notify(appletInfo());

    // disable trackball and ghost camera manipulator
    trackball()->setEnabled(false);
    ghostCameraManipulator()->setEnabled(false);

    // camera setup

    rendering()->as<vl::Rendering>()->setNearFarClippingPlanesOptimized(false);
    rendering()->as<vl::Rendering>()->camera()->setProjectionOrtho(-0.5f);
    // reset view matrix to I
    rendering()->as<vl::Rendering>()->camera()->setViewMatrix( vl::mat4() );

    // load images used later as textures
    
    // load colorful pattern & substitute black with transparent blue
    vl::ref<vl::Image> pattern  = vl::loadImage("/images/pattern.bmp");
    pattern->substituteColorRGB_RGBA(0x000000,0x0000FFAA);
    // load transparent star
    vl::ref<vl::Image> star = vl::loadImage("/images/star.png");
    // colorize the point to green and black
    vl::ref<vl::Image> circle16_bg = vl::loadImage("/images/circle16.png");
    circle16_bg->substituteColorGreenKey(0x00FF00,0x000000);
    // colorize the point to yellow and red
    vl::ref<vl::Image> circle16_yr = vl::loadImage("/images/circle16.png");
    circle16_yr->substituteColorGreenKey(0xFFFF00,0xFF0000);
    // generate the color spectrums
    vl::ref<vl::Image> spectrum1 = vl::makeColorSpectrum(128, vl::blue,  vl::green, vl::yellow, vl::red);
    vl::ref<vl::Image> spectrum2 = vl::makeColorSpectrum(128, vl::black, vl::white, vl::gray,   vl::black);

    // add a new VectorGraphics to our SceneManagerVectorGraphics
  
    vl::ref<vl::VectorGraphics> vg = new vl::VectorGraphics;
    vg->setLineSmoothing(true);
    vg->setPolygonSmoothing(true);

    vl::ref<vl::SceneManagerVectorGraphics> vgscene = new vl::SceneManagerVectorGraphics;
    vgscene->vectorGraphicObjects()->push_back(vg.get());
    rendering()->as<vl::Rendering>()->sceneManagers()->push_back(vgscene.get());

    // start drawing with our new VectorGraphics object!

    vg->startDrawing();

      // clear the viewport
      vg->clearColor(vl::white);

      // ###### textured quad rendering ######

      vg->setImage(pattern.get());
      // this way the texels are perfectly centered on the pixels
      vg->translate(-0.5f,-0.5f);

      vg->pushMatrix();
        // textured quad #1 repeat texturing
        vg->translate(10,110);
        vg->setTextureMode(vl::TextureMode_Repeat);
        vg->fillQuad( 0,0 , pattern->width()*3.0f,pattern->height()*3.0f );
        // textured quad #2 stretch texturing
        vg->translate(100,0);
        vg->setTextureMode(vl::TextureMode_Clamp);
        vg->fillQuad( 0,0 , pattern->width()*3.0f,pattern->height()*3.0f );
        // textured quad #3 stretch texturing
        vg->translate(100,0);
        vg->setImage(spectrum2.get());
        vg->setTextureMode(vl::TextureMode_Clamp);
        vg->fillQuad( 0,0 , pattern->width()*3.0f,pattern->height()*3.0f );
      vg->popMatrix();

      // ###### line rendering ###### 

      vg->setImage(NULL);
      vg->setLineWidth(1.0f);
      vg->setColor(vl::black);
      vg->resetMatrix();
      vg->translate(10,250);

      vg->setLineStipple(vl::LineStipple_Dash);
      vg->drawLine(0,0, 200,0);

      vg->translate(0,10);
      vg->setLineStipple(vl::LineStipple_Dash4);
      vg->drawLine(0,0, 200,0);

      vg->translate(0,10);
      vg->setLineStipple(vl::LineStipple_Dash8);
      vg->drawLine(0,0, 200,0);

      vg->translate(0,10);
      vg->setLineStipple(vl::LineStipple_DashDot);
      vg->drawLine(0,0, 200,0);

      vg->translate(0,10);
      vg->setLineStipple(vl::LineStipple_DashDotDot);
      vg->drawLine(0,0, 200,0);

      vg->translate(0,10);
      vg->setLineStipple(vl::LineStipple_Dot);
      vg->drawLine(0,0, 200,0);

      vg->translate(0,10);
      vg->setLineStipple(vl::LineStipple_Solid);
      vg->drawLine(0,0, 200,0);

      vg->resetMatrix();
      vg->translate(10,350);
      for(int x=0; x<=100; x+=4)
        vg->drawLine(x,0,x,100);
      for(int y=0; y<=100; y+=4)
        vg->drawLine(0,y,100,y);

      // ###### textured point rendering + scissor ###### 

      // with the Scissor we can clip the rendering against a specific rectangular area
      int scissor_w = 200;
      int scissor_h = 200;
      vg->setScissor(256-scissor_w/2,256+128-scissor_h/2,scissor_w,scissor_h);

      vg->setColor(vl::fvec4(1,1,1,0.5f)); // transparent white
      vg->setPoint(circle16_bg.get()); // the same as setImage(image) + setPointSize(image->width())
      vg->resetMatrix();
      vg->translate(256,256+128);
      // generate the points
      std::vector<vl::dvec2> points;
      for(int i=0; i<1000; ++i)
      {
        points.push_back(vl::dvec2(rand()%128-64,rand()%128-64));
        points[i].normalize();
        points[i] *= (rand()%1280) / 10.0f;
        // snap to integer coordinates to avoid aliasing problems
        points[i] = vl::trunc(points[i]);
      }
      // draw the points
      vg->drawPoints(points);

      vg->setPoint(circle16_yr.get()); // the same as setImage(image) + setPointSize(image->width())
      points.clear();
      for(int i=0; i<200; ++i)
      {
        points.push_back(vl::dvec2(rand()%128-64,rand()%128-64));
        points[i].normalize();
        points[i] *= (rand()%1280) / 10.0f;
        // snap to integer coordinates to avoid aliasing problems
        points[i] = vl::trunc(points[i]);
      }
      // draw the points
      vg->drawPoints(points);

      vg->removeScissor();

      // ###### rounded point rendering ###### 

      vg->setImage(NULL);
      vg->setPointSize(7);
      vg->setPointSmoothing(true); /* default value */
      vg->setColor(vl::crimson);
      vg->translate(196,64);
      points.clear();
      for(int i=0; i<100; ++i)
      {
        points.push_back(vl::dvec2(rand()%128-64,rand()%128-64));
        points[i].normalize();
        points[i] *= (rand()%640) / 10.0f;
        // snap to integer coordinates to avoid aliasing problems
        points[i] = vl::trunc(points[i]);
      }
      vg->drawPoints(points);

      // ###### squared point rendering ###### 

      vg->setImage(NULL);
      vg->setPointSize(5);
      vg->setPointSmoothing(false);
      vg->setColor(vl::green);
      vg->translate(0,-128);
      points.clear();
      for(int i=0; i<100; ++i)
      {
        points.push_back(vl::dvec2(rand()%128-64,rand()%128-64));
        points[i].normalize();
        points[i] *= (rand()%640) / 10.0f;
        // snap to integer coordinates to avoid aliasing problems
        points[i] = vl::trunc(points[i]);
      }
      vg->drawPoints(points);

      // ###### stencil buffer rendering ###### 

      // reset states
      vg->resetMatrix();
      vg->setColor(vl::white);
      vg->setImage(NULL);

      // clear the stencil buffer
      vg->clearStencil(0);
      // enable stencil test
      vg->setStencilTestEnabled(true);
      // setup stencil test: writes 0x01 on the stencil buffer when rendering polygons, lines etc.
      vg->setStencilFunc(vl::FU_NOTEQUAL, 0x01, 0x01);
      vg->setStencilOp(vl::SO_REPLACE, vl::SO_REPLACE, vl::SO_REPLACE);

      // ###### render the rose on the stencil buffer ###### 

      // rose create and bind transform
      mRoseTransform = new vl::Transform;
#if 1
      rendering()->as<vl::Rendering>()->transform()->addChild(mRoseTransform.get());
      // draw our rotating rose as a set of 4 filled ellipses and bind them to mRoseTransform
      vg->fillEllipse(0,0 , 150,25)->setTransform(mRoseTransform.get());
      vg->pushMatrix();
      vg->rotate(45);
      vg->fillEllipse(0,0 , 150,25)->setTransform(mRoseTransform.get());
      vg->rotate(45);
      vg->fillEllipse(0,0 , 150,25)->setTransform(mRoseTransform.get());
      vg->rotate(45);
      vg->fillEllipse(0,0 , 150,25)->setTransform(mRoseTransform.get());
      vg->popMatrix();

      // ###### fill the rose with color ###### 

      // setup stencil test: renders only where the stencil buffer is set to 0x01
      vg->setStencilFunc(vl::FU_EQUAL, 0x01, 0x01);
      vg->setStencilOp(vl::SO_KEEP, vl::SO_KEEP, vl::SO_KEEP);
#endif

      // make sure our matrix is clean
      vg->resetMatrix();
      // render random blue ellipses
      vg->setColor(vl::blue);
      for(int i=0; i<400; ++i)
        vg->drawEllipse(rand()%512,rand()%512 , rand()%20+10,rand()%20+10);
      // renders concentric red circles
      vg->setColor(vl::red);
      for(int i=0; i<256/4; ++i)
        vg->drawEllipse(256,256 , i*8,i*8);

      // finish with the stencil
      vg->setStencilTestEnabled(false);

      int ymax = -1500;

#if 1
      // render text following our rotating rose
      vl::Font *font = vl::defFontManager()->acquireFont("/font/bitstream-vera/Vera.ttf", 14);
      font->setSmooth(true);
      //font->setFreeTypLoadForceAutoHint(true);
      vg->setFont(font);
      vg->setColor(vl::black);
      // note that the 2D text is not transformed by mRoseTransform but just follows an idea point transformed by mRoseTransform.
      //for(int n=-500;n<500;n+=20){
      int n = 0;
        vl::Text *text = new vl::Text;
        vl::ref<vl::Transform> transform = new vl::Transform;
        transform->translate(n,n,0);
        text->setText("Stencil buffer in action here!");
        text->setMode(vl::Text2D);
        /*
          text->setInterlineSpacing(0.1f);
          text->setKerningEnabled(false);
        */
        text->setAlignment(vl::AlignHCenter|vl::AlignVCenter);
        vg->drawText(text)->setTransform(transform.get());
        rendering()->as<vl::Rendering>()->transform()->addChild(transform.get());
        //}

#if 1
        for(int n=0; n< 300 ; n++){
          vl::Text *text = new vl::Text;
          vl::ref<vl::Transform> transform = new vl::Transform;
          transform->translate(rnd(-100,1600),rnd(ymax,1000),0);
          text->setText("C#3"); //Stencil buffer in action here!");
          text->setMode(vl::Text2D);
        /*
          text->setInterlineSpacing(0.1f);
          text->setKerningEnabled(false);
        */
          text->setAlignment(vl::AlignHCenter|vl::AlignVCenter);
          vg->drawText(text)->setTransform(transform.get());
          rendering()->as<vl::Rendering>()->transform()->addChild(transform.get());
        }
#endif

#else
      vg->drawText("Stencil buffer in action here!",vl::AlignHCenter|vl::AlignVCenter)->setTransform(mRoseTransform.get());
#endif

      /* my experimental line */
      for(int n=0; n< 100 ; n++){
#if 1
        vl::ref<vl::Image> spectrum = vl::makeColorSpectrum(16, vl::blue,  vl::green);
        vg->setColor(vl::fvec4(1,1,1,0.75f)); // transparent whitevl::white);
        vg->setImage(spectrum.get());
#else
        vg->setColor(vl::blue);
#endif
#if 1   
        //vg->translate(50,50,0);
        float x = rnd(-100,1600);
        float y = rnd(ymax,1000);
        vg->fillQuad(x,y,x+600,y+100);
#else
        //vg->translate(rnd(-100,1600),rnd(-100,1000),0);
        vg->setLineWidth(1.5f);
        vg->setLineStipple(vl::LineStipple_Solid);
        vg->drawLine(rnd(-100,1600),rnd(ymax,1000),rnd(-100,1600),rnd(ymax,1000));
        //vg->drawLine(0,0, -200,200);
#endif
      }

      
      // ###### draws a rotated text ###### 

      vg->setColor(vl::black);
      vg->setFont("/font/VeraMono.ttf", 14, true);
      vg->pushMatrix();
      vg->rotate(45);
      vg->drawText(256, 256, "Rotated Text", vl::AlignHCenter|vl::AlignVCenter);
      vg->popMatrix();

      // ###### transparent star image ###### 

      vg->pushState();
        vg->setColor(vl::fvec4(1,1,1,0.75f)); // transparent white
        vg->setImage(star.get());
        vg->translate(-star->width()/2,-star->height()/2); // center the quad
        vl::Actor* rota_star = vg->fillQuad(0,0,star->width(),star->height());
        mStarTransform = new vl::Transform;
        rota_star->setTransform( mStarTransform.get() );
        rendering()->as<vl::Rendering>()->transform()->addChild( mStarTransform.get() );
      vg->popState();

      // ###### how to instance multiple times the same object ###### 

      // generate 5 corner star: line loop primitive
      std::vector<vl::dvec2> star_line_loop;
      for(int i=0; i<5; ++i)
      {
        vl::dvec3 v = vl::dmat4::getRotation(90.0+i*360.0/5.0*2.0 , 0,0,1) * vl::dvec3(50.0,0,0);
        // snap to integer coordinates to avoid aliasing
        v = vl::trunc(v);
        star_line_loop.push_back(v.xy());
      }

      // ###### generate 5 corner star: lines primitive ###### 
      std::vector<vl::dvec2> star_lines;
      for(int i=0; i<5; ++i)
      {
        vl::dvec3 v1 = vl::dmat4::getRotation(90.0+i*360.0/5.0*2.0 , 0,0,1) * vl::dvec3(50.0,0,0);
        vl::dvec3 v2 = vl::dmat4::getRotation(90.0+((i+1)%5)*360.0/5.0*2.0 , 0,0,1) * vl::dvec3(50.0,0,0);
        // snap to integer coordinates to avoid aliasing
        v1 = vl::trunc(v1);
        v2 = vl::trunc(v2);
        star_lines.push_back(v1.xy());
        star_lines.push_back(v2.xy());
      }

      // star1
      vg->setLineWidth(4.0f);
      vg->setColor(vl::gold);
      vl::Actor* star1 = vg->drawLineLoop(star_line_loop);
      // star2 - recycle the geometry from star1
      vg->setLineWidth(2.0f);
      vg->setColor(vl::red);
      vl::Actor* star2 = vg->drawActorCopy(star1);
      // star3 - recycle the geometry from star1
      vg->setLineWidth(1.0f);
      vg->setLineStipple(vl::LineStipple_DashDotDot);
      vl::Actor* star3 = vg->drawActorCopy(star1);
      // star4 - texturing #1
      vg->setColor(vl::white); // make sure color is white so that the texture color is not filtered
      vg->setImage(spectrum1.get());
      vg->setLineWidth(2.0f);
      vg->setLineStipple(vl::LineStipple_Solid);
      // Here we call drawLineLoop() because we need to create a new geometry instance
      // so that VL can generate appropriate UV texture coordinates that are dependent
      // on the currently active Image.
      vl::Actor* star4 = vg->drawLineLoop(star_line_loop);
      // star5 - texturing #2
      // Here we draw the star using drawLines() instead or drawLineLoop().
      // Note how drawLines() and drawLineLoop() generate different texturing effects.
      vl::Actor* star5 = vg->drawLines(star_lines);

      // render the 4 instances in 4 different points
      star1->setTransform( new vl::Transform );
      star2->setTransform( new vl::Transform );
      star3->setTransform( new vl::Transform );
      star4->setTransform( new vl::Transform );
      star5->setTransform( new vl::Transform );
      star1->transform()->setLocalMatrix( vl::mat4::getTranslation(50,50,0) );
      star2->transform()->setLocalMatrix( vl::mat4::getTranslation(150,50,0) );
      star3->transform()->setLocalMatrix( vl::mat4::getTranslation(250,50,0) );
      star4->transform()->setLocalMatrix( vl::mat4::getTranslation(350,50,0) );
      star5->transform()->setLocalMatrix( vl::mat4::getTranslation(450,50,0) );
      // Since they are not animated we can setup the matrices without binding it to the Rendering's root Transform
      star1->transform()->computeWorldMatrix();
      star2->transform()->computeWorldMatrix();
      star3->transform()->computeWorldMatrix();
      star4->transform()->computeWorldMatrix();
      star5->transform()->computeWorldMatrix();

    vg->endDrawing();
  }

  vl::Time time;
  vl::Time time2;
  double counter;

  virtual void updateScene()
  {
    static bool inited =false;
    if(inited==false){
      time.start();
      counter = 0;
      inited=true;
    }

    //printf("UPdating scene\n");

    //vl::Time time;
    //time.start();

    int pos = time.elapsed()*200.0;
    //vl::mat4 mat = vl::mat4::getRotation(vl::Time::currentTime() * 60.0f, 0, 0, 1);
    //mat = vl::mat4::getTranslation(0,pos,0); // * mat;
    vl::mat4 mat = vl::mat4::getTranslation(0,counter,0); // * mat;
    //mat = vl::mat4::getRotation(vl::Time::currentTime() * 5.0f / 40.0, 0, 0, 1) * mat;
    //mat = vl::mat4::getRotation(0, 0, 0, 1) * mat;
    mat = vl::mat4::getTranslation(256,256,0) * mat;
    //mRoseTransform->setLocalAndWorldMatrix(mat);

    rendering()->as<vl::Rendering>()->camera()->setViewMatrix( mat );

    counter+=1.177;
    if(counter >= 800){
      counter = -500;
      //time.start();
      //updateScene();
      //openglContext()->update();
      rendering()->as<vl::Rendering>()->sceneManagers()->clear();
      initEvent();
    }

    //printf("pos: %d\n",pos);
    if(pos>500)
      time.start();

    /*
    mat.setIdentity();
    mat.rotate(vl::Time::currentTime()*60/100.0, 0,0,1);
    mat.translate(256,256,0);
    mat.translate(200*cos(vl::Time::currentTime()*vl::fPi*2.0f/17.0f/100.0),0,0);
    mat.translate(0,200*sin(vl::Time::currentTime()*vl::fPi*2.0f/9.0f/100.0),0);
    mStarTransform->setLocalMatrix(mat);
    */

#if 0
    vl::mat4 mat = vl::mat4::getRotation(0.0f, 0, 0, 1);
    mat.setIdentity();
    mat.translate(200*cos(vl::Time::currentTime()*vl::fPi*2.0f/17.0f/100.0),0,0);

    mRoseTransform->setLocalAndWorldMatrix(mat);
#endif

    //sleep(1);
    //if(time2.elapsed()*1000.0>18.0)
    //  printf("updating scene %fms\n",(float)time2.elapsed()*1000.0);
    time2.start();
  }

  void resizeEvent(int w, int h)
  {
    rendering()->as<vl::Rendering>()->camera()->viewport()->setWidth(w);
    rendering()->as<vl::Rendering>()->camera()->viewport()->setHeight(h);
    rendering()->as<vl::Rendering>()->camera()->setProjectionOrtho(-0.5f);
  }

protected:
  vl::ref<vl::Transform> mRoseTransform;
  vl::ref<vl::Transform> mStarTransform;
};

// Have fun!

BaseDemo* Create_App_VectorGraphics() { return new App_VectorGraphics; }
