// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef INSPECTOR_PROTOCOL_INSPECTOR_H
#define INSPECTOR_PROTOCOL_INSPECTOR_H

#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/Values.h"

#include <v8.h>

namespace inspector {

namespace protocol {
class Dispatcher;
class Frontend;
class FrontendChannel;
}

class Inspector {
public:
    Inspector(v8::Isolate*, v8::Local<v8::Context>);
    ~Inspector();

    // Transport interface.
    virtual void connectFrontend(protocol::FrontendChannel*) = 0;
    virtual void disconnectFrontend() = 0;
    virtual void dispatchMessageFromFrontend(const String16& message) = 0;


}; // class Inspector
} // namespace inspector

#endif // INSPECTOR_PROTOCOL_INSPECTOR_H
