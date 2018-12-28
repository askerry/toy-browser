// Classes for representing a DOM hierarchy.

#include "dom.h"

#include <iostream>
#include <map>
#include <regex>
#include <vector>

#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"

#include "constants.h"
#include "util.h"

const std::vector<std::string> METATAGS = {
    constants::html_tags::HEAD, constants::html_tags::STYLE,
    constants::html_tags::LINK, constants::html_tags::META};

namespace dom {

std::string TextNode::toLogStr() const { return "text = '" + text_ + "'"; }

std::string ElementNode::toLogStr() const {
  return absl::StrFormat("\n\ttag: %s\n\tid: %s\n\tclasses: %s", get_tag(),
                         get_id(), getAttr(constants::html_attributes::CLASS));
}

std::vector<std::string> ElementNode::get_classes() const {
  return absl::StrSplit(getAttr(constants::html_attributes::CLASS), ' ');
}

std::string ElementNode::get_id() const {
  return getAttr(constants::html_attributes::ID);
}

std::string ElementNode::getAttr(const std::string &attribute,
                                 const std::string &default_value) const {
  auto it = attrs_.find(attribute);
  if (it == attrs_.end()) {
    return default_value;
  }
  return it->second;
}

bool ElementNode::isDisplayable() const {
  return std::find(METATAGS.begin(), METATAGS.end(), get_tag()) ==
         METATAGS.end();
}
}  // namespace dom
