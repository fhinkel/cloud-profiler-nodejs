/**
 * Copyright 2015 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "v8-profiler.h"
#include "nan.h"

using namespace v8;

CpuProfiler* profiler_ = nullptr;
Isolate* isolate_;

CpuProfiler* profiler(Isolate* isolate) {
  if (!profiler_) {
    profiler_ = CpuProfiler::New(isolate);
    isolate_ = isolate;
  }

  if (isolate != isolate_) {
    printf("Different isos\n\n\n\n");
  }
  return profiler_;
}


Local<Value> TranslateTimeProfileNode(const CpuProfileNode* node) {
  Local<Object> js_node = Nan::New<Object>();
  js_node->Set(Nan::New<String>("name").ToLocalChecked(),
    node->GetFunctionName());
  js_node->Set(Nan::New<String>("scriptName").ToLocalChecked(),
    node->GetScriptResourceName());
  js_node->Set(Nan::New<String>("scriptId").ToLocalChecked(),
    Nan::New<Integer>(node->GetScriptId()));
  js_node->Set(Nan::New<String>("lineNumber").ToLocalChecked(),
    Nan::New<Integer>(node->GetLineNumber()));
  js_node->Set(Nan::New<String>("columnNumber").ToLocalChecked(),
    Nan::New<Integer>(node->GetColumnNumber()));

  // printf("%s in %s, total ticks: %d\n", node->GetFunctionNameStr(), node->GetScriptResourceNameStr(), node->GetHitCount());

  js_node->Set(Nan::New<String>("hitCount").ToLocalChecked(),
    Nan::New<Integer>(node->GetHitCount()));

  unsigned int hitLineCount = node->GetHitLineCount();
  std::vector<CpuProfileNode::LineTick> entries(hitLineCount);

  int32_t count = node->GetChildrenCount();

  Local<Array> children;
  unsigned int index = 0;
  if (node->GetLineTicks(&entries[0], hitLineCount)) {
    children = Nan::New<Array>(count + entries.size());
    js_node->Set(Nan::New<String>("hitCount").ToLocalChecked(),
      Nan::New<Integer>(0));
    for (const CpuProfileNode::LineTick entry: entries) {
      Local<Object> js_node_hit = Nan::New<Object>();
      js_node_hit->Set(Nan::New<String>("name").ToLocalChecked(),
        node->GetFunctionName());
      js_node_hit->Set(Nan::New<String>("scriptName").ToLocalChecked(),
        node->GetScriptResourceName());
      js_node_hit->Set(Nan::New<String>("scriptId").ToLocalChecked(),
        Nan::New<Integer>(node->GetScriptId()));
      js_node_hit->Set(Nan::New<String>("lineNumber").ToLocalChecked(),
        Nan::New<Integer>(entry.line));
      js_node_hit->Set(Nan::New<String>("columnNumber").ToLocalChecked(),
        Nan::New<Integer>(0));
      js_node_hit->Set(Nan::New<String>("hitCount").ToLocalChecked(),
        Nan::New<Integer>(entry.hit_count));
      js_node_hit->Set(Nan::New<String>("children").ToLocalChecked(),
        Nan::New<Array>(0));

      children->Set(index++, js_node_hit);

      // printf("  line %d, ticks %d in %s\n", entry.line, entry.hit_count, node->GetScriptResourceNameStr());
    }
  } else {
    children = Nan::New<Array>(count);
  }


  for (int32_t i = 0; i < count; i++) {
    children->Set(i + index, TranslateTimeProfileNode(node->GetChild(i)));
  }
  js_node->Set(Nan::New<String>("children").ToLocalChecked(),
    children);
  return js_node;
}

Local<Value> TranslateTimeProfile(const CpuProfile* profile) {
  Local<Object> js_profile = Nan::New<Object>();
  js_profile->Set(Nan::New<String>("title").ToLocalChecked(),
    profile->GetTitle());
  js_profile->Set(Nan::New<String>("topDownRoot").ToLocalChecked(),
    TranslateTimeProfileNode(profile->GetTopDownRoot()));
  js_profile->Set(Nan::New<String>("samplesCount").ToLocalChecked(),
    Nan::New<Integer>(profile->GetSamplesCount()));
  js_profile->Set(Nan::New<String>("startTime").ToLocalChecked(),
    Nan::New<Number>(profile->GetStartTime()));
  js_profile->Set(Nan::New<String>("endTime").ToLocalChecked(),
    Nan::New<Number>(profile->GetEndTime()));
  return js_profile;
}

NAN_METHOD(StartProfiling) {
  Local<String> name = info[0].As<String>();
  bool record_samples = info[1].As<Boolean>()->BooleanValue();
  profiler(info.GetIsolate())->StartProfiling(name, record_samples);
}

NAN_METHOD(StopProfiling) {
  Local<String> name = info[0].As<String>();
  CpuProfile* profile =
    profiler(info.GetIsolate())->StopProfiling(name);
  Local<Value> translated_profile = TranslateTimeProfile(profile);
  profile->Delete();
  info.GetReturnValue().Set(translated_profile);
}

NAN_METHOD(SetSamplingInterval) {
  int us = info[0].As<Integer>()->IntegerValue();
  profiler(info.GetIsolate())->SetSamplingInterval(us);
}

NAN_METHOD(SetIdle) {
  bool is_idle = info[0].As<Boolean>()->BooleanValue();
  profiler(info.GetIsolate())->SetIdle(is_idle);
}

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("startProfiling").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(StartProfiling)).ToLocalChecked());
  Nan::Set(target, Nan::New("stopProfiling").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(StopProfiling)).ToLocalChecked());
  Nan::Set(target, Nan::New("setSamplingInterval").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(SetSamplingInterval)).ToLocalChecked());
  Nan::Set(target, Nan::New("setIdle").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(SetIdle)).ToLocalChecked());
}

NODE_MODULE(time_profiler, InitAll);
