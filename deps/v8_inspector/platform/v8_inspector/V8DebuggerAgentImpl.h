// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8DebuggerAgentImpl_h
#define V8DebuggerAgentImpl_h

#include "platform/inspector_protocol/Collections.h"
#include "platform/inspector_protocol/String16.h"
#include "platform/inspector_protocol/protocol/Debugger.h"
#include "platform/v8_inspector/V8DebuggerImpl.h"

namespace inspector {
namespace protocol {
class DictionaryValue;
}
}

namespace blink {

class JavaScriptCallFrame;
class PromiseTracker;
class V8InspectorSessionImpl;
class V8Regex;
class V8StackTraceImpl;

using inspector::protocol::Maybe;

class V8DebuggerAgentImpl : public inspector::protocol::Debugger::Backend {
    PROTOCOL_DISALLOW_COPY(V8DebuggerAgentImpl);
public:
    enum SkipPauseRequest {
        RequestNoSkip,
        RequestContinue,
        RequestStepInto,
        RequestStepOut,
        RequestStepFrame
    };

    enum BreakpointSource {
        UserBreakpointSource,
        DebugCommandBreakpointSource,
        MonitorCommandBreakpointSource
    };

    V8DebuggerAgentImpl(V8InspectorSessionImpl*, inspector::protocol::FrontendChannel*, inspector::protocol::DictionaryValue* state);
    ~V8DebuggerAgentImpl() override;
    void restore();

    // Part of the protocol.
    void enable(ErrorString*) override;
    void disable(ErrorString*) override;
    void setBreakpointsActive(ErrorString*, bool active) override;
    void setSkipAllPauses(ErrorString*, bool skipped) override;
    void setBreakpointByUrl(ErrorString*,
        int lineNumber,
        const Maybe<String16>& optionalURL,
        const Maybe<String16>& optionalURLRegex,
        const Maybe<int>& optionalColumnNumber,
        const Maybe<String16>& optionalCondition,
        String16*,
        std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::Location>>* locations) override;
    void setBreakpoint(ErrorString*,
        std::unique_ptr<inspector::protocol::Debugger::Location>,
        const Maybe<String16>& optionalCondition,
        String16*,
        std::unique_ptr<inspector::protocol::Debugger::Location>* actualLocation) override;
    void removeBreakpoint(ErrorString*, const String16& breakpointId) override;
    void continueToLocation(ErrorString*,
        std::unique_ptr<inspector::protocol::Debugger::Location>,
        const Maybe<bool>& interstateLocationOpt) override;
    void getBacktrace(ErrorString*,
        std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::CallFrame>>*,
        Maybe<inspector::protocol::Runtime::StackTrace>*) override;
    void searchInContent(ErrorString*,
        const String16& scriptId,
        const String16& query,
        const Maybe<bool>& optionalCaseSensitive,
        const Maybe<bool>& optionalIsRegex,
        std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::SearchMatch>>*) override;
    void canSetScriptSource(ErrorString*, bool* result) override { *result = true; }
    void setScriptSource(ErrorString*,
        const String16& inScriptId,
        const String16& inScriptSource,
        const Maybe<bool>& inPreview,
        Maybe<inspector::protocol::Array<inspector::protocol::Debugger::CallFrame>>* optOutCallFrames,
        Maybe<bool>* optOutStackChanged,
        Maybe<inspector::protocol::Runtime::StackTrace>* optOutAsyncStackTrace,
        Maybe<inspector::protocol::Debugger::SetScriptSourceError>* optOutCompileError) override;
    void restartFrame(ErrorString*,
        const String16& callFrameId,
        std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::CallFrame>>* newCallFrames,
        Maybe<inspector::protocol::Runtime::StackTrace>* asyncStackTrace) override;
    void getScriptSource(ErrorString*, const String16& scriptId, String16* scriptSource) override;
    void getFunctionDetails(ErrorString*,
        const String16& functionId,
        std::unique_ptr<inspector::protocol::Debugger::FunctionDetails>*) override;
    void getGeneratorObjectDetails(ErrorString*,
        const String16& objectId,
        std::unique_ptr<inspector::protocol::Debugger::GeneratorObjectDetails>*) override;
    void getCollectionEntries(ErrorString*,
        const String16& objectId,
        std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::CollectionEntry>>*) override;
    void pause(ErrorString*) override;
    void resume(ErrorString*) override;
    void stepOver(ErrorString*) override;
    void stepInto(ErrorString*) override;
    void stepOut(ErrorString*) override;
    void setPauseOnExceptions(ErrorString*, const String16& pauseState) override;
    void evaluateOnCallFrame(ErrorString*,
        const String16& callFrameId,
        const String16& expression,
        const Maybe<String16>& objectGroup,
        const Maybe<bool>& includeCommandLineAPI,
        const Maybe<bool>& doNotPauseOnExceptionsAndMuteConsole,
        const Maybe<bool>& returnByValue,
        const Maybe<bool>& generatePreview,
        std::unique_ptr<inspector::protocol::Runtime::RemoteObject>* result,
        Maybe<bool>* wasThrown,
        Maybe<inspector::protocol::Runtime::ExceptionDetails>*) override;
    void setVariableValue(ErrorString*,
        int scopeNumber,
        const String16& variableName,
        std::unique_ptr<inspector::protocol::Runtime::CallArgument> newValue,
        const String16& callFrame) override;
    void setAsyncCallStackDepth(ErrorString*, int depth) override;
    void setBlackboxPatterns(ErrorString*,
        std::unique_ptr<inspector::protocol::Array<String16>> patterns) override;
    void setBlackboxedRanges(ErrorString*,
        const String16& scriptId,
        std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::ScriptPosition>> positions) override;

