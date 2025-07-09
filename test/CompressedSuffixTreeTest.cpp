#define SUFFIXTREE_TEST

#include <gtest/gtest.h>

#include "CompressedSuffixTree.hpp"

using namespace container;

/*
  Each diagram drawn about the tree status does not represent
  exactly the stocking order of the nodes inserted at each level
  of depth tree because of the internal management about std::unordered_map.

  Nevertheless, each diagram contains the correct number of child nodes and
  correct values about a node.

  Acronyms on drawn diagrams :
    tw -> terminal word
    tc -> terminal count
*/

TEST(CompressedSuffixTree, Test_1)
{
    CompressedSuffixTree tree;

    //                   Tree status
    //
    //                       ""
    //                   (tw = false,
    //                     tc = 0)

    ASSERT_TRUE(tree.empty());
    ASSERT_EQ(tree.size(), 0);
    ASSERT_EQ(tree.wordCount(), 0);

    auto root = tree.root().lock();

    if (!root)
    {
        FAIL() << "expected root to exist";
    }

    EXPECT_TRUE(root->s.empty());
    EXPECT_FALSE(root->terminalWord);
    EXPECT_EQ(root->terminalCount, 0);
    EXPECT_TRUE(root->childNodes.empty());

    // checks with empty string
    EXPECT_FALSE(tree.insert(""));
    EXPECT_FALSE(tree.erase(""));
    EXPECT_FALSE(tree.search(""));
    EXPECT_FALSE(tree.endsWith(""));

    EXPECT_EQ(tree, CompressedSuffixTree{});

    EXPECT_TRUE(tree.insert("a"));

    //                   Tree status (after insert "a")
    //
    //                     [root]
    //                       ""
    //                   (tw = false,
    //                     tc = 0)
    //
    //                        |
    //
    //                       "a"
    //                   (tw = true,
    //                     tc = 1)


    ASSERT_FALSE(tree.empty());
    ASSERT_EQ(tree.size(), 1);
    ASSERT_EQ(tree.wordCount(), 1);
    EXPECT_TRUE(tree.search("a"));
    EXPECT_FALSE(tree.endsWith("a"));

    EXPECT_FALSE(tree.insert("a"));

    EXPECT_TRUE(tree.insert("ab"));

    //                  Tree status (after insert "ab")
    //
    //                    [root]
    //            - -       ""      - -
    //                  (tw = false,
    //                    tc = 0)
    //
    //            |                   |
    //
    //           "b"                 "a"
    //       (tw = false,        (tw = true,
    //         tc = 1)             tc = 1)
    //                                |
    //
    //                               "b"
    //                           (tw = true,
    //                             tc = 1)


    ASSERT_FALSE(tree.empty());
    ASSERT_EQ(tree.size(), 3);
    ASSERT_EQ(tree.wordCount(), 2);
    EXPECT_TRUE(tree.search("ab"));
    EXPECT_TRUE(tree.search("a"));
    EXPECT_FALSE(tree.search("b"));
    EXPECT_FALSE(tree.endsWith("ab"));
    EXPECT_FALSE(tree.endsWith("a"));
    EXPECT_TRUE(tree.endsWith("b"));

    EXPECT_FALSE(tree.insert("ab"));

    EXPECT_TRUE(tree.insert("cab"));

    //                  Tree status (after insert "cab")
    //
    //                                           [root]
    //        - -                        - -       ""      - -
    //                                         (tw = false,
    //                                           tc = 0)
    //
    //        |                          |                   |
    //
    //       "cab"                      "b"                 "a"
    //    (tw = true,               (tw = false,        (tw = true,
    //      tc = 1)                   tc = 2)             tc = 1)
    //                                                       |
    //
    //                                                      "b"
    //                                                  (tw = true,
    //                                                    tc = 2)


    ASSERT_FALSE(tree.empty());
    ASSERT_EQ(tree.size(), 4);
    ASSERT_EQ(tree.wordCount(), 3);
    EXPECT_TRUE(tree.search("cab"));
    EXPECT_FALSE(tree.search("c"));
    EXPECT_FALSE(tree.search("ca"));
    EXPECT_FALSE(tree.search("b"));
    EXPECT_TRUE(tree.search("ab"));
    EXPECT_FALSE(tree.endsWith("cab"));
    EXPECT_FALSE(tree.endsWith("c"));
    EXPECT_FALSE(tree.endsWith("ca"));
    EXPECT_TRUE(tree.endsWith("b"));
    EXPECT_TRUE(tree.endsWith("ab"));

    CompressedSuffixTree tree2 = tree;

    ASSERT_FALSE(tree2.empty());
    ASSERT_EQ(tree2.size(), 4);
    ASSERT_EQ(tree2.wordCount(), 3);

    auto root2 = tree2.root().lock();

    if (!root2)
    {
        FAIL() << "expected root2 to exist";
    }

    EXPECT_TRUE(root2->s.empty());
    EXPECT_FALSE(root2->terminalWord);
    EXPECT_EQ(root2->terminalCount, 0);
    EXPECT_FALSE(root2->childNodes.empty());
    EXPECT_EQ(tree2, tree);

    CompressedSuffixTree tree3 = std::move(tree);

    ASSERT_TRUE(tree.empty());
    ASSERT_EQ(tree.size(), 0);
    ASSERT_EQ(tree.wordCount(), 0);

    root = tree.root().lock();

    if (!root)
    {
        FAIL() << "expected root to exist";
    }

    EXPECT_TRUE(root->s.empty());
    EXPECT_FALSE(root->terminalWord);
    EXPECT_EQ(root->terminalCount, 0);
    EXPECT_TRUE(root->childNodes.empty());
    EXPECT_NE(tree, tree3);

    ASSERT_FALSE(tree3.empty());
    ASSERT_EQ(tree3.size(), 4);
    ASSERT_EQ(tree3.wordCount(), 3);

    auto root3 = tree3.root().lock();

    if (!root3)
    {
        FAIL() << "expected root3 to exist";
    }

    EXPECT_TRUE(root3->s.empty());
    EXPECT_FALSE(root3->terminalWord);
    EXPECT_EQ(root3->terminalCount, 0);
    EXPECT_FALSE(root3->childNodes.empty());
    EXPECT_EQ(tree2, tree3);

    tree2.clear();

    ASSERT_TRUE(tree2.empty());
    ASSERT_EQ(tree2.size(), 0);
    ASSERT_EQ(tree2.wordCount(), 0);

    root2 = tree.root().lock();

    if (!root)
    {
        FAIL() << "expected root to exist";
    }

    EXPECT_TRUE(root2->s.empty());
    EXPECT_FALSE(root2->terminalWord);
    EXPECT_EQ(root2->terminalCount, 0);
    EXPECT_TRUE(root2->childNodes.empty());

    EXPECT_TRUE(tree3.erase("a"));

    //                  Tree status (after erase "a")
    //
    //                                           [root]
    //        - -                        - -       ""      - -
    //                                         (tw = false,
    //                                           tc = 0)
    //
    //        |                          |                   |
    //
    //       "cab"                      "b"                 "ab"
    //    (tw = true,               (tw = false,        (tw = true,
    //      tc = 1)                   tc = 2)             tc = 2)


    ASSERT_FALSE(tree3.empty());
    ASSERT_EQ(tree3.size(), 3);
    ASSERT_EQ(tree3.wordCount(), 2);
    EXPECT_FALSE(tree3.search("a"));
    EXPECT_FALSE(tree3.endsWith("a"));

    EXPECT_FALSE(tree3.erase("a"));

    EXPECT_TRUE(tree3.erase("ab"));

    //                  Tree status (after erase "ab")
    //
    //                                           [root]
    //        - -                        - -       ""      - -
    //                                         (tw = false,
    //                                           tc = 0)
    //
    //        |                          |                   |
    //
    //       "cab"                      "b"                 "ab"
    //    (tw = true,               (tw = false)         (tw = false,
    //      tc = 1)                   tc = 1)              tc = 1)

    ASSERT_FALSE(tree3.empty());
    ASSERT_EQ(tree3.size(), 3);
    ASSERT_EQ(tree3.wordCount(), 1);
    EXPECT_FALSE(tree3.search("ab"));
    EXPECT_FALSE(tree3.search("a"));
    EXPECT_FALSE(tree3.search("b"));
    EXPECT_TRUE(tree3.endsWith("ab"));
    EXPECT_FALSE(tree3.endsWith("a"));
    EXPECT_TRUE(tree3.endsWith("b"));

    EXPECT_FALSE(tree3.erase("ab"));

    EXPECT_TRUE(tree3.erase("cab"));

    //                   Tree status (after erase "cab")
    //
    //                       ""
    //                   (tw = false,
    //                     tc = 0)


    ASSERT_TRUE(tree3.empty());
    ASSERT_EQ(tree3.size(), 0);
    ASSERT_EQ(tree3.wordCount(), 0);
    EXPECT_FALSE(tree3.search("cab"));
    EXPECT_FALSE(tree3.search("c"));
    EXPECT_FALSE(tree3.search("ca"));
    EXPECT_FALSE(tree3.search("b"));
    EXPECT_FALSE(tree3.search("ab"));
    EXPECT_FALSE(tree3.endsWith("cab"));
    EXPECT_FALSE(tree3.endsWith("c"));
    EXPECT_FALSE(tree3.endsWith("ca"));
    EXPECT_FALSE(tree3.endsWith("b"));
    EXPECT_FALSE(tree3.endsWith("ab"));

    root3 = tree3.root().lock();

    if (!root3)
    {
        FAIL() << "expected root3 to exist";
    }

    EXPECT_TRUE(root3->s.empty());
    EXPECT_FALSE(root3->terminalWord);
    EXPECT_EQ(root3->terminalCount, 0);
    EXPECT_TRUE(root3->childNodes.empty());
    EXPECT_EQ(tree3, tree2);
}

