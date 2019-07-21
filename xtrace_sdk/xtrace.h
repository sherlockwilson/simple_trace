#pragma once

#include "xtrace_sdk/xtrace_sdk.h"

/**
 * 创建xtrace上下文
 * 根据服务名，zipkin服务IP，zipkin服务端口初始化xtrace环境
 * @param service_name 服务名称
 * @param service_ip zipkin的IP
 * @return None
 */
#define XTRACE_CREATE_CONTEXT(service_name, service_ip, service_port) \
({ \
    top::xtrace::XTraceSDK::CreateTraceContext(service_name, service_ip, service_port); \
})

/**
 * 开始Root Span
 * 根据Span名称开始Root Span
 * @param span_name span名称
 * @return 刚刚创建的Span对象
 */
#define XTRACE_START_ROOT_SPAN(span_name) \
({ \
    top::xtrace::XTraceSDK::StartRootSpan(span_name); \
})

/**
 * 开始子Span
 * 根据父Span，Span名称创建子Span
 * @param parent_span父span句柄
 * @param span_name span名称
 * @return 刚刚创建的Span对象
 */
#define XTRACE_START_CHILD_SPAN(parent_span, span_name) \
({ \
    top::xtrace::XTraceSDK::StartChildSpan(parent_span, span_name); \
})

/**
 * 开始Follow Span
 * 根据Span，Span名称创建跟随的Span
 * @param span 跟随的span对象
 * @param span_name span名称
 * @return 刚刚创建的Span对象
 */
#define XTRACE_START_FOLLOW_SPAN(span, span_name) \
({ \
    top::xtrace::XTraceSDK::StartFollowsSpan(span, span_name); \
})

/**
 * 给Span加上标签
 * 按Key值和Value值给Span加上标签
 * @param span span对象
 * @param key key值
 * @param value value值
 * @brief 复杂对象需要加上"()",例如(Values{123, Dictionary{{"abc", 123}, {"xyz", 4.0}}})
 * @return None
 */
#define XTRACE_TAG_SPAN(span, key, value) \
({ \
    top::xtrace::XTraceSDK::TagSpan(span, key, value); \
})

/**
 * 结束Span
 * 结束指定的Span对象
 * @param span 指定的span对象
 * @return None
 */
#define XTRACE_FINISH_SPAN(span) \
({ \
    top::xtrace::XTraceSDK::FinishSpan(span); \
})

/**
 * 打包Span上下文
 * 以传入的span作为结束节点，打包包含它以及它之前的所有span成上下文
 * @param span 根据span对象打包上下文
 * @return span上下文二进制流
 */
#define XTRACE_PACK_SPAN(span) \
({ \
    top::xtrace::XTraceSDK::PackSpanContext(span); \
})

/**
 * 解开span上下文二进制流
 * 传入的span上下文，解开上下文并返回span对象
 * @param span_context_stream span上下文流
 * @return span上下文
 */
#define XTRACE_UNPACK_SPAN_CONTEXT(span_context_stream) \
({ \
    top::xtrace::XTraceSDK::UnpackSpanContext(span_context_stream); \
})

/**
 * 销毁xtrace上下文
 * 销毁xtrace上下文
 * @return None
 */
#define XTRACE_DESTROY_CONTEXT() \
({ \
    top::xtrace::XTraceSDK::DestroyTraceContext(); \
})