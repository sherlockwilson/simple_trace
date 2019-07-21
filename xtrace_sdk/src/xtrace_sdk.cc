#include "xtrace_sdk.h"

#include "base/text_map_carrier.h"

namespace top {
namespace xtrace {

typedef std::shared_ptr<opentracing::Tracer> TracePtr;

static TracePtr s_tracer;

void
XTraceSDK::CreateTraceContext(
        const std::string& service_name,
        const std::string& service_ip,
        const uint32_t& service_port) {
    zipkin::ZipkinOtTracerOptions options;
    options.service_name = service_name;
    options.collector_host = service_ip;
    options.collector_port = service_port;
    s_tracer = makeZipkinOtTracer(options);
}

std::unique_ptr<opentracing::Span> 
XTraceSDK::StartRootSpan(
        const std::string& span_name) {
    auto root_span_ptr = s_tracer->StartSpan(span_name.c_str());
    return std::move(root_span_ptr);
}

std::unique_ptr<opentracing::Span> 
XTraceSDK::StartChildSpan(
        std::unique_ptr<opentracing::Span>& parent_span,
        const std::string& span_name) {
    auto span_ptr = s_tracer->StartSpan(span_name.c_str(), {ChildOf(&parent_span->context())});
    return std::move(span_ptr);
}

std::unique_ptr<opentracing::Span> 
XTraceSDK::StartChildSpan(
        std::unique_ptr<opentracing::SpanContext>& parent_span_text,
        const std::string& span_name) {
    auto span_ptr = s_tracer->StartSpan(span_name.c_str(), {ChildOf(parent_span_text.get())});
    return std::move(span_ptr);
}

std::unique_ptr<opentracing::Span> 
XTraceSDK::StartChildSpan(
        opentracing::expected<std::unique_ptr<opentracing::SpanContext>>& span_text_maybe,
        const std::string& span_name) {
    auto span_ptr = s_tracer->StartSpan(span_name.c_str(), {ChildOf(span_text_maybe->get())});
    return std::move(span_ptr);
}

std::unique_ptr<opentracing::Span> 
XTraceSDK::StartFollowsSpan(
        std::unique_ptr<opentracing::Span>& parent_span,
        const std::string& span_name) {
    auto span_ptr = s_tracer->StartSpan(span_name.c_str(), {FollowsFrom(&parent_span->context())});
    return std::move(span_ptr);
}

std::unique_ptr<opentracing::Span> 
XTraceSDK::StartFollowsSpan(
        std::unique_ptr<opentracing::SpanContext>& parent_span_text,
        const std::string& span_name) {
    auto span_ptr = s_tracer->StartSpan(span_name.c_str(), {FollowsFrom(parent_span_text.get())});
    return std::move(span_ptr);
}

void
XTraceSDK::FinishSpan(std::unique_ptr<opentracing::Span>& span) {
    span->Finish();
}

void
XTraceSDK::TagSpan(
    std::unique_ptr<opentracing::Span>& span,
    opentracing::string_view key,
    const opentracing::Value& value) {
    return span->SetTag(key, value); 
}

std::string
XTraceSDK::PackSpanContext(const std::unique_ptr<opentracing::Span>& span) {
    std::unordered_map<std::string, std::string> text_map;
    TextMapCarrier carrier(text_map);
    s_tracer->Inject(span->context(), carrier);
    return read_span_context(text_map);
}

opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
XTraceSDK::UnpackSpanContext(const std::string& stream) {
    std::unordered_map<std::string, std::string> text_map;
    write_span_context(std::ref(text_map),(char*)(stream.c_str()));
    TextMapCarrier carrier(text_map);
    return s_tracer->Extract(carrier);
}

void
XTraceSDK::DestroyTraceContext() {
    if(s_tracer) {
        s_tracer->Close();
    }
    s_tracer.reset();
}

}
}