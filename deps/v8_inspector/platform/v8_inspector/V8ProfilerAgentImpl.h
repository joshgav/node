// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8ProfilerAgentImpl_h
#define V8ProfilerAgentImpl_h

#include "platform/inspector_protocol/Allocator.h"
#include "platform/inspector_protocol/String16.h"
#include "platform/inspector_protocol/protocol/Profiler.h"

namespace v8 {
class Isolate;
}

namespace blink {

class V8InspectorSessionImpl;

class V8ProfilerAgentImpl : public inspector::protocol::Profiler::Backend {
    PROTOCOL_DISALLOW_COPY(V8ProfilerAgentImpl);
public:
    V8ProfilerAgentImpl(V8InspectorSessionImpl*, inspector::protocol::FrontendChannel*, inspector::protocol::DictionaryValue* state);
    ~V8ProfilerAgentImpl() override;

    bool enabled() const { return m_enabled; }
    void restore();

    void enable(ErrorString*) override;
    void disable(ErrorString*) override;
    void setSamplingInterval(ErrorString*, int) override;
    void start(ErrorString*) override;
    void stop(ErrorString*, std::unique_ptr<inspector::protocol::Profiler::CPUProfile>*) override;

    void consoleProfile(const String16& title);
    void consoleProfileEnd(const String16& title);

private:
    String16 nextProfileId();

    void startProfiling(const String16& title);
    std::unique_ptr<inspector::protocol::Profiler::CPUProfile> stopProfiling(const String16& title, bool serialize);

    bool isRecording() const;

    V8InspectorSessionImpl* m_session;
    v8::Isolate* m_isolate;
    inspector::protocol::DictionaryValue* m_state;
    inspector::protocol::Profiler::Frontend m_frontend;
    bool m_enabled;
    bool m_recordingCPUProfile;
    class ProfileDescriptor;
    inspector::protocol::Vector<ProfileDescriptor> m_startedProfiles;
    String16 m_frontendInitiatedProfileId;
};

} // namespace blink


#endif // !defined(V8ProfilerAgentImpl_h)
