#pragma once
#include <unordered_map>
#include <string>
#include <memory>
class Symbol;
using LinkerSymbolTable=std::unordered_map<std::string_view,std::unique_ptr<Symbol>>;