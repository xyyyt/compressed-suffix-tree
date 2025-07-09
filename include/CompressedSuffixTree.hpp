#ifndef COMPRESSED_SUFFIX_TREE_HPP_
# define COMPRESSED_SUFFIX_TREE_HPP_

# include <unordered_map>
# include <string>
# include <string_view>
# include <memory>
# include <utility>
# include <cassert>

# define assertm(EXPR, MSG) assert((void(MSG), EXPR))

namespace container
{
    template <template <typename...> typename Alloc = std::allocator>
    class CompressedSuffixTree
    {
        struct Node;

    public :
        CompressedSuffixTree() = default;

        CompressedSuffixTree(const CompressedSuffixTree& other) :
            _size(other._size),
            _wordCount(other._wordCount),
            _root(Node::deepCopy(other._root))
        { }

        CompressedSuffixTree(CompressedSuffixTree&& other) :
            _size(std::exchange(other._size, 0)),
            _wordCount(std::exchange(other._wordCount, 0)),
            _root(std::exchange(
                other._root, std::allocate_shared<Node>(Alloc<Node>{})))
        { }

        CompressedSuffixTree(std::initializer_list<std::string_view> initList)
        {
            for (auto sv : initList)
            {
                insert(sv);
            }
        }

        template <typename InputIterator>
        CompressedSuffixTree(InputIterator begin, InputIterator end)
        {
            for (; begin != end; ++begin)
            {
                insert(*begin);
            }
        }

        CompressedSuffixTree& operator=(const CompressedSuffixTree& other)
        {
            if (this != &other)
            {
                _size = other._size;
                _wordCount = other._wordCount;
                _root = Node::deepCopy(other._root);
            }

            return *this;
        }

        CompressedSuffixTree& operator=(CompressedSuffixTree&& other)
        {
            if (this != &other)
            {
                _size = std::exchange(other._size, 0);
                _wordCount = std::exchange(other._wordCount, 0);
                _root = std::exchange(
                    other._root, std::allocate_shared<Node>(Alloc<Node>{}));
            }

            return *this;
        }

        [[nodiscard]]
        friend inline bool operator==(
            const CompressedSuffixTree& lhs, const CompressedSuffixTree& rhs) noexcept
        {
            return lhs._size == rhs._size
                && lhs._wordCount == rhs._wordCount
                && Node::deepEqual(lhs._root, rhs._root);
        }

