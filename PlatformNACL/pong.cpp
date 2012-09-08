// Copyright (c) 2011 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "pong.h"
#include "platformnacl.h"
#include "../RTCode/RayTracer.h"

#include <stdio.h>
#include <cmath>
#include <string>
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/point.h"
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

CPlatformNaCl g_NaClPlatform;


// Input event key codes.  PPAPI uses Windows Virtual key codes.
const uint32_t kSpaceBar = 0x20;
const uint32_t kUpArrow = 0x26;
const uint32_t kDownArrow = 0x28;

const uint32_t kOpaqueColorMask = 0xff000000;  // Opaque pixels.
const uint32_t kWhiteMask = 0xffffff;

void UpdateCallback(void* data, int32_t /*result*/);

// This is called by the browser when the 2D context has been flushed to the
// browser window.
void FlushCallback(void* data, int32_t result) {
  static_cast<View*>(data)->set_flush_pending(false);
	Pong *pong = static_cast<Pong*>(static_cast<View*>(data)->GetInstance());
  UpdateCallback(pong,0);
}
View::View(pp::Instance* instance)
    : instance_(instance), last_key_code_(0x0), flush_pending_(false),
    graphics_2d_context_(NULL), pixel_buffer_(NULL) {}

View::~View() {
  DestroyContext();
  delete pixel_buffer_;
}

pp::Size View::GetSize() const {
  pp::Size size;
  if (graphics_2d_context_) {
    size.SetSize(graphics_2d_context_->size().width(),
                  graphics_2d_context_->size().height());
  }
  return size;
}

bool View::KeyChar(const pp::KeyboardInputEvent& key)
{
  pp::Var keyPressed = key.GetCharacterText();
  
  if(keyPressed.is_string())
  {
	CRayTracer::GetSingleton().OnKeyTyped(keyPressed.AsString().c_str()[0]);
  }
  return false;
}

bool View::KeyDown(const pp::KeyboardInputEvent& key)
{
  CRayTracer::GetSingleton().OnKeyDown(key.GetKeyCode());

  if(key.GetKeyCode() == 8)
  {
	  CRayTracer::GetSingleton().OnKeyTyped(8);
  }

  return false;
}

bool View::KeyUp(const pp::KeyboardInputEvent& key)
{
  CRayTracer::GetSingleton().OnKeyUp(key.GetKeyCode());
  return false;
}

void View::Draw()
{
	uint32_t* pixels = static_cast<uint32_t*>(pixel_buffer_->data());
	if (NULL == pixels)
		return;

	static struct timeval nLastTickCount;
	static bool bInited = false;
	static int nFrame = 0;
	MRTFLOAT fFrameTime = 0;

	nFrame++;

	if(!bInited)
	{
		g_NaClPlatform.Init(instance_);
		//CRayTracer::GetSingleton().Init(&g_NaClPlatform,pixel_buffer_->size().width(), pixel_buffer_->size().height());
		CRayTracer::GetSingleton().Init(&g_NaClPlatform,500,500);
		bInited = true;
		gettimeofday(&nLastTickCount, NULL);
	}
	else
	{
		struct timeval nTickCount;
		gettimeofday(&nTickCount, NULL);

		double tS = nLastTickCount.tv_sec*1000000 + (nLastTickCount.tv_usec);
		double tE = nTickCount.tv_sec*1000000  + (nTickCount.tv_usec);
		tS /= 1000000.0f;
		tE /= 1000000.0f;

		fFrameTime = tE - tS;

		nLastTickCount = nTickCount;
	}

	CRayTracer::GetSingleton().RenderFrame(fFrameTime);

	CRayTracer::GetSingleton().CopyFrameToPixels((unsigned char *)pixels,pixel_buffer_->size().width() * 4);  

	FlushPixelBuffer();
}

void View::UpdateView(const pp::Rect& position,
                      const pp::Rect& clip,
                      pp::Instance* instance) {
  const int32_t width =
      pixel_buffer_ ? pixel_buffer_->size().width() : 0;
  const int32_t height =
      pixel_buffer_ ? pixel_buffer_->size().height() : 0;

  if (position.size().width() == width &&
      position.size().height() == height)
    return;  // Size didn't change, no need to update anything.

  // Create a new device context with the new size.
  DestroyContext();
  CreateContext(position.size(), instance);
  // Delete the old pixel buffer and create a new one.
  delete pixel_buffer_;
  pixel_buffer_ = NULL;
  if (graphics_2d_context_ != NULL) {
    pixel_buffer_ = new pp::ImageData(instance, PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                                      graphics_2d_context_->size(),
                                      false);
  }
}

void View::ClearBuffer()
{
	//fill it in with black since the ray tracer won't render to the entire thing when less than full sized
	uint32_t* pixels = static_cast<uint32_t*>(pixel_buffer_->data());
	memset(pixels,0,sizeof(uint32_t) * pixel_buffer_->size().width() * pixel_buffer_->size().height());
}