    bool enabled();
    V8DebuggerImpl& debugger() { return *m_debugger; }

    void setBreakpointAt(const String16& scriptId, int lineNumber, int columnNumber, BreakpointSource, const String16& condition = String16());
    void removeBreakpointAt(const String16& scriptId, int lineNumber, int columnNumber, BreakpointSource);
    void schedulePauseOnNextStatement(const String16& breakReason, std::unique_ptr<inspector::protocol::DictionaryValue> data);
    void cancelPauseOnNextStatement();
    void breakProgram(const String16& breakReason, std::unique_ptr<inspector::protocol::DictionaryValue> data);
    void breakProgramOnException(const String16& breakReason, std::unique_ptr<inspector::protocol::DictionaryValue> data);

    // Async call stacks implementation.
    void asyncTaskScheduled(const String16& taskName, void* task, bool recurring);
    void asyncTaskCanceled(void* task);
    void asyncTaskStarted(void* task);
    void asyncTaskFinished(void* task);
    void allAsyncTasksCanceled();

    void reset();

    // Interface for V8DebuggerImpl
    SkipPauseRequest didPause(v8::Local<v8::Context>, v8::Local<v8::Value> exception, const inspector::protocol::Vector<String16>& hitBreakpoints, bool isPromiseRejection);
    void didContinue();
    void didParseSource(const V8DebuggerParsedScript&);
    bool v8AsyncTaskEventsEnabled() const;
    void didReceiveV8AsyncTaskEvent(v8::Local<v8::Context>, const String16& eventType, const String16& eventName, int id);
    void willExecuteScript(int scriptId);
    void didExecuteScript();

    v8::Isolate* isolate() { return m_isolate; }
    int maxAsyncCallChainDepth() { return m_maxAsyncCallStackDepth; }
    V8StackTraceImpl* currentAsyncCallChain();

private:
    bool checkEnabled(ErrorString*);
    void enable();

    SkipPauseRequest shouldSkipExceptionPause(JavaScriptCallFrame* topCallFrame);
    SkipPauseRequest shouldSkipStepPause(JavaScriptCallFrame* topCallFrame);

