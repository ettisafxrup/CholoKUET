#pragma once

#include <string>
#include <vector>

#include "Location.h"

struct TreeNode
{
    std::string name;
    int locationId = -1;
    std::vector<TreeNode> children;
};

class CampusTree
{
public:
    void build(const std::vector<std::string> &categories, const std::vector<Location> &locations);

    TreeNode &root() { return rootNode; }
    const TreeNode &root() const { return rootNode; }

    TreeNode *addChild(TreeNode &parent, const std::string &name, int locationId = -1);
    const TreeNode *find(const std::string &name) const;
    bool remove(const std::string &name);

    void print() const;
    int countNodes() const;
    int height() const;

private:
    TreeNode rootNode{"KUET", -1, {}};
};