        [[nodiscard]]
        friend inline bool operator!=(
            const CompressedSuffixTree& lhs, const CompressedSuffixTree& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        [[nodiscard]]
        inline bool empty() const noexcept { return _root->childNodes.empty(); }

        [[nodiscard]]
        inline size_t size() const noexcept { return _size; }

        [[nodiscard]]
        inline size_t wordCount() const noexcept { return _wordCount; }

#ifdef SUFFIXTREE_TEST
        [[nodiscard]]
        inline std::weak_ptr<const Node> root() const noexcept
        {
            return std::static_pointer_cast<const Node>(_root);
        }
#endif

        [[nodiscard]]
         inline bool search(std::string_view word) const
        {
            return search(_root, word);
        }

        [[nodiscard]]
        inline bool endsWith(std::string_view suffix) const
        {
            return endsWith(_root, suffix);
        }

        bool insert(std::string_view word)
        {
            if (word.empty() || !insert(_root, word, true))
            {
                return false;
            }

            for (size_t n = 1; n < word.size(); ++n)
            {
                bool res = insert(_root, word.substr(n), false);

                assertm(res, "res cannot be false");
            }

            return true;
        }

        bool erase(std::string_view word)
        {
            if (word.empty() || !erase(_root, word, true))
            {
                return false;
            }

            for (size_t n = 1; n < word.size(); ++n)
            {
                bool res = erase(_root, word.substr(n), false);

                assertm(res, "res cannot false");
            }

            return true;
        }

        void clear()
        {
            _size = 0;
            _wordCount = 0;
            _root = std::allocate_shared<Node>(Alloc<Node>{});
        }

    private :
        using CustomString_t = std::basic_string<
            char, std::char_traits<char>, Alloc<char>>;

        template <typename Key, typename T>
        using CustomHashMap_t = std::unordered_map<
            Key,
            T,
            std::hash<Key>,
            std::equal_to<Key>,
            Alloc<std::pair<const Key, T>>>;

        using ChildNodes_t = CustomHashMap_t<
            std::string_view, std::shared_ptr<Node>>;

        struct Node
        {
            CustomString_t s = "";
            bool terminalWord = false; // true means that's node represents end of word
            int terminalCount = 0; /* could represent end of word as well as end of
                                      suffixes from others words */

            /* to optimize the search, we should use a Compressed Trie / Prefix Tree
               rather than a hash map */
            ChildNodes_t childNodes;

            [[nodiscard]]
            static std::shared_ptr<Node> deepCopy(const std::shared_ptr<Node> nodeOther)
            {
                if (!nodeOther)
                {
                    return nullptr;
                }

                auto node = std::allocate_shared<Node>(Alloc<Node>{});

                node->s = nodeOther->s;
                node->terminalWord = nodeOther->terminalWord;
                node->terminalCount = nodeOther->terminalCount;

                for (const auto& [_, childNodeOther] : nodeOther->childNodes)
                {
                    auto childNode = deepCopy(childNodeOther);

                    node->childNodes.emplace(childNode->s, childNode);
                }

                return node;
            }

            [[nodiscard]]
            static bool deepEqual(const std::shared_ptr<Node> node,
                                  const std::shared_ptr<Node> nodeOther)
            {
                if (!node && !nodeOther)
                {
                    return true;
                }
                else if (!node
                         || !nodeOther
                         || (node->s != nodeOther->s
                             || node->terminalWord != nodeOther->terminalWord
                             || node->terminalCount != nodeOther->terminalCount
                             || node->childNodes.size() != nodeOther->childNodes.size()))
                {
                    return false;
                }

                for (const auto& [sv, childNode] : node->childNodes)
                {
                    auto it = nodeOther->childNodes.find(sv);

                    if (it == nodeOther->childNodes.cend()
                        || !deepEqual(childNode, it->second))
                    {
                        return false;
                    }
                }

                return true;
            }

            [[nodiscard]]
            auto findByDeterminingPrefix(std::string_view sv) const
                -> std::pair<
                    typename std::decay_t<decltype(childNodes)>::const_iterator,
                    size_t>
            {
                if (sv.empty())
                {
                    return {{}, 0};
                }

                auto begin = childNodes.cbegin();
                auto end = childNodes.cend();

                while (begin != end)
                {
                    if (begin->first[0] == sv[0])
                    {
                        size_t endPos = 1;

                        while (endPos < begin->first.size()
                               && endPos < sv.size()
                               && begin->first[endPos] == sv[endPos])
                        {
                            ++endPos;
                        }

                        return {begin, endPos};
                    }

                    ++begin;
                }

                return {{}, 0};
            }
        };

        size_t _size = 0;
        size_t _wordCount = 0;
        std::shared_ptr<Node> _root = std::allocate_shared<Node>(Alloc<Node>{});

        [[nodiscard]]
        bool search(const std::shared_ptr<Node> node, std::string_view word) const
        {
            if (word.empty())
            {
                return node->terminalWord;
            }

            auto [it, endPos] = node->findByDeterminingPrefix(word);

            return (it != node->childNodes.cend() && endPos >= it->first.size()) ?
                search(it->second, word.substr(endPos)) : false;
        }

        [[nodiscard]]
        bool endsWith(const std::shared_ptr<Node> node, std::string_view suffix) const
        {
            if (suffix.empty())
            {
                return (!node->terminalWord && node->terminalCount > 0)
                    || (node->terminalWord && node->terminalCount > 1);
            }

            auto [it, endPos] = node->findByDeterminingPrefix(suffix);

            return (it != node->childNodes.cend() && endPos >= it->first.size()) ?
                endsWith(it->second, suffix.substr(endPos)) : false;
        }

        bool insert(std::shared_ptr<Node> node, std::string_view sv, bool isWord)
        {
            if (sv.empty())
            {
                // only for word
                if (isWord)
                {
                    if (node->terminalWord)
                    {
                        return false;
                    }

                    // node is considered as end of word now
                    node->terminalWord = true;
                    ++_wordCount;
                }

                ++node->terminalCount;

                return true;
            }

            auto [it, endPos] = node->findByDeterminingPrefix(sv);
            decltype(it) it2 = {};
            size_t endPos2 = 0;

            if (it == node->childNodes.cend())
            {
                auto childNode = std::allocate_shared<Node>(Alloc<Node>{});

                childNode->s.assign(sv.data(), sv.size());
                it2 = node->childNodes.emplace(childNode->s, childNode).first;
                ++_size;
                endPos2 = sv.size();
            }
            else
            {
                if (endPos < it->first.size())
                {
                    auto substr = std::string(
                        it->first.data() + endPos,
                        it->first.size() - endPos);

                    // iterator "it" will be invalid after this line
                    auto containerNodeHandle = node->childNodes.extract(it);

                    auto childNode = containerNodeHandle.mapped();

                    // truncate string to keep prefix only
                    childNode->s.resize(endPos);

                    bool terminalWord = std::exchange(
                        childNode->terminalWord, false);
                    int terminalCount = std::exchange(
                        childNode->terminalCount, 0);

                    containerNodeHandle.key() = childNode->s;

                    // reinsert the modified child node
                    it2 = node->childNodes.insert(
                        std::move(containerNodeHandle)).position;

                    assertm(it2 != node->childNodes.end(),
                            "it2 cannot be null");

                    auto childNode2 = std::allocate_shared<Node>(Alloc<Node>{});

                    childNode2->s = std::move(substr);
                    childNode2->terminalWord = terminalWord;
                    childNode2->terminalCount = terminalCount;
                    childNode2->childNodes = std::move(it2->second->childNodes);

                    auto it3 = it2->second->childNodes.emplace(
                        childNode2->s, childNode2).first;

                    assertm(it3 != it2->second->childNodes.end(),
                            "it3 cannot be null");
                    ++_size;
                }
                else
                {
                    auto sv2 = sv.substr(0, endPos);

                    it2 = node->childNodes.find(sv2);
                    assertm(it2 != node->childNodes.end(),
                            "it2 cannot be null");
                }

                endPos2 = endPos;
            }

            return insert(it2->second, sv.substr(endPos2), isWord);
        }

        bool erase(std::shared_ptr<Node> node, std::string_view sv, bool isWord)
        {
            if (sv.empty())
            {
                // only for word
                if (isWord)
                {
                    if (!node->terminalWord)
                    {
                        return false;
                    }

                    // node is no more considered as end of word
                    node->terminalWord = false;
                    --_wordCount;
                }

                --node->terminalCount;

                return true;
            }

            auto [it, endPos] = node->findByDeterminingPrefix(sv);

            if (it == node->childNodes.cend() || endPos < it->first.size())
            {
                return false;
            }

            bool res = erase(it->second, sv.substr(endPos), isWord);

            if (res && !it->second->terminalCount)
            {
                if (it->second->childNodes.empty())
                {
                    node->childNodes.erase(it);
                    --_size;
                }
                else if (it->second->childNodes.size() == 1)
                {
                    auto containerNodeHandle = node->childNodes.extract(it);
                    auto childNode = containerNodeHandle.mapped();
                    auto it2 = childNode->childNodes.begin();

                    assertm(it2 != childNode->childNodes.end(),
                            "it2 cannot be null");
                    childNode->s.append(it2->second->s);

                    // if (auto it3 = node->childNodes.find(childNode->s);
                    //     it3 != node->childNodes.end())
                    // {
                    //     if (!it3->second->terminalWord
                    //         && it2->second->terminalWord)
                    //     {
                    //         it3->second->terminalWord = true;
                    //     }

                    //     it3->second->terminalCount += it2->second->terminalCount;

                    //     /* no need to call "node->childNodes.erase(it)" because
                    //        childNode has been extracted outside of the container
                    //        and will be destroyed with RAII */
                    //     --_size;
                    // }
                    // else
                    // {

                    childNode->terminalWord = it2->second->terminalWord;
                    childNode->terminalCount = it2->second->terminalCount;
                    containerNodeHandle.key() = childNode->s;
                    node->childNodes.insert(std::move(containerNodeHandle));

                    childNode->childNodes.erase(it2);
                    --_size;
                }
            }

            return res;
        }
    };
}

#endif
