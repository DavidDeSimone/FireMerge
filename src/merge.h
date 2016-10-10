/***
Copyright (c) 2016 Big Fish Games 

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***/

#include <unordered_map>
#include <set>
#include <string>
#include "json.hpp"

class FileRevision {
 public:
  FileRevision(const std::string& fileName);
  FileRevision() = default;

  std::string getID(const nlohmann::json& node) const;

  void convertIDsToIndex();
  void mapIndexToIDs();
  void mapIDToIndex(std::string&& id);

  nlohmann::json getNode(const std::string& ID);
  nlohmann::json jsonObject;

 private:
  void convertIDToIndex(nlohmann::json& node);
  void convertIndexToID(nlohmann::json& node);

  std::unordered_map<int, std::string> jsonIndexMap;
  std::unordered_map<std::string, int> jsonIDMap;
};

nlohmann::json readJSON(const std::string& fileName);
void writeJSON(const std::string& json, const std::string& fileName);


