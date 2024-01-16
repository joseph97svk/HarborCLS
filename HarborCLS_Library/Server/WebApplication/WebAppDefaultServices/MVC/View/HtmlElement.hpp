//
// Created by josephvalverde on 1/15/24.
//

#ifndef HARBOR_CLS_HTMLELEMENT_HPP
#define HARBOR_CLS_HTMLELEMENT_HPP

#include <string>
#include <vector>
#include <memory>

namespace HarborCLS {

  class HtmlElement {
    std::string _elementTag;

    std::string _name;
    std::string _id;

    std::vector<std::pair<std::string, std::string>> _properties;
    std::vector<std::pair<std::string, std::string>> _styles;

    std::vector<std::shared_ptr<HtmlElement>> _children;

    std::string _content;

  public:
    explicit HtmlElement(std::string elementTag, std::string name = "", std::string id = "")
        : _elementTag(std::move(elementTag)), _name(std::move(name)), _id(std::move(id)) {
    }

    HtmlElement& addProperty(std::string property, std::string value) {
      _properties.emplace_back(std::move(property), std::move(value));

      return *this;
    }

    HtmlElement& addStyle(std::string style, std::string value) {
      _styles.emplace_back(std::move(style), std::move(value));

      return *this;
    }

    HtmlElement& addChild(std::shared_ptr<HtmlElement> child) {
      _children.push_back(std::move(child));

      return *this;
    }

    HtmlElement& addContent(std::string_view content) {
      _content += content;

      return *this;
    }

    std::string toString() {
      std::string result = "<" + _elementTag;

      if (!_name.empty()) {
        result += " name=\"" + _name + "\" ";
      }

      if (!_id.empty()) {
        result += " id=\"" + _id + "\" ";
      }

      for (const auto &property: _properties) {
        result += " " + property.first + "=\"" + property.second + "\" ";
      }

      result += " style=\"";

      for (const auto &style: _styles) {
        result += style.first + ": " + style.second + "; ";
      }

      result += "\" ";

      result += ">\n";

      result += _content;

      for (const auto &child: _children) {
        result += child->toString();
      }

      result += "</" + _elementTag + ">\n";

      return result;
    }

    static std::shared_ptr<HtmlElement> createHtmlElement(
        std::string elementTag
        , std::string name = ""
        , std::string id = "") {
      return std::make_shared<HtmlElement>(std::move(elementTag), std::move(name), std::move(id));
    }
  };
}

#endif //HARBOR_CLS_HTMLELEMENT_HPP
