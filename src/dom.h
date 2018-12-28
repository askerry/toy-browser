// Classes for representing a DOM hierarchy.

#ifndef DOM_H
#define DOM_H

#include <iostream>
#include <map>
#include <vector>

#include "constants.h"

typedef std::map<std::string, std::string> Attrs;

namespace dom {

// Represents a node in the DOM tree. Can be either TextNode or ElementNode.
class Node {
 private:
  std::vector<std::unique_ptr<Node>> children_;

 public:
  Node(){};
  Node(std::vector<std::unique_ptr<Node>> children) {
    children_ = std::move(children);
  };
  virtual ~Node() {}
  // Delete copy constructor
  Node(const Node &node) = delete;
  Node &operator=(const Node &node) = delete;

  std::vector<std::reference_wrapper<Node>> get_children() const {
    std::vector<std::reference_wrapper<Node>> children;
    for (const auto &child : children_) {
      children.push_back(*child);
    }
    return children;
  };

  virtual std::string toLogStr() const = 0;
};

// Represents a raw text node.
class TextNode : public Node {
  const std::string text_;

 public:
  // Delete copy constructor
  TextNode(const TextNode &node) = delete;
  TextNode(std::string text) : text_(std::move(text)) {}
  TextNode(std::string text, std::vector<std::unique_ptr<Node>> children)
      : Node(std::move(children)), text_(std::move(text)) {}
  std::string get_text() const { return text_; }

  std::string toLogStr() const override;
};

// Represents any non-text node of the DOM.
class ElementNode : public Node {
 private:
  const std::string tag_name_;
  const std::map<std::string, std::string> attrs_;

 public:
  ElementNode(const ElementNode &node) = delete;

  ElementNode(std::string tag_name, Attrs attrs)
      : tag_name_(std::move(tag_name)), attrs_(std::move(attrs)){};
  ElementNode(std::string tag_name, Attrs attrs,
              std::vector<std::unique_ptr<Node>> children)
      : Node(std::move(children)),
        tag_name_(std::move(tag_name)),
        attrs_(std::move(attrs)){};

  std::string get_id() const;
  std::string get_tag() const { return tag_name_; };
  std::vector<std::string> get_classes() const;

  // Returns whether this node is one that is displayed to the screen,
  // or something like <head>, <meta> that are used for metadata only.
  bool isDisplayable() const;
  // Return the provided attribute. Returns the provided default_value
  // if the attribute is not present in attrs_.
  std::string getAttr(
      const std::string &property,
      const std::string &default_value = constants::DEFAULT) const;
  std::string toLogStr() const override;
};

}  // namespace dom

#endif
