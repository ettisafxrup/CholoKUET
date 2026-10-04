#include "../include/CampusTree.h"
#include "check.h"

int main()
{
    CampusTree tree;
    CHECK(tree.countNodes() == 1);
    CHECK(tree.height() == 1);

    std::vector<Location> places = {
        {4, "CSE", "Academic", "", 0, 0},
        {3, "Library", "Facility", "", 0, 0},
        {5, "EEE", "academic", "", 0, 0},
    };
    tree.build({"Academic", "Sports", "Facility"}, places);

    CHECK(tree.root().children.size() == 2);
    CHECK(tree.root().children[0].name == "Academic");
    CHECK(tree.root().children[0].children.size() == 2);
    CHECK(tree.countNodes() == 6);
    CHECK(tree.height() == 3);

    const TreeNode *eee = tree.find("eee");
    CHECK(eee != nullptr && eee->locationId == 5);
    CHECK(tree.find("Gym") == nullptr);

    TreeNode *sports = tree.addChild(tree.root(), "Sports");
    tree.addChild(*sports, "Gymnasium", 22);
    CHECK(tree.find("Gymnasium") != nullptr);

    CHECK(tree.remove("Academic"));
    CHECK(tree.find("CSE") == nullptr);
    CHECK(tree.find("Library") != nullptr);
    CHECK(!tree.remove("Academic"));
    CHECK(tree.countNodes() == 5);

    return finish("Tree");
}