TEST(CompressedSuffixTree, Test_2)
{
    CompressedSuffixTree tree = {"abde", "abc", "b", "abd", "b"}; // last "b" element won't affect tree

    //                  Tree status (after insert "abde")
    //
    //                                           [root]
    //        - -                        - -       ""      - -                        - -
    //                                         (tw = false,
    //                                           tc = 0)
    //
    //        |                          |                   |                          |
    //
    //       "e"                        "de"               "bde"                      "abde"
    //   (tw = false,                (tw = false,        (tw = false,               (tw = true,
    //     tc = 1)                     tc = 1)             tc = 1)                    tc = 1)




    //                  Tree status (after insert "abc")
    //
    //                                                                      [root]
    //        - -                        - -                        - -       ""      - -                                 - -
    //                                                                    (tw = false,
    //                                                                      tc = 0)
    //
    //        |                          |                          |                   |                                   |
    //
    //       "c"                        "e"                        "de"                "b"                                "ab"
    //    (tw = false,              (tw = false,                (tw = false,       (tw = false,                        (tw = false,
    //      tc = 1)                   tc = 1)                     tc = 1)            tc = 0)                             tc = 0)
    //
    //                                                                          - -             - -               - -             - -
    //
    //                                                                          |                 |               |                 |
    //
    //                                                                         "c"               "de"            "c"               "de"
    //                                                                     (tw = false,      (tw = false,    (tw = true,        (tw = true,
    //                                                                       tc = 1)           tc = 1)         tc = 1)            tc = 1)




    //                  Tree status (after insert "b")
    //
    //                                                                      [root]
    //        - -                        - -                        - -       ""      - -                                 - -
    //                                                                    (tw = false,
    //                                                                      tc = 0)
    //
    //        |                          |                          |                   |                                   |
    //
    //       "c"                        "e"                        "de"                "b"                                "ab"
    //    (tw = false,              (tw = false,                (tw = false,       (tw = true,                        (tw = false,
    //      tc = 1)                   tc = 1)                     tc = 1)            tc = 1)                            tc = 0)
    //
    //                                                                          - -             - -               - -             - -
    //
    //                                                                          |                 |               |                 |
    //
    //                                                                         "c"               "de"            "c"               "de"
    //                                                                     (tw = false,      (tw = false,    (tw = true,        (tw = true,
    //                                                                       tc = 1)           tc = 1)         tc = 1)            tc = 1)




    //                  Tree status (after insert "abd")
    //
    //                                                                      [root]
    //        - -                        - -                       - -        ""         - -                                 - -
    //                                                                    (tw = false,
    //                                                                      tc = 0)
    //
    //        |                          |                         |                       |                                   |
    //
    //       "c"                        "e"                       "d"                     "b"                                "ab"
    //   (tw = false,               (tw = false,              (tw = false,            (tw = true,                         (tw = false,
    //     tc = 1)                    tc = 1)                   tc = 1)                 tc = 1)                             tc = 0)
    //
    //                                                                            - -             - -               - -                 - -
    //
    //                                                             |              |                 |               |                     |
    //
    //                                                            "e"            "c"               "d"             "c"                   "d"
    //                                                        (tw = false,   (tw = false,      (tw = false,    (tw = true,           (tw = true,
    //                                                          tc = 1)        tc = 1)           tc = 1)         tc = 1)               tc = 1)
    //
    //                                                                                              |                                     |
    //
    //                                                                                             "e"                                   "e"
    //                                                                                         (tw = false,                          (tw = true,
    //                                                                                           tc = 1)                               tc = 1)

    ASSERT_FALSE(tree.empty());
    ASSERT_EQ(tree.size(), 12);
    ASSERT_EQ(tree.wordCount(), 4);

    auto root = tree.root().lock();

    if (!root)
    {
        FAIL() << "expected root to exist";
    }

    EXPECT_TRUE(root->s.empty());
    EXPECT_FALSE(root->terminalWord);
    EXPECT_EQ(root->terminalCount, 0);
    EXPECT_FALSE(root->childNodes.empty());

    // checks for "abde" word
    EXPECT_TRUE(tree.search("abde"));
    EXPECT_FALSE(tree.search("a"));
    EXPECT_FALSE(tree.search("ab"));
    EXPECT_TRUE(tree.search("abd"));
    EXPECT_FALSE(tree.search("e"));
    EXPECT_FALSE(tree.search("de"));
    EXPECT_FALSE(tree.search("bde"));
    EXPECT_FALSE(tree.endsWith("abde"));
    EXPECT_FALSE(tree.endsWith("a"));
    EXPECT_FALSE(tree.endsWith("ab"));
    EXPECT_FALSE(tree.endsWith("abd"));
    EXPECT_TRUE(tree.endsWith("e"));
    EXPECT_TRUE(tree.endsWith("de"));
    EXPECT_TRUE(tree.endsWith("bde"));

    // checks for "abc" word
    EXPECT_TRUE(tree.search("abc"));
    EXPECT_FALSE(tree.search("a"));
    EXPECT_FALSE(tree.search("ab"));
    EXPECT_FALSE(tree.search("c"));
    EXPECT_FALSE(tree.search("bc"));
    EXPECT_FALSE(tree.endsWith("abc"));
    EXPECT_FALSE(tree.endsWith("a"));
    EXPECT_FALSE(tree.endsWith("ab"));
    EXPECT_TRUE(tree.endsWith("c"));
    EXPECT_TRUE(tree.endsWith("bc"));

    // checks for "b" word
    EXPECT_TRUE(tree.search("b"));
    EXPECT_FALSE(tree.endsWith("b"));

    // checks for "abd" word
    EXPECT_TRUE(tree.search("abd"));
    EXPECT_FALSE(tree.search("a"));
    EXPECT_FALSE(tree.search("ab"));
    EXPECT_FALSE(tree.search("d"));
    EXPECT_FALSE(tree.search("bd"));
    EXPECT_FALSE(tree.endsWith("abd"));
    EXPECT_FALSE(tree.endsWith("a"));
    EXPECT_FALSE(tree.endsWith("ab"));
    EXPECT_TRUE(tree.endsWith("d"));
    EXPECT_TRUE(tree.endsWith("bd"));

    std::string_view array[] = {"", "b", "abd", "", "abde", "abc"};
    CompressedSuffixTree tree2(std::cbegin(array), std::cend(array));

    ASSERT_FALSE(tree2.empty());
    ASSERT_EQ(tree2.size(), 12);
    ASSERT_EQ(tree2.wordCount(), 4);

    auto root2 = tree2.root().lock();

    if (!root2)
    {
        FAIL() << "expected root2 to exist";
    }

    EXPECT_TRUE(root2->s.empty());
    EXPECT_FALSE(root2->terminalWord);
    EXPECT_EQ(root2->terminalCount, 0);
    EXPECT_FALSE(root2->childNodes.empty());
    EXPECT_EQ(tree2, tree);

    tree.clear();

    EXPECT_NE(tree2, tree);

    CompressedSuffixTree tree3;

    ASSERT_TRUE(tree3.empty());
    ASSERT_EQ(tree3.size(), 0);
    ASSERT_EQ(tree3.wordCount(), 0);

    auto root3 = tree3.root().lock();

    if (!root3)
    {
        FAIL() << "expected root3 to exist";
    }

    EXPECT_TRUE(root3->s.empty());
    EXPECT_FALSE(root3->terminalWord);
    EXPECT_EQ(root3->terminalCount, 0);
    EXPECT_TRUE(root3->childNodes.empty());

    tree3 = tree2;

    ASSERT_FALSE(tree3.empty());
    ASSERT_EQ(tree3.size(), 12);
    ASSERT_EQ(tree3.wordCount(), 4);

    root3 = tree3.root().lock();

    if (!root3)
    {
        FAIL() << "expected root3 to exist";
    }

    EXPECT_TRUE(root3->s.empty());
    EXPECT_FALSE(root3->terminalWord);
    EXPECT_EQ(root3->terminalCount, 0);
    EXPECT_FALSE(root3->childNodes.empty());
    EXPECT_EQ(tree3, tree2);

    tree2.clear();

    CompressedSuffixTree tree4;

    tree4 = std::move(tree3);

    ASSERT_EQ(tree3.size(), 0);
    ASSERT_EQ(tree3.wordCount(), 0);

    root3 = tree3.root().lock();

    if (!root3)
    {
        FAIL() << "expected root3 to exist";
    }

    EXPECT_TRUE(root3->s.empty());
    EXPECT_FALSE(root3->terminalWord);
    EXPECT_EQ(root3->terminalCount, 0);
    EXPECT_TRUE(root3->childNodes.empty());

    ASSERT_FALSE(tree4.empty());
    ASSERT_EQ(tree4.size(), 12);
    ASSERT_EQ(tree4.wordCount(), 4);

    auto root4 = tree4.root().lock();

    if (!root4)
    {
        FAIL() << "expected root4 to exist";
    }

    EXPECT_TRUE(root4->s.empty());
    EXPECT_FALSE(root4->terminalWord);
    EXPECT_EQ(root4->terminalCount, 0);
    EXPECT_FALSE(root4->childNodes.empty());
    EXPECT_NE(tree4, tree3);

    EXPECT_TRUE(tree4.erase("abde"));

    //                  Tree status (after erase "abde")
    //
    //                                           [root]
    //        - -                       - -        ""         - -                                 - -
    //                                         (tw = false,
    //                                           tc = 0)
    //
    //        |                         |                       |                                   |
    //
    //       "c"                       "d"                     "b"                                "ab"
    //   (tw = false,              (tw = false,            (tw = true,                         (tw = false,
    //     tc = 1)                   tc = 1)                 tc = 1)                             tc = 0)
    //
    //                                                - -             - -               - -                 - -
    //
    //                                                |                 |               |                     |
    //
    //                                               "c"               "d"             "c"                   "d"
    //                                           (tw = false,      (tw = false,    (tw = true,           (tw = true,
    //                                             tc = 1)           tc = 1)         tc = 1)               tc = 1)


    ASSERT_FALSE(tree4.empty());
    ASSERT_EQ(tree4.size(), 8);
    ASSERT_EQ(tree4.wordCount(), 3);
    EXPECT_FALSE(tree4.search("abde"));
    EXPECT_FALSE(tree4.search("a"));
    EXPECT_FALSE(tree4.search("ab"));
    EXPECT_TRUE(tree4.search("abd"));
    EXPECT_FALSE(tree4.search("e"));
    EXPECT_FALSE(tree4.search("de"));
    EXPECT_FALSE(tree4.search("bde"));
    EXPECT_FALSE(tree4.endsWith("abde"));
    EXPECT_FALSE(tree4.endsWith("a"));
    EXPECT_FALSE(tree4.endsWith("ab"));
    EXPECT_FALSE(tree4.endsWith("abd"));
    EXPECT_FALSE(tree4.endsWith("e"));
    EXPECT_FALSE(tree4.endsWith("de"));
    EXPECT_FALSE(tree4.endsWith("bde"));

    EXPECT_FALSE(tree4.erase("abde"));

    EXPECT_TRUE(tree4.erase("b"));

    //                  Tree status (after erase "b")
    //
    //                                           [root]
    //        - -                       - -        ""         - -                                 - -
    //                                         (tw = false,
    //                                           tc = 0)
    //
    //        |                         |                       |                                   |
    //
    //       "c"                       "d"                     "b"                                "ab"
    //   (tw = false,              (tw = false,            (tw = false,                        (tw = false,
    //     tc = 1)                   tc = 1)                 tc = 0)                             tc = 0)
    //
    //                                                - -             - -               - -                 - -
    //
    //                                                |                 |               |                     |
    //
    //                                               "c"               "d"             "c"                   "d"
    //                                           (tw = false,      (tw = false,    (tw = true,           (tw = true,
    //                                             tc = 1)           tc = 1)         tc = 1)               tc = 1)


    ASSERT_FALSE(tree4.empty());
    ASSERT_EQ(tree4.size(), 8);
    ASSERT_EQ(tree4.wordCount(), 2);
    EXPECT_FALSE(tree4.search("b"));
    EXPECT_FALSE(tree4.endsWith("b"));

    EXPECT_FALSE(tree4.erase("b"));

    EXPECT_TRUE(tree4.erase("abd"));

    //                  Tree status (after erase "abd")
    //
    //                    [root]
    //           - -        ""         - -                                 - -
    //                  (tw = false,
    //                    tc = 0)
    //
    //          |                        |                                   |
    //
    //         "c"                     "bc"                                "abc"
    //     (tw = false,             (tw = false,                        (tw = true,
    //       tc = 1)                  tc = 1)                             tc = 1)


    ASSERT_FALSE(tree4.empty());
    ASSERT_EQ(tree4.size(), 3);
    ASSERT_EQ(tree4.wordCount(), 1);
    EXPECT_FALSE(tree4.search("abd"));
    EXPECT_FALSE(tree4.search("a"));
    EXPECT_FALSE(tree4.search("ab"));
    EXPECT_FALSE(tree4.search("d"));
    EXPECT_FALSE(tree4.search("bd"));
    EXPECT_FALSE(tree4.endsWith("abd"));
    EXPECT_FALSE(tree4.endsWith("a"));
    EXPECT_FALSE(tree4.endsWith("ab"));
    EXPECT_FALSE(tree4.endsWith("d"));
    EXPECT_FALSE(tree4.endsWith("bd"));

    EXPECT_FALSE(tree4.erase("abd"));

    EXPECT_TRUE(tree4.insert("c"));

    //                  Tree status (after insert "c")
    //
    //                    [root]
    //           - -        ""         - -                                 - -
    //                  (tw = false,
    //                    tc = 0)
    //
    //          |                        |                                   |
    //
    //         "c"                     "bc"                                "abc"
    //     (tw = true,              (tw = false,                        (tw = true,
    //       tc = 2)                  tc = 1)                             tc = 1)


    ASSERT_FALSE(tree4.empty());
    ASSERT_EQ(tree4.size(), 3);
    ASSERT_EQ(tree4.wordCount(), 2);
    EXPECT_TRUE(tree4.search("c"));
    EXPECT_TRUE(tree4.endsWith("c"));

    EXPECT_FALSE(tree4.insert("c"));

    EXPECT_TRUE(tree4.erase("abc"));

    //                  Tree status (after erase "abc")
    //
    //                    [root]
    //                      ""
    //                  (tw = false,
    //                    tc = 0)
    //
    //                       |
    //
    //                      "c"
    //                  (tw = true,
    //                    tc = 1)


    ASSERT_FALSE(tree4.empty());
    ASSERT_EQ(tree4.size(), 1);
    ASSERT_EQ(tree4.wordCount(), 1);
    EXPECT_FALSE(tree4.search("abc"));
    EXPECT_FALSE(tree4.search("a"));
    EXPECT_FALSE(tree4.search("ab"));
    EXPECT_TRUE(tree4.search("c"));
    EXPECT_FALSE(tree4.search("bc"));
    EXPECT_FALSE(tree4.endsWith("abc"));
    EXPECT_FALSE(tree4.endsWith("a"));
    EXPECT_FALSE(tree4.endsWith("ab"));
    EXPECT_FALSE(tree4.endsWith("c"));
    EXPECT_FALSE(tree4.endsWith("bc"));

    EXPECT_FALSE(tree4.erase("abc"));

    EXPECT_TRUE(tree4.erase("c"));

    //                  Tree status (after erase "c")
    //
    //                      ""
    //                  (tw = false,
    //                    tc = 0)


    ASSERT_TRUE(tree4.empty());
    ASSERT_EQ(tree4.size(), 0);
    ASSERT_EQ(tree4.wordCount(), 0);

    root4 = tree4.root().lock();

    if (!root4)
    {
        FAIL() << "expected root4 to exist";
    }

    EXPECT_TRUE(root4->s.empty());
    EXPECT_FALSE(root4->terminalWord);
    EXPECT_EQ(root4->terminalCount, 0);
    EXPECT_TRUE(root4->childNodes.empty());

    EXPECT_FALSE(tree4.search("c"));
    EXPECT_FALSE(tree4.endsWith("c"));

    EXPECT_FALSE(tree4.erase("c"));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
