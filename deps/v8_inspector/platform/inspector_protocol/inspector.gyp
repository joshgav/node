# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'inspector_output_dir': '<(SHARED_INTERMEDIATE_DIR)/platform/inspector_protocol',
    'inspector_protocol_output_dir': '<(inspector_output_dir)/protocol'
  },
  'targets': [
    {
      # GN version: //third_party/WebKit/Source/platform:inspector_protocol_sources
      'target_name': 'protocol_sources',
      'type': 'none',
      'dependencies': ['protocol_version'],
      'variables': {
        'conditions': [
          ['debug_devtools=="node"', {
              # Node build
              'jinja_module_files': [
                '../../deps/jinja2/jinja2/__init__.py',
                '../../deps/markupsafe/markupsafe/__init__.py',  # jinja2 dep
              ],
            }, {
              'jinja_module_files': [
                '<(DEPTH)/third_party/jinja2/__init__.py',
                '<(DEPTH)/third_party/markupsafe/__init__.py',  # jinja2 dep
              ],
            }
          ],
        ],
      },
      'actions': [
        {
          'action_name': 'generateInspectorProtocolBackendSources',
          'inputs': [
            '<@(jinja_module_files)',
            # The python script in action below.
            './CodeGenerator.py',
            # Source code templates.
            './TypeBuilder_h.template',
            './TypeBuilder_cpp.template',
            # Protocol definitions
            './js_protocol.json',
          ],
          'outputs': [
            '<(inspector_protocol_output_dir)/Debugger.cpp',
            '<(inspector_protocol_output_dir)/Debugger.h',
            '<(inspector_protocol_output_dir)/HeapProfiler.cpp',
            '<(inspector_protocol_output_dir)/HeapProfiler.h',
            '<(inspector_protocol_output_dir)/Profiler.cpp',
            '<(inspector_protocol_output_dir)/Profiler.h',
            '<(inspector_protocol_output_dir)/Runtime.cpp',
            '<(inspector_protocol_output_dir)/Runtime.h',
          ],
          'action': [
            'python',
            './CodeGenerator.py',
            '--protocol', 'js_protocol.json',
            '--string_type', 'String16',
            '--export_macro', 'PLATFORM_EXPORT',
            '--output_dir', '<(inspector_protocol_output_dir)',
            '--output_package', 'platform/inspector_protocol/protocol',
          ],
          'message': 'Generating protocol backend sources from json definitions.',
        },
      ]
    },
    {
      # GN version: //third_party/WebKit/Source/core/inspector:protocol_version
      'target_name': 'protocol_version',
      'type': 'none',
      'actions': [
         {
          'action_name': 'generateInspectorProtocolVersion',
          'inputs': [
            './generate-inspector-protocol-version',
            './js_protocol.json',
          ],
          'outputs': [
            '<(inspector_output_dir)/protocol.json',
          ],
          'action': [
            'python',
            './generate-inspector-protocol-version',
            '--o',
            '<@(_outputs)',
            'js_protocol.json',
          ],
          'message': 'Validate v8_inspector protocol for backwards compatibility and generate version file',
        },
      ]
    },
    {
      'target_name': 'inspector',
      'type': '<(component)',
      'dependencies': [
        ':protocol_sources',
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
        '<(inspector_protocol_output_dir)/Debugger.cpp',
        '<(inspector_protocol_output_dir)/Debugger.h',
        '<(inspector_protocol_output_dir)/HeapProfiler.cpp',
        '<(inspector_protocol_output_dir)/HeapProfiler.h',
        '<(inspector_protocol_output_dir)/Profiler.cpp',
        '<(inspector_protocol_output_dir)/Profiler.h',
        '<(inspector_protocol_output_dir)/Runtime.cpp',
        '<(inspector_protocol_output_dir)/Runtime.h',

        'Allocator.h',
        'Array.h',
        'Collections.h',
        'CollectionsSTL.h',
        'DispatcherBase.cpp',
        'DispatcherBase.h',
        'ErrorSupport.cpp',
        'ErrorSupport.h',
        'Inspector.h',
        'Inspector.cpp',
        'Maybe.h',
        'Parser.cpp',
        'Parser.h',
        'FrontendChannel.h',
        'String16.h',
        'String16STL.cpp',
        'String16STL.h',
        'Values.cpp',
        'Values.h',
        'ValueConversions.cpp',
        'ValueConversions.h',

      ],
    },
  ],  # targets
}
