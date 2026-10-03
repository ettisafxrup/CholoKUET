#pragma once

#include <string>
#include <vector>

#include "Location.h"

// A general tree: every node can have any number of children.
//
//   KUET
//   ├── Academic
//   │   ├── Central Library
//   │   └── ...
//   ├── Residential
//   └── ...
//
// Children are stored by value, so deleting a node deletes its whole
// subtree automatically.
struct TreeNode
{
    std::string name;
    int locationId = -1;             // -1 for the root and category nodes
    std::vector<TreeNode> children;
};

class CampusTree
{
public:
    // KUET -> category -> location. Categories keep the given order and
    // empty ones are left out.
    void build(const std::vector<std::string>& categories, const std::vector<Location>& locations);

    TreeNode& root() { return rootNode; }
    const TreeNode& root() const { return rootNode; }

    TreeNode* addChild(TreeNode& parent, const std::string& name, int locationId = -1);
    const TreeNode* find(const std::string& name) const;   // case-insensitive
    bool remove(const std::string& name);                  // removes the subtree

    void print() const;   // drawn with ├── and └── (a preorder walk)
    int countNodes() const;
    int height() const;

private:
    TreeNode rootNode{"KUET", -1, {}};
};
