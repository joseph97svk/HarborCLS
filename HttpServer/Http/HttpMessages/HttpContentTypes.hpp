//
// Created by josephvalverde on 12/17/23.
//

#ifndef LEGO_FIGURE_MAKER_HTTPCONTENTTYPES_HPP
#define LEGO_FIGURE_MAKER_HTTPCONTENTTYPES_HPP

#include <unordered_map>

enum class ContentType {
  TextHtml,
  TextPlain,
  TextCss,
  TextJavascript,
  ApplicationJson,
  ApplicationXml,
  ApplicationXhtmlXml,
  ApplicationJavascript,
  ApplicationPdf,
  ApplicationMsword,
  ApplicationVndMsExcel,
  ApplicationVndOpenxmlformatsOfficedocumentSpreadsheetmlSheet,
  ApplicationVndOpenxmlformatsOfficedocumentWordprocessingmlDocument,
  ApplicationZip,
  ApplicationOctetStream,
  ImagePng,
  ImageJpeg,
  ImageGif,
  ImageSvgXml,
  ImageWebp,
  ImageBmp,
  ImageXIcon,
  VideoMp4,
  VideoQuicktime,
  VideoXMsVideo,
  VideoWebm,
  AudioMpeg,
  AudioOgg,
  AudioWav,
  AudioWebm,
  AudioFlac,
  AudioXWav,
  AudioXAac,
  MultipartFormData,
  TextCsv,
  TextXml,
  ApplicationRssXml,
  ApplicationAtomXml
};

const std::unordered_map<ContentType, std::string> contentTypeMap = {
    {ContentType::TextHtml, "text/html"},
    {ContentType::TextPlain, "text/plain"},
    {ContentType::TextCss, "text/css"},
    {ContentType::TextJavascript, "text/javascript"},
    {ContentType::ApplicationJson, "application/json"},
    {ContentType::ApplicationXml, "application/xml"},
    {ContentType::ApplicationXhtmlXml, "application/xhtml+xml"},
    {ContentType::ApplicationJavascript, "application/javascript"},
    {ContentType::ApplicationPdf, "application/pdf"},
    {ContentType::ApplicationMsword, "application/msword"},
    {ContentType::ApplicationVndMsExcel, "application/vnd.ms-excel"},
    {ContentType::ApplicationVndOpenxmlformatsOfficedocumentSpreadsheetmlSheet, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {ContentType::ApplicationVndOpenxmlformatsOfficedocumentWordprocessingmlDocument, "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {ContentType::ApplicationZip, "application/zip"},
    {ContentType::ApplicationOctetStream, "application/octet-stream"},
    {ContentType::ImagePng, "image/png"},
    {ContentType::ImageJpeg, "image/jpeg"},
    {ContentType::ImageGif, "image/gif"},
    {ContentType::ImageSvgXml, "image/svg+xml"},
    {ContentType::ImageWebp, "image/webp"},
    {ContentType::ImageBmp, "image/bmp"},
    {ContentType::ImageXIcon, "image/x-icon"},
    {ContentType::VideoMp4, "video/mp4"},
    {ContentType::VideoQuicktime, "video/quicktime"},
    {ContentType::VideoXMsVideo, "video/x-msvideo"},
    {ContentType::VideoWebm, "video/webm"},
    {ContentType::AudioMpeg, "audio/mpeg"},
    {ContentType::AudioOgg, "audio/ogg"},
    {ContentType::AudioWav, "audio/wav"},
    {ContentType::AudioWebm, "audio/webm"},
    {ContentType::AudioFlac, "audio/flac"},
    {ContentType::AudioXWav, "audio/x-wav"},
    {ContentType::AudioXAac, "audio/x-aac"},
    {ContentType::MultipartFormData, "multipart/form-data"},
    {ContentType::TextCsv, "text/csv"},
    {ContentType::TextXml, "text/xml"},
    {ContentType::ApplicationRssXml, "application/rss+xml"},
    {ContentType::ApplicationAtomXml, "application/atom+xml"}
};


#endif //LEGO_FIGURE_MAKER_HTTPCONTENTTYPES_HPP
