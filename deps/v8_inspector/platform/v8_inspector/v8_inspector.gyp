# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'platform_v8_inspector_output_dir': '<(SHARED_INTERMEDIATE_DIR)/platform/v8_inspector',
  },
  'targets': [
    {
      # GN version: //third_party/WebKit/Source/platform:inspector_injected_script
      'target_name': 'inspector_injected_script',
      'type': 'none',
      'actions': [
        {
          'action_name': 'ConvertFileToHeaderWithCharacterArray',
          'inputs': [
            'build/xxd.py',
            'InjectedScriptSource.js',
          ],
          'outputs': [ '<(platform_v8_inspector_output_dir)/InjectedScriptSource.h', ],
          'action': [
            'python', 'build/xxd.py', 'InjectedScriptSource_js', 'InjectedScriptSource.js', '<@(_outputs)'
          ],
        },
      ],
      # Since this target generates header files, it needs to be a hard dependency.
      'hard_dependency': 1,
    },
    {
      # GN version: //third_party/WebKit/Source/platform:inspector_debugger_script
      'target_name': 'inspector_debugger_script',
      'type': 'none',
      'actions': [
        {
          'action_name': 'ConvertFileToHeaderWithCharacterArray',
          'inputs': [
            'build/xxd.py',
            'DebuggerScript.js',
          ],
          'outputs': [ '<(platform_v8_inspector_output_dir)/DebuggerScript.h', ],
          'action': [
            'python', 'build/xxd.py', 'DebuggerScript_js', 'DebuggerScript.js', '<@(_outputs)'
          ],
        },
      ],
      # Since this target generates header files, it needs to be a hard dependency.
      'hard_dependency': 1,
    },
    {
      'target_name': 'v8_inspector_stl',
      'type': '<(component)',
      'dependencies': [
        '../inspector_protocol/inspector.gyp:inspector',
        ':inspector_injected_script',
        ':inspector_debugger_script',
      ],
      'defines': [
        'V8_INSPECTOR_USE_STL=1'
      ],
      'include_dirs': [
        '../..',
        '../../../v8/include',
        '../../../v8',
        '<(SHARED_INTERMEDIATE_DIR)',
      ],
      'sources': [
        'Atomics.h',
        'IgnoreExceptionsScope.h',
        'InjectedScript.cpp',
        'InjectedScript.h',
        'InjectedScriptNative.cpp',
        'InjectedScriptNative.h',
        'InspectedContext.cpp',
        'InspectedContext.h',
        'JavaScriptCallFrame.cpp',
        'JavaScriptCallFrame.h',
        'MuteConsoleScope.h',
        'ScriptBreakpoint.h',
        'RemoteObjectId.cpp',
        'RemoteObjectId.h',
        'V8Console.cpp',
        'V8Console.h',
        'V8DebuggerAgentImpl.cpp',
        'V8DebuggerAgentImpl.h',
        'V8DebuggerImpl.cpp',
        'V8DebuggerImpl.h',
        'V8DebuggerScript.cpp',
        'V8DebuggerScript.h',
        'V8FunctionCall.cpp',
        'V8FunctionCall.h',
        'V8HeapProfilerAgentImpl.cpp',
        'V8HeapProfilerAgentImpl.h',
        'V8InjectedScriptHost.cpp',
        'V8InjectedScriptHost.h',
        'V8InspectorSessionImpl.cpp',
        'V8InspectorSessionImpl.h',
        'V8ProfilerAgentImpl.cpp',
        'V8ProfilerAgentImpl.h',
        'V8Regex.cpp',
        'V8Regex.h',
        'V8RuntimeAgentImpl.cpp',
        'V8RuntimeAgentImpl.h',
        'V8StackTraceImpl.cpp',
        'V8StackTraceImpl.h',
        'V8StringUtil.cpp',
        'V8StringUtil.h',
        'public/V8EventListenerInfo.h',
        'public/V8ContentSearchUtil.h',
        'public/V8ContextInfo.h',
        'public/V8Debugger.h',
        'public/V8DebuggerClient.h',
        'public/V8HeapProfilerAgent.h',
        'public/V8Inspector.cpp',
        'public/V8Inspector.h',
        'public/V8InspectorSession.h',
        'public/V8StackTrace.h',
        'public/V8ToProtocolValue.h',

        '<(platform_v8_inspector_output_dir)/DebuggerScript.h',
        '<(platform_v8_inspector_output_dir)/InjectedScriptSource.h',
      ],
    },
  ],  # targets
}
