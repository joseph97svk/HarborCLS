//
// Created by josephvalverde on 12/25/23.
//

#ifndef HARBOR_CLS_BASEVIEW_HPP
#define HARBOR_CLS_BASEVIEW_HPP

#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <optional>
#include <functional>

namespace HarborCLS {

  class BaseView {
    std::string _htmlFileName {};
    std::optional<std::string> _layoutFileName {};

    std::vector<std::pair<std::string, std::string>> _replacements {};

  public:
    explicit BaseView(std::string htmlFileName, const std::optional<std::string>& layoutFileName = std::nullopt)
        : _htmlFileName(std::move(htmlFileName))
        , _layoutFileName(layoutFileName) {
    }

    [[nodiscard]] std::optional<std::string> getHtmlContents() {
      std::string htmlContents {};
      std::fstream htmlFile(_htmlFileName);

      if (!htmlFile.is_open()) {
        return std::nullopt;
      }

      while (!htmlFile.eof()) {
        htmlContents += BaseView::getLineContents(htmlFile
            , R"(\{.*\})"
            , [this](std::string& line, std::smatch& foundTag)-> void {
              this->replaceTagWithContents(line, foundTag);
            } );
      }

      htmlFile.close();

      if (_layoutFileName) {
        placePageWithinLayout(htmlContents);
      }

      return htmlContents;
    }

    void replace(const std::string& key, const std::string& value) {
      std::string replacementTag = "{" + key + "}";
      _replacements.emplace_back(replacementTag, value);
    }

  protected:
    [[nodiscard]] static inline std::string getLineContents(
        std::fstream& htmlFile
        , const std::string& tag
        , std::function<void(std::string&, std::smatch&)> replacementFunction) {
      std::string line {};
      std::getline(htmlFile, line);

      std::regex findReplacementTag {tag};
      std::smatch foundTag;
      std::string::const_iterator searchStart(line.cbegin());

      if (std::regex_search(searchStart, line.cend(), foundTag, findReplacementTag)) {
        replacementFunction(line, foundTag);
      }

      return line;
    }

    void replaceTagWithContents(std::string& line
                                , std::smatch& foundTag) {
      std::string replacementTag = foundTag.str();
      std::string replacementValue {};

      auto tagReplacement = std::find_if(
          _replacements.begin()
          , _replacements.end()
          , [replacementTag](std::pair<std::string, std::string>& value) -> bool {
            return value.first == replacementTag;
          });

      if (tagReplacement != _replacements.end()) {
        replacementValue = tagReplacement->second;
      }

      line.replace(foundTag.position(), foundTag.length(), replacementValue);
    }

    inline void placePageWithinLayout(std::string& pageContents) {
      std::fstream layoutFile(*_layoutFileName);

      if (!layoutFile.is_open()) {
        return;
      }

      std::string layoutContents {};

      while (!layoutFile.eof()) {
        auto replacementFunction =
            [pageContents](std::string& line, std::smatch& foundTag) -> void {
              std::string replacementTag = foundTag.str();
              line.replace(foundTag.position(), foundTag.length(), pageContents);
            };

        layoutContents += BaseView::getLineContents(layoutFile
            , R"(\{Body\})"
            , replacementFunction);
      }

      pageContents = layoutContents;
    }
  };

}

#endif //HARBOR_CLS_BASEVIEW_HPP
