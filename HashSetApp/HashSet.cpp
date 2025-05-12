#include "pch.h"
#include "HashSet.h"
#include "Logger.h"

HashSet::Iterator::Iterator(
    typename std::vector<std::unordered_set<std::string>>::const_iterator bucketIter,
    typename std::vector<std::unordered_set<std::string>>::const_iterator bucketEnd,
    typename std::unordered_set<std::string>::const_iterator elementIter)
    : bucketIter(bucketIter), bucketEnd(bucketEnd), elementIter(elementIter)
{
    Logger::LogInfo("�������� ��������� HashSet");
    skipEmptyBuckets();
}

HashSet::Iterator& HashSet::Iterator::operator++() {
    if (bucketIter == bucketEnd) {
        Logger::LogInfo("�������� ������ ����� ����������");
        return *this;
    }

    ++elementIter;
    if (elementIter == bucketIter->end()) {
        ++bucketIter;
        skipEmptyBuckets();
        Logger::LogInfo("�������� ������� � ���������� ������");
    }
    return *this;
}

HashSet::Iterator HashSet::Iterator::operator++(int) {
    Logger::LogInfo("����������� ��������� ���������");
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool HashSet::Iterator::operator==(const Iterator& other) const {
    bool result = bucketIter == other.bucketIter &&
        (bucketIter == bucketEnd || elementIter == other.elementIter);
    Logger::LogInfo("��������� ����������: " + std::string(result ? "�����" : "�� �����"));
    return result;
}

bool HashSet::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

const std::string& HashSet::Iterator::operator*() const {
    Logger::LogInfo("������������������ ���������");
    return *elementIter;
}

const std::string* HashSet::Iterator::operator->() const {
    Logger::LogInfo("������ � ����� ����� ��������");
    return &(*elementIter);
}

void HashSet::Iterator::skipEmptyBuckets() {
    while (bucketIter != bucketEnd && bucketIter->empty()) {
        ++bucketIter;
    }
    if (bucketIter != bucketEnd) {
        elementIter = bucketIter->begin();
    }
    Logger::LogInfo("������� ������ ������� ��������");
}

HashSet::HashSet(size_t bucketCount) : buckets(bucketCount) {
    Logger::LogInfo("�������� HashSet � " + std::to_string(bucketCount) + " ��������");
}

bool HashSet::insert(const std::string& value) {
    size_t bucketIndex = hashFunction(value) % buckets.size();
    auto& bucket = buckets[bucketIndex];
    auto result = bucket.insert(value);

    if (result.second) {
        Logger::LogInfo("��������� ����� � HashSet: '" + value + "' � ����� " + std::to_string(bucketIndex));
    }
    else {
        Logger::LogWarning("������� �������� ������������ �����: '" + value + "'");
    }

    return result.second;
}

bool HashSet::erase(const std::string& value) {
    size_t bucketIndex = hashFunction(value) % buckets.size();
    auto& bucket = buckets[bucketIndex];
    bool result = bucket.erase(value) > 0;

    if (result) {
        Logger::LogInfo("������� ����� �� HashSet: '" + value + "' �� ������ " + std::to_string(bucketIndex));
    }
    else {
        Logger::LogWarning("������� ������� ������������� �����: '" + value + "'");
    }

    return result;
}

bool HashSet::contains(const std::string& value) const {
    size_t bucketIndex = hashFunction(value) % buckets.size();
    const auto& bucket = buckets[bucketIndex];
    bool result = bucket.find(value) != bucket.end();

    Logger::LogInfo("�������� ������� ����� '" + value + "': " + (result ? "�������" : "�� �������"));
    return result;
}

HashSet::Iterator HashSet::begin() const {
    Logger::LogInfo("��������� begin() ���������");
    return Iterator(buckets.begin(), buckets.end(), buckets.begin()->begin());
}

HashSet::Iterator HashSet::end() const {
    Logger::LogInfo("��������� end() ���������");
    return Iterator(buckets.end(), buckets.end(), buckets.begin()->begin());
}

size_t HashSet::size() const {
    size_t count = 0;
    for (const auto& bucket : buckets) {
        count += bucket.size();
    }

    Logger::LogInfo("��������� ������� HashSet: " + std::to_string(count) + " ���������");
    return count;
}

bool HashSet::empty() const {
    bool result = size() == 0;
    Logger::LogInfo("�������� �� ������� HashSet: " + std::string(result ? "����" : "�� ����"));
    return result;
}

std::vector<std::string> HashSet::getShortWords(size_t maxLength) const {
    Logger::LogInfo("����� �������� ���� (����. �����: " + std::to_string(maxLength) + ")");

    std::vector<std::string> result;
    for (const auto& word : *this) {
        if (word.length() < maxLength) {
            result.push_back(word);
        }
    }

    std::sort(result.begin(), result.end());
    Logger::LogInfo("������� �������� ����: " + std::to_string(result.size()));
    return result;
}

std::vector<std::string> HashSet::getPalindromes() const {
    Logger::LogInfo("����� �����������");

    std::vector<std::string> result;
    for (const auto& word : *this) {
        if (isPalindrome(word)) {
            result.push_back(word);
        }
    }

    std::sort(result.begin(), result.end());
    Logger::LogInfo("������� �����������: " + std::to_string(result.size()));
    return result;
}

size_t HashSet::hashFunction(const std::string& str) const {
    size_t hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

bool HashSet::isPalindrome(const std::string& word) const {
    if (word.length() < 2) {
        Logger::LogInfo("�������� �� ���������: ����� '" + word + "' ������� ��������");
        return false;
    }

    size_t i = 0;
    size_t j = word.length() - 1;

    while (i < j) {
        if (word[i] != word[j]) {
            Logger::LogInfo("����� '" + word + "' �� �������� �����������");
            return false;
        }
        i++;
        j--;
    }

    Logger::LogInfo("������ ���������: '" + word + "'");
    return true;
}