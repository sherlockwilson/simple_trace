#pragma once

#include <zipkin/opentracing.h>

namespace top {
namespace xtrace {

class XTraceSDK {
public:
    static void
    CreateTraceContext(const std::string& service_name);

    static std::unique_ptr<opentracing::Span> 
    StartRootSpan(const std::string& span_name);

    static std::unique_ptr<opentracing::Span> 
    StartChildSpan(
        std::unique_ptr<opentracing::Span>& parent_span,
        const std::string& span_name);

    static std::unique_ptr<opentracing::Span> 
    StartChildSpan(
        std::unique_ptr<opentracing::SpanContext>& parent_span_text,
        const std::string& span_name);
        
    static std::unique_ptr<opentracing::Span> 
    StartChildSpan(
        opentracing::expected<std::unique_ptr<opentracing::SpanContext>>& span_text_maybe,
        const std::string& span_name);

    static std::unique_ptr<opentracing::Span> 
    StartFollowsSpan(
        std::unique_ptr<opentracing::Span>& parent_span,
        const std::string& span_name);

    static std::unique_ptr<opentracing::Span> 
    StartFollowsSpan(
        std::unique_ptr<opentracing::SpanContext>& parent_span_text,
        const std::string& span_name);

    static void
    FinishSpan(std::unique_ptr<opentracing::Span>& span);

    static void
    TagSpan(
        std::unique_ptr<opentracing::Span>& span,
        opentracing::string_view key,
        const opentracing::Value& value);

    static std::string
    PackSpanContext(const std::unique_ptr<opentracing::Span>& span);

    static opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
    UnpackSpanContext(const std::string& stream);

    static void
    DestroyTraceContext();
};

}
}