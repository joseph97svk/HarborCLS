//
// Created by josephvalverde on 12/17/23.
//

#ifndef HARBOR_CLS_HTTPRESPONSESTATUSCODES_HPP
#define HARBOR_CLS_HTTPRESPONSESTATUSCODES_HPP

#include <unordered_map>
#include <string>

namespace HarborCLS {

  enum class StatusCode {
    // 1xx: Informational
    Continue = 100,
    SwitchingProtocols = 101,
    Processing = 102,

    // 2xx: Success
    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultiStatus = 207,
    AlreadyReported = 208,
    IMUsed = 226,

    // 3xx: Redirection
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,

    // 4xx: Client Error
    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    PayloadTooLarge = 413,
    URITooLong = 414,
    UnsupportedMediaType = 415,
    RangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    MisdirectedRequest = 421,
    UnprocessableEntity = 422,
    Locked = 423,
    FailedDependency = 424,
    TooEarly = 425,
    UpgradeRequired = 426,
    PreconditionRequired = 428,
    TooManyRequests = 429,
    RequestHeaderFieldsTooLarge = 431,
    UnavailableForLegalReasons = 451,

    // 5xx: Server Error
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    InsufficientStorage = 507,
    LoopDetected = 508,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511
  };

  struct HttpResponseStatusCode {
    std::unordered_map<StatusCode, std::string> statusCodes;

    static HttpResponseStatusCode &getInstance() {
      static HttpResponseStatusCode instance;
      return instance;
    }

    static std::string getStatusCodeString(StatusCode statusCode) {
      if (!getInstance().statusCodes.contains(statusCode)) {
        return "";
      }

      return std::to_string(static_cast<int>(statusCode)) + " " + getInstance().statusCodes[statusCode];
    }

  protected:
    HttpResponseStatusCode() :
        statusCodes{
            {StatusCode::Continue,                      "Continue"},
            {StatusCode::SwitchingProtocols,            "Switching Protocols"},
            {StatusCode::Processing,                    "Processing"},
            {StatusCode::OK,                            "OK"},
            {StatusCode::Created,                       "Created"},
            {StatusCode::Accepted,                      "Accepted"},
            {StatusCode::NonAuthoritativeInformation,   "Non-Authoritative Information"},
            {StatusCode::NoContent,                     "No Content"},
            {StatusCode::ResetContent,                  "Reset Content"},
            {StatusCode::PartialContent,                "Partial Content"},
            {StatusCode::MultiStatus,                   "Multi-Status"},
            {StatusCode::AlreadyReported,               "Already Reported"},
            {StatusCode::IMUsed,                        "IM Used"},
            {StatusCode::MultipleChoices,               "Multiple Choices"},
            {StatusCode::MovedPermanently,              "Moved Permanently"},
            {StatusCode::Found,                         "Found"},
            {StatusCode::SeeOther,                      "See Other"},
            {StatusCode::NotModified,                   "Not Modified"},
            {StatusCode::UseProxy,                      "Use Proxy"},
            {StatusCode::TemporaryRedirect,             "Temporary Redirect"},
            {StatusCode::PermanentRedirect,             "Permanent Redirect"},
            {StatusCode::BadRequest,                    "Bad Request"},
            {StatusCode::Unauthorized,                  "Unauthorized"},
            {StatusCode::PaymentRequired,               "Payment Required"},
            {StatusCode::Forbidden,                     "Forbidden"},
            {StatusCode::NotFound,                      "Not Found"},
            {StatusCode::MethodNotAllowed,              "Method Not Allowed"},
            {StatusCode::NotAcceptable,                 "Not Acceptable"},
            {StatusCode::ProxyAuthenticationRequired,   "Proxy Authentication Required"},
            {StatusCode::RequestTimeout,                "Request Timeout"},
            {StatusCode::Conflict,                      "Conflict"},
            {StatusCode::Gone,                          "Gone"},
            {StatusCode::LengthRequired,                "Length Required"},
            {StatusCode::PreconditionFailed,            "Precondition Failed"},
            {StatusCode::PayloadTooLarge,               "Payload Too Large"},
            {StatusCode::URITooLong,                    "URI Too Long"},
            {StatusCode::UnsupportedMediaType,          "Unsupported Media Type"},
            {StatusCode::RangeNotSatisfiable,           "Range Not Satisfiable"},
            {StatusCode::ExpectationFailed,             "Expectation Failed"},
            {StatusCode::MisdirectedRequest,            "Misdirected Request"},
            {StatusCode::UnprocessableEntity,           "Unprocessable Entity"},
            {StatusCode::Locked,                        "Locked"},
            {StatusCode::FailedDependency,              "Failed Dependency"},
            {StatusCode::TooEarly,                      "Too Early"},
            {StatusCode::UpgradeRequired,               "Upgrade Required"},
            {StatusCode::PreconditionRequired,          "Precondition Required"},
            {StatusCode::TooManyRequests,               "Too Many Requests"},
            {StatusCode::RequestHeaderFieldsTooLarge,   "Request Header Fields Too Large"},
            {StatusCode::UnavailableForLegalReasons,    "Unavailable For Legal Reasons"},
            {StatusCode::InternalServerError,           "Internal Server Error"},
            {StatusCode::NotImplemented,                "Not Implemented"},
            {StatusCode::BadGateway,                    "Bad Gateway"},
            {StatusCode::ServiceUnavailable,            "Service Unavailable"},
            {StatusCode::GatewayTimeout,                "Gateway Timeout"},
            {StatusCode::HTTPVersionNotSupported,       "HTTP Version Not Supported"},
            {StatusCode::VariantAlsoNegotiates,         "Variant Also Negotiates"},
            {StatusCode::InsufficientStorage,           "Insufficient Storage"},
            {StatusCode::LoopDetected,                  "Loop Detected"},
            {StatusCode::NotExtended,                   "Not Extended"},
            {StatusCode::NetworkAuthenticationRequired, "Network Authentication Required"}
        } {}
  };
}
#endif //HARBOR_CLS_HTTPRESPONSESTATUSCODES_HPP
