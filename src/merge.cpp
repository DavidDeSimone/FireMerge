/***
Copyright (c) 2016 Big Fish Games 

Permission is hereby granted, free of charge, to any person obtaining a copy of this so
ftware and associated documentation files (the "Software"), to deal in the Software wit
hout restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit pers
ons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies o
r substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INC
LUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR P
URPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABL
E FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
***/
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <assert.h>
#include "merge.h"

using json = nlohmann::json;

constexpr int NO_CONFLICT = 0;
constexpr int CONFLICT = 1;

int main(int argc, char ** args) {
	std::cout << "Running FireMerge..." << std::endl;

	std::string resultName;
	if (argc == 4) {
		resultName = args[1];
	} else if (argc == 5) {
		resultName = args[4];
	}
  
	int returnCode = NO_CONFLICT;
	FileRevision mine(args[1]);
	FileRevision commonBase(args[2]);
	FileRevision theirs(args[3]);
	FileRevision result;
	result.jsonObject = json::array();
  
	// Iterate over each node in mine and theirs, looking for differences
	for (auto&& node : mine.jsonObject) {
		// Case one, node does not exist in theirs
		auto nodeID = mine.getID(node);
		json theirNode = theirs.getNode(nodeID);
		if (theirNode.empty()) { // No Result Found
			json baseNode = commonBase.getNode(nodeID);
			// Case 1a, Node is found in our diff, but not in theirs
			// and is not in the base diff
			if (baseNode.empty()) {
				result.mapIDToIndex(std::move(nodeID));
				result.jsonObject.push_back(node);
			} else {
				// Case 1b, Node is in our diff, but not in theirs and is in the
				// base diff. Theirs has removed this no, perform no action
			}
		} else {
			// Case two, node exists in both diffs
			if (node != theirNode) {
				// Case 2a: CONFLICT!!!
				json object = json::object();
				object["MINE >>>>>>>>"] = node;
				object["THEIRS <<<<<<<<"] = theirNode;
				result.jsonObject.push_back(std::move(object));
				returnCode = CONFLICT;
			} else {
				// Case 2b: no conflict
				result.jsonObject.push_back(node);
			}
			
			result.mapIDToIndex(std::move(mine.getID(node)));
		}
	}
	
	// Now we need to loop over theirs, to see if they have added any nodes
	for (auto& node : theirs.jsonObject) {
		auto nodeID = theirs.getID(node);
		json myNode = mine.getNode(nodeID);
		if (myNode.empty()) { // No Result Found
			json baseNode = commonBase.getNode(nodeID);
			if (baseNode.empty()) {
				// Node is in theirs, but not in base, they added it
				result.mapIDToIndex(std::move(nodeID));
				result.jsonObject.push_back(node);
			} else {
				// Node is in theirs, in base, and not in ours
				// We have removed this node, perform no-op
			}
		}
	}
	
	// Now we need to restore indexes for the result object
	result.convertIDsToIndex();
	writeJSON(result.jsonObject.dump(4), resultName);

	return returnCode;
}


FileRevision::FileRevision(const std::string& fileName) {
	jsonObject = readJSON(fileName);
	mapIndexToIDs();
	for (auto& node : jsonObject) {
		convertIndexToID(node);
	}
}

json FileRevision::getNode(const std::string& ID) {
	for (const auto& js : jsonObject) {
		if (getID(js) == ID) {
			return js;
		}
	}
	
	return json();
}

// This function is written for cocos creator 1.3, which
// has a very specific data format. This function may need
// to change if the data format changes.
std::string FileRevision::getID(const json& node) const {
	// All cc.Nodes will have an _id field, check for that first
	if (node.count("_id")) {
		return node["_id"];
	} if (node.count("fileId")) {
		// Prefabs are a special case, use their fieldID plus their type
		std::string _fid = node["fileId"];
		return _fid + "cc.preFab";
	} else {
		// Otherwise, we are assuming we are dealing with a component, 
		// (which we hope is a safe assumption)
		// Each node can only have one instance of a component
		// so we derive the components id as a combination of it's owning
		// node's id, plus it's type
		// so if cc.Node 12345 has a cc.Sprite component,
		// the sprite will have id 12345cc.Sprite
		std::string type;
		if (node.count("node")) {
			type = "node";
		} else if (node.count("scene")) {
			type = "scene";
		} else if (node.count("root")) {
			type = "root";
		} else if (node.count("target")) {
			type = "target";
		} else if (node.count("data")) {
			type = "data";
		}

		if (type.length()) {
			// If we have already performed id subsitution,
			// we just fetch the id as a string, overwise
			// we derive it 
			if (node[type]["__id__"].is_string()) {
				std::string _type = node["__type__"];
				std::string _id = node[type]["__id__"];
				return _id + _type;
			} else {
				int index = node[type]["__id__"];
				std::string _type = node["__type__"];
				auto it = jsonIndexMap.find(index);
				assert(it != jsonIndexMap.end());
				std::string _id = it->second + _type;
				return _id;
			}
		}
	}

	std::cout << "Failed to find ID for node " << node.dump(4) << std::endl;
	return "";
}

json readJSON(const std::string& fileName) {
	std::cout << "Reading File " << fileName << std::endl;
	std::ifstream t(fileName);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return json::parse(buffer.str());
}

void writeJSON(const std::string& json, const std::string& fileName) {
	std::ofstream out(fileName);
	out << json;
	out.close();
}

void FileRevision::convertIndexToID(json& node) {
	for (auto it = node.begin(); it != node.end(); ++it) {
		if (node.is_object() && it.key() == "__id__") {
			int value = it.value();
			node[it.key()] = jsonIndexMap[value];
		} else if (it.value().is_object() || it.value().is_array()) {
			convertIndexToID(it.value());
		}	
	}
}

void FileRevision::mapIndexToIDs() {
	// First, do a pass to fill out all cc.Nodes
	for (auto i = 0; i < jsonObject.size(); ++i) {
		auto& js = jsonObject[i];
		if (js.count("_id")) {
			std::string _id = js["_id"];
			jsonIndexMap.insert(std::make_pair(i, _id));
		}
	}


	// Now fill out all non cc.Node entries
	for (auto j = 0; j < jsonObject.size(); ++j) {
		auto& js = jsonObject[j];
		if (!js.count("_id")) {
			std::string id = getID(js);
			jsonIndexMap.insert(std::make_pair(j, id));
		}
	}
}

void FileRevision::mapIDToIndex(std::string&& id) {
	auto count = static_cast<int>(jsonIDMap.size());
	jsonIDMap.insert(std::make_pair(id, count));
}

void FileRevision::convertIDsToIndex() {
	for (auto& node : jsonObject) {
		convertIDToIndex(node);
	} 
}

void FileRevision::convertIDToIndex(json& node) {
	for (auto it = node.begin(); it != node.end(); ++it) {
		if (node.is_object() && it.key() == "__id__") {
			std::string value = it.value();

			auto node_it = jsonIDMap.find(value);
			if (node_it != jsonIDMap.end()) {
				int value = node_it->second;
				node["__id__"] = value;
			} else {
				std::cout << "DEBUG: node is midding id, dump() " << node.dump(4) << std::endl;
			}
		} else if (it.value().is_object() || it.value().is_array()) {
			convertIDToIndex(it.value());
		}	
	}
}
