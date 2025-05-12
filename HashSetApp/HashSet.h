#pragma once
#include <vector>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>

class HashSet {
public:
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = const std::string*;
        using reference = const std::string&;

        Iterator(typename std::vector<std::unordered_set<std::string>>::const_iterator bucketIter,
            typename std::vector<std::unordered_set<std::string>>::const_iterator bucketEnd,
            typename std::unordered_set<std::string>::const_iterator elementIter);

        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        reference operator*() const;
        pointer operator->() const;

    private:
        void skipEmptyBuckets();
        typename std::vector<std::unordered_set<std::string>>::const_iterator bucketIter;
        typename std::vector<std::unordered_set<std::string>>::const_iterator bucketEnd;
        typename std::unordered_set<std::string>::const_iterator elementIter;
    };

    HashSet(size_t bucketCount = 16);
    bool insert(const std::string& value);
    bool erase(const std::string& value);
    bool contains(const std::string& value) const;
    Iterator begin() const;
    Iterator end() const;
    size_t size() const;
    bool empty() const;
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        std::vector<std::string> words;
        std::string word;

        while (file >> word) {
            // Очистка слова
            word.erase(std::remove_if(word.begin(), word.end(),
                [](char c) { return !isalpha(c); }), word.end());

            if (!word.empty()) {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                words.push_back(word);
            }
        }

        // Добавляем слова в хеш-таблицу
        buckets.clear();
        for (const auto& w : words) {
            size_t bucketIndex = hashFunction(w) % buckets.size();
            buckets[bucketIndex].insert(w);
        }

        return true;
    }
    std::vector<std::string> getShortWords(size_t maxLength = 4) const;
    std::vector<std::string> getPalindromes() const;

private:
    std::vector<std::unordered_set<std::string>> buckets;
    size_t hashFunction(const std::string& str) const;
    bool isPalindrome(const std::string& word) const;
};