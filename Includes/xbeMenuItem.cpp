#include "xbeMenuItem.h"

xbeMenuItem::xbeMenuItem(char* text, char* p) :
  menuItem(text) {
  xbePath = (char*)malloc(strlen(p) * sizeof(char) + 1);
  strcpy(xbePath, p);
}

xbeMenuItem::xbeMenuItem(const char* text, const char* p) :
  xbeMenuItem(const_cast<char*>(text), const_cast<char*>(p)) {}

xbeMenuItem::xbeMenuItem(xbeMenuItem const& item) :
  xbeMenuItem(item.getLabel(), item.getXBEPath()) {
  this->setTexture(item.getTexture());
}

xbeMenuItem::~xbeMenuItem() {
  if (xbePath != nullptr) {
    free(xbePath);
    xbePath = nullptr;
  }
}

const char* xbeMenuItem::getXBEPath() const {
  return xbePath;
}

void xbeMenuItem::setXBEPath(const char* p) {
  xbePath = static_cast<char*>(realloc(xbePath, strlen(p) * sizeof(char) + 1));
  strcpy(xbePath, p);
}
