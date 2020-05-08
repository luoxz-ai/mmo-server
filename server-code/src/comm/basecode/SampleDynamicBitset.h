#ifndef SAMPLEDYNAMICBITSET_H
#define SAMPLEDYNAMICBITSET_H

#include <map>

class SampleDynamicBitset
{
public:
    using bit_index_t = uint32_t;
    using block_index_t = uint32_t;
    using block_t = uint32_t;
    static constexpr size_t bit_per_block = 32;
    

    static constexpr block_t block_mask(bit_index_t bit_idx)
    {
        return ((block_t)1) << (bit_idx%bit_per_block);
    }

    static constexpr block_index_t block_pos(bit_index_t bit_idx)
    {
        return bit_idx/bit_per_block;
    }

    static constexpr size_t block_count(const block_t& block)
    {
        size_t count = 0;
        block_t mask  = 1;
        for(size_t bit_index = 0; bit_index < bit_per_block; ++bit_index)
        {
            if((block & mask) != 0)
            {
                count++;
            }
            mask <<= 1;
        }
        return count;
    }

public:
    bool test(bit_index_t bit_idx) const 
    {
        block_index_t block_index = block_pos(bit_idx);
        auto it = m_DataMap.find(block_index);
        if(it == m_DataMap.end())
        {
            return false;
        }

        return (it->second & block_mask(bit_idx)) != 0;
    }

    void set(bit_index_t bit_idx, bool val = true) 
    {
        block_index_t block_index = block_pos(bit_idx);
        auto& block = m_DataMap[block_index];
        if(val)
            block |= block_mask(bit_idx);
        else
            block &= ~(block_mask(bit_idx));
    }

    void flip(bit_index_t bit_idx)
    {
        block_index_t block_index = block_pos(bit_idx);
        auto& block = m_DataMap[block_index];
        block ^= (block_mask(bit_idx));
    }

    bool any() const
    {
        for(const auto& [k,v] : m_DataMap)
        {
            if(v != 0)
            {
                return true;
            }
        }
        return false;
    }

    bool none() const
    {
        return !any();
    }
    
    size_t count() const
    {
        size_t count = 0;

        for(const auto& [k,v] : m_DataMap)
        {
            count += block_count(v);
        }
        return count;
    }



public:
    void clear()
    {
        m_DataMap.clear();
    }

    size_t size() const
    {
        return m_DataMap.size();
    }

    bool empty() const
    {
        return m_DataMap.empty();
    }

    void shrink_to_fit()
    {
        for(auto it = m_DataMap.begin(); it != m_DataMap.end();)
        {
            if(it->second == 0)
            {
                it = m_DataMap.erase(it);
            }
            else
            {
                it++;
            }
            
        }
    }

private:
    std::map<block_index_t, block_t> m_DataMap;
};

#endif /* SAMPLEDYNAMICBITSET_H */
