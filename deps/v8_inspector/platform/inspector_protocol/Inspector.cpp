// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/inspector_protocol/Inspector.h"
#include "platform/v8_inspector/public/V8Inspector.h"

namespace inspector {

Inspector::Inspector(v8::Isolate* isolate, v8::Local<v8::Context> context) {
}

Inspector::~Inspector()
{
    disconnectFrontend();
}

} // namespace inspector
