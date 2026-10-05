#include "../include/CampusTree.h"
#include "../include/Utils.h"

#include <iostream>

void CampusTree::build(const std::vector<std::string> &categories, const std::vector<Location> &locations)
{
    rootNode = TreeNode{"KUET", -1, {}};

    for (const std::string &category : categories)
    {
        TreeNode group{category, -1, {}};
        for (const Location &location : locations)
        {
            if (equalsIgnoreCase(location.category, category))
                group.children.push_back({location.name, location.id, {}});
        }
        if (!group.children.empty())
            rootNode.children.push_back(group);
    }
}

TreeNode *CampusTree::addChild(TreeNode &parent, const std::string &name, int locationId)
{
    parent.children.push_back({name, locationId, {}});
    return &parent.children.back();
}

static const TreeNode *findIn(const TreeNode &node, const std::string &name)
{
    if (equalsIgnoreCase(node.name, name))
        return &node;

    for (const TreeNode &child : node.children)
    {
        if (const TreeNode *found = findIn(child, name))
            return found;
    }
    return nullptr;
}

const TreeNode *CampusTree::find(const std::string &name) const
{
    return findIn(rootNode, name);
}

static bool removeFrom(TreeNode &parent, const std::string &name)
{
    for (size_t i = 0; i < parent.children.size(); i++)
    {
        if (equalsIgnoreCase(parent.children[i].name, name))
        {
            parent.children.erase(parent.children.begin() + i);
            return true;
        }
        if (removeFrom(parent.children[i], name))
            return true;
    }
    return false;
}

bool CampusTree::remove(const std::string &name)
{
    return removeFrom(rootNode, name);
}

static void printBranch(const TreeNode &node, const std::string &indent, bool last)
{
    std::cout << "  " << indent << (last ? "└── " : "├── ") << node.name << "\n";

    std::string childIndent = indent + (last ? "    " : "│   ");
    for (size_t i = 0; i < node.children.size(); i++)
        printBranch(node.children[i], childIndent, i + 1 == node.children.size());
}

void CampusTree::print() const
{
    std::cout << "  " << rootNode.name << "\n";
    for (size_t i = 0; i < rootNode.children.size(); i++)
        printBranch(rootNode.children[i], "", i + 1 == rootNode.children.size());
}

static int count(const TreeNode &node)
{
    int total = 1;
    for (const TreeNode &child : node.children)
        total += count(child);
    return total;
}

int CampusTree::countNodes() const
{
    return count(rootNode);
}

static int heightOf(const TreeNode &node)
{
    int tallest = 0;
    for (const TreeNode &child : node.children)
    {
        int h = heightOf(child);
        if (h > tallest)
            tallest = h;
    }
    return tallest + 1;
}

int CampusTree::height() const
{
    return heightOf(rootNode);
}
