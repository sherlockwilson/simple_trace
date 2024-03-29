#include <zipkin/zipkin_core_types.h>

#include "zipkin_core_constants.h"
#include "zipkin_json_field_names.h"
#include <zipkin/span_context.h>
#include <zipkin/utility.h>

#include <zipkin/rapidjson/stringbuffer.h>
#include <zipkin/rapidjson/writer.h>

namespace zipkin {
const std::string Endpoint::toJson() {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  writer.StartObject();
  if (!address_.valid()) {
    writer.Key(ZipkinJsonFieldNames::get().ENDPOINT_IPV4.c_str());
    writer.String("");
    writer.Key(ZipkinJsonFieldNames::get().ENDPOINT_PORT.c_str());
    writer.Uint(0);
  } else {
    if (address_.version() == IpVersion::v4) {
      // IPv4
      writer.Key(ZipkinJsonFieldNames::get().ENDPOINT_IPV4.c_str());
    } else {
      // IPv6
      writer.Key(ZipkinJsonFieldNames::get().ENDPOINT_IPV6.c_str());
    }
    writer.String(address_.addressAsString().c_str());
    writer.Key(ZipkinJsonFieldNames::get().ENDPOINT_PORT.c_str());
    writer.Uint(address_.port());
  }
  writer.Key(ZipkinJsonFieldNames::get().ENDPOINT_SERVICE_NAME.c_str());
  writer.String(service_name_.c_str());
  writer.EndObject();
  std::string json_string = s.GetString();

  return json_string;
}

Annotation::Annotation(const Annotation &ann) {
  timestamp_ = ann.timestamp();
  value_ = ann.value();
  if (ann.isSetEndpoint()) {
    endpoint_ = ann.endpoint();
  }
}

Annotation &Annotation::operator=(const Annotation &ann) {
  timestamp_ = ann.timestamp();
  value_ = ann.value();
  if (ann.isSetEndpoint()) {
    endpoint_ = ann.endpoint();
  }

  return *this;
}

void Annotation::changeEndpointServiceName(const std::string &service_name) {
  if (endpoint_.valid()) {
    endpoint_.value().setServiceName(service_name);
  }
}

const std::string Annotation::toJson() {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  writer.StartObject();
  writer.Key(ZipkinJsonFieldNames::get().ANNOTATION_TIMESTAMP.c_str());
  writer.Uint64(timestamp_);
  writer.Key(ZipkinJsonFieldNames::get().ANNOTATION_VALUE.c_str());
  writer.String(value_.c_str());
  writer.EndObject();

  std::string json_string = s.GetString();

  if (endpoint_.valid()) {
    JsonUtil::mergeJsons(
        json_string, static_cast<Endpoint>(endpoint_.value()).toJson(),
        ZipkinJsonFieldNames::get().ANNOTATION_ENDPOINT.c_str());
  }

  return json_string;
}

const std::string BinaryAnnotation::toJson() {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  writer.StartObject();
  writer.Key(ZipkinJsonFieldNames::get().BINARY_ANNOTATION_KEY.c_str());
  writer.String(key_.c_str());
  writer.Key(ZipkinJsonFieldNames::get().BINARY_ANNOTATION_VALUE.c_str());
  switch (annotation_type_) {
  case STRING:
    writer.String(value_string_.c_str());
    break;
  case BOOL:
    writer.Bool(value_bool_);
    break;
  case INT64:
    writer.Int64(value_int64_);
    break;
  case DOUBLE:
    writer.Double(value_double_);
    break;
  }

  if (annotation_type_ == INT64) {
    writer.Key(ZipkinJsonFieldNames::get().BINARY_ANNOTATION_TYPE.c_str());
    writer.String(
        ZipkinJsonFieldNames::get().BINARY_ANNOTATION_TYPE_INT64.c_str());
  }

  writer.EndObject();

  std::string json_string = s.GetString();

  if (endpoint_.valid()) {
    JsonUtil::mergeJsons(
        json_string, static_cast<Endpoint>(endpoint_.value()).toJson(),
        ZipkinJsonFieldNames::get().BINARY_ANNOTATION_ENDPOINT.c_str());
  }

  return json_string;
}

const std::string Span::EMPTY_HEX_STRING_ = "0000000000000000";

void Span::setServiceName(const std::string &service_name) {
  for (auto it = annotations_.begin(); it != annotations_.end(); it++) {
    it->changeEndpointServiceName(service_name);
  }
}

const std::string Span::toJson() {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  writer.StartObject();
  writer.Key(ZipkinJsonFieldNames::get().SPAN_TRACE_ID.c_str());
  writer.String(Hex::traceIdToHex(trace_id_).c_str());
  writer.Key(ZipkinJsonFieldNames::get().SPAN_NAME.c_str());
  writer.String(name_.c_str());
  writer.Key(ZipkinJsonFieldNames::get().SPAN_ID.c_str());
  writer.String(Hex::uint64ToHex(id_).c_str());

  if (parent_id_.valid() && !parent_id_.value().empty()) {
    writer.Key(ZipkinJsonFieldNames::get().SPAN_PARENT_ID.c_str());
    writer.String(Hex::traceIdToHex(parent_id_.value()).c_str());
  }

  if (timestamp_.valid()) {
    writer.Key(ZipkinJsonFieldNames::get().SPAN_TIMESTAMP.c_str());
    writer.Int64(timestamp_.value());
  }

  if (duration_.valid()) {
    writer.Key(ZipkinJsonFieldNames::get().SPAN_DURATION.c_str());
    writer.Int64(duration_.value());
  }

  writer.EndObject();

  std::string json_string = s.GetString();

  std::vector<std::string> annotation_json_vector;

  for (auto it = annotations_.begin(); it != annotations_.end(); it++) {
    annotation_json_vector.push_back(it->toJson());
  }
  JsonUtil::addArrayToJson(
      json_string, annotation_json_vector,
      ZipkinJsonFieldNames::get().SPAN_ANNOTATIONS.c_str());

  std::vector<std::string> binary_annotation_json_vector;
  for (auto it = binary_annotations_.begin(); it != binary_annotations_.end();
       it++) {
    binary_annotation_json_vector.push_back(it->toJson());
  }
  JsonUtil::addArrayToJson(
      json_string, binary_annotation_json_vector,
      ZipkinJsonFieldNames::get().SPAN_BINARY_ANNOTATIONS.c_str());

  return json_string;
}

void Span::finish() {
  if (auto t = tracer()) {
    if (this->isSampled()) {
      t->reportSpan(std::move(*this));
    }
  }
}

void Span::setTag(const std::string &name, const std::string &value) {
  if (name.size() > 0 && value.size() > 0) {
    addBinaryAnnotation(BinaryAnnotation(name, value));
  }
}
} // namespace zipkin
