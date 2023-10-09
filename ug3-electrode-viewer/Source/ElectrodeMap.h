//
// Created by Allen Munk on 9/7/23.
//

#ifndef CPP_ELECTRODEMAP_H
#define CPP_ELECTRODEMAP_H

#include <map>
#include <utility>
#include <string>

struct ElectrodeMapKey {

    ElectrodeMapKey(std::string channelName, std::string streamName) : m_channelName(channelName), m_streamName(streamName) {}

    ElectrodeMapKey(const ElectrodeMapKey& key) : m_channelName(key.m_channelName), m_streamName(key.m_streamName) {}

    std::string m_channelName;
    std::string m_streamName;

    bool operator==(const ElectrodeMapKey& otherKey) const {
        return(m_channelName == otherKey.m_channelName && m_streamName == otherKey.m_streamName);
    }

};

template<>
struct std::hash<ElectrodeMapKey> {
    std::size_t operator()(const ElectrodeMapKey& key) const noexcept {
        std::size_t channelHash = std::hash<std::string>{}(key.m_channelName);
        std::size_t streamHash = std::hash<std::string>{}(key.m_streamName);
        return channelHash ^ (streamHash << 1);
    }
};


class ElectrodeMap {
public:
    ElectrodeMap(int cols, int rows) : m_cols(cols), m_rows(rows) {}

    ElectrodeMap& withLayout(std::unordered_map<ElectrodeMapKey, int> layoutMapping) {
        m_layoutMapping = std::move(layoutMapping);
        return *this;
    }

    //returns dimension of map, columns (max x) and rows (max y)
    std::pair<int,int> getDimensions() const {
        return {m_cols, m_rows};
    }

    bool hasMap()  {
        return m_layoutMapping.size() > 0;
    }

    std::optional<int>  getChannelMapping(std::string channelName, std::string streamName) {
        ElectrodeMapKey key(channelName,streamName);
        auto mapping = m_layoutMapping.find(key);
        if(mapping != m_layoutMapping.end()) {
            return mapping -> second;
        }
        return std::nullopt;
    }

private:
    int m_cols;
    int m_rows;
    std::unordered_map<ElectrodeMapKey, int> m_layoutMapping;
};


#endif //CPP_ELECTRODEMAP_H
