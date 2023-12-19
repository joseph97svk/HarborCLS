//
// Created by josephvalverde on 12/16/23.
//

#ifndef HARBOR_CLS_HTTPMAPPINGS_HPP
#define HARBOR_CLS_HTTPMAPPINGS_HPP

#include <unordered_map>

namespace HarborCLS {

  struct HttpMappings {
    static constexpr const std::string separator = "\r\n\r\n";

    enum class Method {
      DEFAULT = 0,
      GET,
      POST,
      PUT,
      DELETE,
      HEAD,
      OPTIONS,
      TRACE,
      CONNECT,
      PATCH
    };

    enum class HeaderField {
      DEFAULT = 0,
      host,
      userAgent,
      contentType,
      languagesToAccept,
      encodingsToAccept,
      KeepConnectionAlive,
      upgradeConnectionSafety,
      expectedResponseType,
      requestMode,
      originRelation
    };

    const std::unordered_map<std::string, HttpMappings::Method> methods;

    const std::unordered_map<std::string, HttpMappings::HeaderField> headerFields;

    const std::unordered_map<std::string, bool> contentTypeRequiresVectorizedBody;

    static HttpMappings &getInstance() {
      static HttpMappings instance;
      return instance;
    }

  protected:
    HttpMappings() :
        methods{
            {"GET",     HttpMappings::Method::GET},
            {"HEAD",    HttpMappings::Method::HEAD},
            {"POST",    HttpMappings::Method::POST},
            {"PUT",     HttpMappings::Method::PUT},
            {"DELETE",  HttpMappings::Method::DELETE},
            {"CONNECT", HttpMappings::Method::CONNECT},
            {"OPTIONS", HttpMappings::Method::OPTIONS},
            {"TRACE",   HttpMappings::Method::TRACE},
            {"PATCH",   HttpMappings::Method::PATCH}},
        headerFields{
            {"",                          HttpMappings::HeaderField::DEFAULT},
            {"Host",                      HttpMappings::HeaderField::host},
            {"User-Agent",                HttpMappings::HeaderField::userAgent},
            {"Accept",                    HttpMappings::HeaderField::contentType},
            {"Accept-Language",           HttpMappings::HeaderField::languagesToAccept},
            {"Accept-Encoding",           HttpMappings::HeaderField::encodingsToAccept},
            {"Connection",                HttpMappings::HeaderField::KeepConnectionAlive},
            {"Upgrade-Insecure-Requests", HttpMappings::HeaderField::upgradeConnectionSafety},
            {"Sec-Fetch-Dest",            HttpMappings::HeaderField::expectedResponseType},
            {"Sec-Fetch-Mode",            HttpMappings::HeaderField::requestMode},
            {"Sec-Fetch-Site",            HttpMappings::HeaderField::originRelation}},
        contentTypeRequiresVectorizedBody{
            {"text/plain",                                                                false},
            {"text/html",                                                                 false},
            {"text/css",                                                                  false},
            {"text/javascript",                                                           false},
            {"text/xml",                                                                  false},
            {"application/json",                                                          false},
            {"application/xml",                                                           false},
            {"application/x-www-form-urlencoded",                                         false},

            // Image formats
            {"image/png",                                                                 true},
            {"image/jpeg",                                                                true},
            {"image/gif",                                                                 true},
            {"image/svg+xml",                                                             true},
            {"image/bmp",                                                                 true},
            {"image/webp",                                                                true},
            {"image/tiff",                                                                true},
            {"image/x-icon",                                                              true},

            // Video and audio formats
            {"video/mp4",                                                                 true},
            {"video/quicktime",                                                           true},
            {"video/x-msvideo",                                                           true},
            {"video/x-matroska",                                                          true},
            {"video/webm",                                                                true},
            {"audio/mpeg",                                                                true},
            {"audio/ogg",                                                                 true},
            {"audio/wav",                                                                 true},
            {"audio/webm",                                                                true},
            {"audio/aac",                                                                 true},
            {"audio/x-wav",                                                               true},

            // Application-specific formats
            {"application/pdf",                                                           true},
            {"application/zip",                                                           true},
            {"application/msword",                                                        true},
            {"application/vnd.ms-excel",                                                  true},
            {"application/vnd.openxmlformats-officedocument.wordprocessingml.document",   true},
            {"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",         true},
            {"application/vnd.ms-powerpoint",                                             true},
            {"application/vnd.openxmlformats-officedocument.presentationml.presentation", true},
            {"application/java-archive",                                                  true},
            {"application/x-tar",                                                         true},
            {"application/x-rar-compressed",                                              true},
            {"application/gzip",                                                          true},
            {"application/vnd.apple.installer+xml",                                       true},
            {"application/epub+zip",                                                      true},

            // Other binary formats
            {"application/octet-stream",                                                  true},
            {"multipart/form-data",                                                       true}
        } {}
  };
}
#endif //HARBOR_CLS_HTTPMAPPINGS_HPP