void View::CreateContext(const pp::Size& size, pp::Instance* instance) {
  if (IsContextValid())
    return;
  graphics_2d_context_ = new pp::Graphics2D(instance, size,
                                            false);
  if (!instance->BindGraphics(*graphics_2d_context_)) {
    instance_->PostMessage(pp::Var("ERROR: Couldn't bind the device context"));
  }
}

void View::DestroyContext() {
  if (!IsContextValid())
    return;
  delete graphics_2d_context_;
  graphics_2d_context_ = NULL;
}

void View::FlushPixelBuffer() {
  if (!IsContextValid())
    return;
  // Note that the pixel lock is held while the buffer is copied into the
  // device context and then flushed.
  graphics_2d_context_->PaintImageData(*pixel_buffer_, pp::Point());
  if (flush_pending_)
    return;
  flush_pending_ = true;
  graphics_2d_context_->Flush(pp::CompletionCallback(&FlushCallback, this));
}

bool View::IsContextValid() const {
    return graphics_2d_context_ != NULL;
}

// Callback that is called as a result of pp::Core::CallOnMainThread
void UpdateCallback(void* data, int32_t /*result*/)
{
  Pong* pong = static_cast<Pong*>(data);
  pong->Update();
}

Pong::Pong(PP_Instance instance)
    : pp::Instance(instance),
      view_(NULL)
{
  // Request to receive input events.
  RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_KEYBOARD);
}

Pong::~Pong()
{
  delete view_;
}

void Pong::ClearBuffer()
{
	view_->ClearBuffer();
}

bool Pong::Init(uint32_t argc, const char* argn[], const char* argv[])
{
  if(view_)
	  delete view_;
  view_ = new View(this);
  pp::Core* core = pp::Module::Get()->core();
  core->CallOnMainThread(0, pp::CompletionCallback(UpdateCallback, this));
  return true;
}

void Pong::DidChangeView(const pp::Rect& position, const pp::Rect& clip)
{
  pp::Size view_size = view_->GetSize();
  //const bool view_was_empty = view_size.IsEmpty();
  view_->UpdateView(position, clip, this);
}

bool Pong::HandleInputEvent(const pp::InputEvent& event)
{
	switch(event.GetType())
	{
		case PP_INPUTEVENT_TYPE_MOUSEMOVE:
		{
			pp::MouseInputEvent mouse = pp::MouseInputEvent(event);
			int mousex = mouse.GetPosition().x();
			int mousey = mouse.GetPosition().y();
			g_NaClPlatform.SetMousePosition(mousex,mousey);
			break;
		}
		case PP_INPUTEVENT_TYPE_MOUSEUP:
		{
			return true;
		}
		case PP_INPUTEVENT_TYPE_MOUSEDOWN:
		{
			pp::MouseInputEvent mouse = pp::MouseInputEvent(event);

			if(mouse.GetButton() == 0)
				CRayTracer::GetSingleton().OnLeftClick();

			return true;
		}
		case PP_INPUTEVENT_TYPE_KEYUP:
		{
			pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
			return view_->KeyUp(key);
		}
		case PP_INPUTEVENT_TYPE_KEYDOWN:
		{
			pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
			return view_->KeyDown(key);
		}
		case PP_INPUTEVENT_TYPE_CHAR:
		{
			pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
			return view_->KeyChar(key);
		}
		case PP_INPUTEVENT_TYPE_UNDEFINED:
		case PP_INPUTEVENT_TYPE_MOUSEENTER:
		case PP_INPUTEVENT_TYPE_MOUSELEAVE:
		case PP_INPUTEVENT_TYPE_WHEEL:
		case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
		case PP_INPUTEVENT_TYPE_CONTEXTMENU:
		case PP_INPUTEVENT_TYPE_IME_COMPOSITION_END:
		case PP_INPUTEVENT_TYPE_IME_COMPOSITION_UPDATE:
		case PP_INPUTEVENT_TYPE_IME_COMPOSITION_START:
		case PP_INPUTEVENT_TYPE_IME_TEXT:
		default:
		{
			return false;
		}
	}

	return false;
}

void Pong::HandleMessage(const pp::Var& var_message)
{
  /*
  if (!var_message.is_string())
    return;
  std::string message = var_message.AsString();
  if (message == kResetScoreMethodId) {
    ResetScore();
  }*/
}

void Pong::Update()
{
  //render then schedule another update
  view_->Draw();
}

// The Module class.  The browser calls the CreateInstance() method to create
// an instance of your NaCl module on the web page.  The browser creates a new
// instance for each <embed> tag with type="application/x-nacl".
class PongModule : public pp::Module {
 public:
  PongModule() : pp::Module() {}
  virtual ~PongModule() {}

  // Create and return a PiGeneratorInstance object.
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new Pong(instance);
  }
};

// Factory function called by the browser when the module is first loaded.
// The browser keeps a singleton of this module.  It calls the
// CreateInstance() method on the object you return to make instances.  There
// is one instance per <embed> tag on the page.  This is the main binding
// point for your NaCl module with the browser.
namespace pp {
Module* CreateModule() {
  return new PongModule();
}
}  // namespace pp
