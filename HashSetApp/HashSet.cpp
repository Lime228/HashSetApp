#include "pch.h"
#include "HashSet.h"
#include "Logger.h"

HashSet::Iterator::Iterator(
    typename std::vector<std::unordered_set<std::string>>::const_iterator bucketIter,
    typename std::vector<std::unordered_set<std::string>>::const_iterator bucketEnd,
    typename std::unordered_set<std::string>::const_iterator elementIter)
    : bucketIter(bucketIter), bucketEnd(bucketEnd), elementIter(elementIter)
{
    Logger::LogInfo("Создание итератора HashSet");
    skipEmptyBuckets();
}

HashSet::Iterator& HashSet::Iterator::operator++() {
    if (bucketIter == bucketEnd) {
        Logger::LogInfo("Итератор достиг конца контейнера");
        return *this;
    }

    ++elementIter;
    if (elementIter == bucketIter->end()) {
        ++bucketIter;
        skipEmptyBuckets();
        Logger::LogInfo("Итератор перешел к следующему бакету");
    }
    return *this;
}

HashSet::Iterator HashSet::Iterator::operator++(int) {
    Logger::LogInfo("Постфиксный инкремент итератора");
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool HashSet::Iterator::operator==(const Iterator& other) const {
    bool result = bucketIter == other.bucketIter &&
        (bucketIter == bucketEnd || elementIter == other.elementIter);
    Logger::LogInfo("Сравнение итераторов: " + std::string(result ? "равны" : "не равны"));
    return result;
}

bool HashSet::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

const std::string& HashSet::Iterator::operator*() const {
    Logger::LogInfo("Дереференцирование итератора");
    return *elementIter;
}

const std::string* HashSet::Iterator::operator->() const {
    Logger::LogInfo("Доступ к члену через итератор");
    return &(*elementIter);
}

void HashSet::Iterator::skipEmptyBuckets() {
    while (bucketIter != bucketEnd && bucketIter->empty()) {
        ++bucketIter;
    }
    if (bucketIter != bucketEnd) {
        elementIter = bucketIter->begin();
    }
    Logger::LogInfo("Пропуск пустых бакетов завершен");
}

HashSet::HashSet(size_t bucketCount) : buckets(bucketCount) {
    Logger::LogInfo("Создание HashSet с " + std::to_string(bucketCount) + " бакетами");
}

bool HashSet::insert(const std::string& value) {
    size_t bucketIndex = hashFunction(value) % buckets.size();
    auto& bucket = buckets[bucketIndex];
    auto result = bucket.insert(value);

    if (result.second) {
        Logger::LogInfo("Добавлено слово в HashSet: '" + value + "' в бакет " + std::to_string(bucketIndex));
    }
    else {
        Logger::LogWarning("Попытка добавить существующее слово: '" + value + "'");
    }

    return result.second;
}

bool HashSet::erase(const std::string& value) {
    size_t bucketIndex = hashFunction(value) % buckets.size();
    auto& bucket = buckets[bucketIndex];
    bool result = bucket.erase(value) > 0;

    if (result) {
        Logger::LogInfo("Удалено слово из HashSet: '" + value + "' из бакета " + std::to_string(bucketIndex));
    }
    else {
        Logger::LogWarning("Попытка удалить отсутствующее слово: '" + value + "'");
    }

    return result;
}

bool HashSet::contains(const std::string& value) const {
    size_t bucketIndex = hashFunction(value) % buckets.size();
    const auto& bucket = buckets[bucketIndex];
    bool result = bucket.find(value) != bucket.end();

    Logger::LogInfo("Проверка наличия слова '" + value + "': " + (result ? "найдено" : "не найдено"));
    return result;
}

HashSet::Iterator HashSet::begin() const {
    Logger::LogInfo("Получение begin() итератора");
    return Iterator(buckets.begin(), buckets.end(), buckets.begin()->begin());
}

HashSet::Iterator HashSet::end() const {
    Logger::LogInfo("Получение end() итератора");
    return Iterator(buckets.end(), buckets.end(), buckets.begin()->begin());
}

size_t HashSet::size() const {
    size_t count = 0;
    for (const auto& bucket : buckets) {
        count += bucket.size();
    }

    Logger::LogInfo("Получение размера HashSet: " + std::to_string(count) + " элементов");
    return count;
}

bool HashSet::empty() const {
    bool result = size() == 0;
    Logger::LogInfo("Проверка на пустоту HashSet: " + std::string(result ? "пуст" : "не пуст"));
    return result;
}

std::vector<std::string> HashSet::getShortWords(size_t maxLength) const {
    Logger::LogInfo("Поиск коротких слов (макс. длина: " + std::to_string(maxLength) + ")");

    std::vector<std::string> result;
    for (const auto& word : *this) {
        if (word.length() < maxLength) {
            result.push_back(word);
        }
    }

    std::sort(result.begin(), result.end());
    Logger::LogInfo("Найдено коротких слов: " + std::to_string(result.size()));
    return result;
}

std::vector<std::string> HashSet::getPalindromes() const {
    Logger::LogInfo("Поиск палиндромов");

    std::vector<std::string> result;
    for (const auto& word : *this) {
        if (isPalindrome(word)) {
            result.push_back(word);
        }
    }

    std::sort(result.begin(), result.end());
    Logger::LogInfo("Найдено палиндромов: " + std::to_string(result.size()));
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
        Logger::LogInfo("Проверка на палиндром: слово '" + word + "' слишком короткое");
        return false;
    }

    size_t i = 0;
    size_t j = word.length() - 1;

    while (i < j) {
        if (word[i] != word[j]) {
            Logger::LogInfo("Слово '" + word + "' не является палиндромом");
            return false;
        }
        i++;
        j--;
    }

    Logger::LogInfo("Найден палиндром: '" + word + "'");
    return true;
}