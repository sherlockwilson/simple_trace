#pragma once

#include "xtrace_sdk/xtrace_sdk.h"

#define XTRACE_CREATE_CONTEXT(service_name) \
({ \
    top::xtrace::XTraceSDK::CreateTraceContext(service_name); \
})

#define XTRACE_START_ROOT_SPAN(span_name) \
({ \
    top::xtrace::XTraceSDK::StartRootSpan(span_name); \
})

#define XTRACE_START_CHILD_SPAN(parent_span, span_name) \
({ \
    top::xtrace::XTraceSDK::StartChildSpan(parent_span, span_name); \
})

#define XTRACE_START_FOLLOW_SPAN(parent_span, span_name) \
({ \
    top::xtrace::XTraceSDK::StartFollowsSpan(parent_span, span_name); \
})

//complext object,should add "()",such as (Values{123, Dictionary{{"abc", 123}, {"xyz", 4.0}}})
#define XTRACE_TAG_SPAN(span, key, value) \
({ \
    top::xtrace::XTraceSDK::TagSpan(span, key, value); \
})

#define XTRACE_FINISH_SPAN(span) \
({ \
    top::xtrace::XTraceSDK::FinishSpan(span); \
})

#define XTRACE_PACK_SPAN(span) \
({ \
    top::xtrace::XTraceSDK::PackSpanContext(span); \
})

#define XTRACE_UNPACK_SPAN_CONTEXT(span_context_stream) \
({ \
    top::xtrace::XTraceSDK::UnpackSpanContext(span_context_stream); \
})

#define XTRACE_DESTROY_CONTEXT() \
({ \
    top::xtrace::XTraceSDK::DestroyTraceContext(); \
})