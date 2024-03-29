// text_map_carrier.h, copy from example folder
#pragma once

#include <opentracing/propagation.h>
#include <string>
#include <unordered_map>

using opentracing::TextMapReader;
using opentracing::TextMapWriter;
using opentracing::expected;
using opentracing::string_view;

// class textMapCarrier
class TextMapCarrier : public TextMapReader, public TextMapWriter {
public:
  TextMapCarrier(std::unordered_map<std::string, std::string> &text_map)
      : text_map_(text_map) {}

  expected<void> Set(string_view key, string_view value) const override {
    text_map_[key] = value;
    return {};
  }

  expected<void> ForeachKey(
      std::function<expected<void>(string_view key, string_view value)> f)
      const override {
    for (const auto &key_value : text_map_) {
      auto result = f(key_value.first, key_value.second);
      if (!result)
        return result;
    }
    return {};
  }

  std::unordered_map<std::string, std::string> &text_map_;
};

// span context reader/writer helper

std::string read_span_context(const std::unordered_map<std::string, std::string>& m)
{
    std::string output = "";

    for (auto it = m.cbegin(); it != m.cend(); it++)
    {
        output += (it->first) + ":" + it->second + ",";
    }

    return output.substr(0, output.size() - 1);
}


void write_span_context(std::unordered_map<std::string, std::string> &m, char *context)
{
   char * keypair = strtok(context, ",");
   while(keypair != NULL)
   {
       std::string s(keypair);
       std::string::size_type found = s.find_first_of(':');
       m.insert(std::pair<std::string, std::string>(s.substr(0,found),s.substr(found+1)));
       keypair =strtok(NULL, ",");
   }
}