    void schedulePauseOnNextStatementIfSteppingInto();

    std::unique_ptr<inspector::protocol::Array<inspector::protocol::Debugger::CallFrame>> currentCallFrames(ErrorString*);
    std::unique_ptr<inspector::protocol::Runtime::StackTrace> currentAsyncStackTrace();

    void changeJavaScriptRecursionLevel(int step);

    void setPauseOnExceptionsImpl(ErrorString*, int);

    std::unique_ptr<inspector::protocol::Debugger::Location> resolveBreakpoint(const String16& breakpointId, const String16& scriptId, const ScriptBreakpoint&, BreakpointSource);
    void removeBreakpoint(const String16& breakpointId);
    bool assertPaused(ErrorString*);
    void clearBreakDetails();

    bool isCurrentCallStackEmptyOrBlackboxed();
    bool isTopPausedCallFrameBlackboxed();
    bool isCallFrameWithUnknownScriptOrBlackboxed(JavaScriptCallFrame*);

    void internalSetAsyncCallStackDepth(int);
    void increaseCachedSkipStackGeneration();

    bool setBlackboxPattern(ErrorString*, const String16& pattern);

    using ScriptsMap = inspector::protocol::HashMap<String16, V8DebuggerScript>;
    using BreakpointIdToDebuggerBreakpointIdsMap = inspector::protocol::HashMap<String16, inspector::protocol::Vector<String16>>;
    using DebugServerBreakpointToBreakpointIdAndSourceMap = inspector::protocol::HashMap<String16, std::pair<String16, BreakpointSource>>;
    using MuteBreakpoins = inspector::protocol::HashMap<String16, std::pair<String16, int>>;

    enum DebuggerStep {
        NoStep = 0,
        StepInto,
        StepOver,
        StepOut
    };

    V8DebuggerImpl* m_debugger;
    V8InspectorSessionImpl* m_session;
    bool m_enabled;
    inspector::protocol::DictionaryValue* m_state;
    inspector::protocol::Debugger::Frontend m_frontend;
    v8::Isolate* m_isolate;
    v8::Global<v8::Context> m_pausedContext;
    JavaScriptCallFrames m_pausedCallFrames;
    ScriptsMap m_scripts;
    BreakpointIdToDebuggerBreakpointIdsMap m_breakpointIdToDebuggerBreakpointIds;
    DebugServerBreakpointToBreakpointIdAndSourceMap m_serverBreakpoints;
    String16 m_continueToLocationBreakpointId;
    String16 m_breakReason;
    std::unique_ptr<inspector::protocol::DictionaryValue> m_breakAuxData;
    DebuggerStep m_scheduledDebuggerStep;
    bool m_skipNextDebuggerStepOut;
    bool m_javaScriptPauseScheduled;
    bool m_steppingFromFramework;
    bool m_pausingOnNativeEvent;

    int m_skippedStepFrameCount;
    int m_recursionLevelForStepOut;
    int m_recursionLevelForStepFrame;
    bool m_skipAllPauses;

    using AsyncTaskToStackTrace = inspector::protocol::HashMap<void*, std::unique_ptr<V8StackTraceImpl>>;
    AsyncTaskToStackTrace m_asyncTaskStacks;
    inspector::protocol::HashSet<void*> m_recurringTasks;
    int m_maxAsyncCallStackDepth;
    inspector::protocol::Vector<void*> m_currentTasks;
    inspector::protocol::Vector<std::unique_ptr<V8StackTraceImpl>> m_currentStacks;
    std::unique_ptr<V8Regex> m_blackboxPattern;
    inspector::protocol::HashMap<String16, inspector::protocol::Vector<std::pair<int, int>>> m_blackboxedPositions;
};

} // namespace blink


#endif // V8DebuggerAgentImpl_h
