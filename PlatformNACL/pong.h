// Copyright (c) 2011 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXAMPLES_PONG_PONG_H_
#define EXAMPLES_PONG_PONG_H_

#include <string>

#include "ppapi/c/pp_file_info.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"

#include "ppapi/cpp/rect.h"
#include <ppapi/cpp/module.h>

namespace pp {
class Graphics2D;
class ImageData;
class Instance;
class KeyboardInputEvent;
class Rect;
class Size;
}  // namespace pp

class View {
 public:
  explicit View(pp::Instance* instance);
  ~View();

  const uint32_t& last_key_code() const {
    return last_key_code_;
  }
  void set_left_paddle_rect(const pp::Rect& left_paddle_rect) {
    left_paddle_rect_ = left_paddle_rect;
  }
  void set_right_paddle_rect(const pp::Rect& right_paddle_rect) {
    right_paddle_rect_ = right_paddle_rect;
  }
  void set_ball_rect(const pp::Rect& ball_rect) {
    ball_rect_ = ball_rect;
  }
  void set_flush_pending(bool flush_pending) {
    flush_pending_ = flush_pending;
  }

  void ClearBuffer();

  pp::Size GetSize() const;
  bool KeyDown(const pp::KeyboardInputEvent& key);
  bool KeyUp(const pp::KeyboardInputEvent& key);
  bool KeyChar(const pp::KeyboardInputEvent& key);
  void Draw();
  void UpdateView(const pp::Rect& position,
                  const pp::Rect& clip,
                  pp::Instance* instance);

  pp::Instance* GetInstance() {return instance_;}

 private:
  pp::Instance* const instance_;  // weak
  // Create and initialize the 2D context used for drawing.
  void CreateContext(const pp::Size& size, pp::Instance* instance);
  // Destroy the 2D drawing context.
  void DestroyContext();
  // Push the pixels to the browser, then attempt to flush the 2D context.  If
  // there is a pending flush on the 2D context, then update the pixels only
  // and do not flush.
  void FlushPixelBuffer();
  bool IsContextValid() const;

  uint32_t last_key_code_;
  // Geometry for drawing
  pp::Rect left_paddle_rect_;
  pp::Rect right_paddle_rect_;
  pp::Rect ball_rect_;
  // Drawing stuff
  bool flush_pending_;
  pp::Graphics2D* graphics_2d_context_;
  pp::ImageData* pixel_buffer_;
};


// The Instance class.  One of these exists for each instance of your NaCl
// module on the web page.  The browser will ask the Module object to create
// a new Instance for each occurrence of the <embed> tag that has these
// attributes:
//     type="application/x-nacl"
//     nacl="pong.nmf"
class Pong : public pp::Instance {
 public:
  explicit Pong(PP_Instance instance);
  virtual ~Pong();

  // Open the file (if available) that stores the game scores.
  virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);

  // Update the graphics context to the new size, and regenerate |pixel_buffer_|
  // to fit the new size as well.
  virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

  virtual bool HandleInputEvent(const pp::InputEvent& event);

  void ClearBuffer();

  // Called by the browser to handle the postMessage() call in Javascript.
  // The message in this case is expected to contain the string 'update', or
  // 'resetScore' in order to invoke either the Update or ResetScore function
  // respectively.
  virtual void HandleMessage(const pp::Var& var_message);

  void InternalUpdate();
  void Update();

 private:
  Pong(const Pong&);  // Disallow copy

  View* view_;
};

#endif  // EXAMPLES_PONG_PONG_H_
