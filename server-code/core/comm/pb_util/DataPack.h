#ifndef DATAPACK_H
#define DATAPACK_H

#include "datapack/CfgDataPack.pb.h"
#include "ProtobuffUtil.h"
namespace DataPack
{
    template<class cfg_message_t>
    bool LoadFromBinaryFile(const std::string& filename, std::vector<cfg_message_t>& vecData)
    {
        CfgDataPack input;
        if(pb_util::LoadFromBinaryFile(filename, input) == false)
        {
            LOGERROR("InitFromFile {} Fail.", filename);
            return false;
        }

        for(const auto& data: input.data_set())
        {
            cfg_message_t cfg;
            cfg.ParseFromString(data);

            vecData.emplace_back(std::move(cfg));
        }
        return true;
    }

    template<class cfg_message_t>
    bool SaveToBinaryFile(const std::vector<cfg_message_t>& vecData, const std::string& filename)
    {
        CfgDataPack output;
        output.set_size(vecData.size());
        for(const cfg_message_t& row: vecData)
        {
            auto data = output.add_data_set();
            row.SerializeToString(data);
        }

        return pb_util::SaveToBinaryFile(output, filename.c_str());
    }
} // namespace DataPack

#endif // DATAPACK_H