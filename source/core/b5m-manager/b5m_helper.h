#ifndef SF1R_B5MMANAGER_B5MHELPER_H_
#define SF1R_B5MMANAGER_B5MHELPER_H_

#include <string>
#include <vector>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <common/ScdParser.h>
#include <common/Utilities.h>

namespace sf1r {
    class B5MHelper {
    public:
        static const std::vector<std::string> B5M_PROPERTY_LIST;
        static const std::vector<std::string> B5MO_PROPERTY_LIST;
        static const std::vector<std::string> B5MP_PROPERTY_LIST;
        static const std::vector<std::string> B5MC_PROPERTY_LIST;

        static uint128_t StringToUint128(const std::string& str)
        {
            return Utilities::md5ToUint128(str);
        }

        static std::string Uint128ToString(uint128_t u)
        {
            return Utilities::uint128ToMD5(u);
        }

        static void GetScdList(const std::string& scd_path, std::vector<std::string>& scd_list)
        {
            namespace bfs = boost::filesystem;
            if(!bfs::exists(scd_path)) return;
            if( bfs::is_regular_file(scd_path) && ScdParser::checkSCDFormat(scd_path))
            {
                scd_list.push_back(scd_path);
            }
            else if(bfs::is_directory(scd_path))
            {
                bfs::path p(scd_path);
                bfs::directory_iterator end;
                for(bfs::directory_iterator it(p);it!=end;it++)
                {
                    if(bfs::is_regular_file(it->path()))
                    {
                        std::string file = it->path().string();
                        if(ScdParser::checkSCDFormat(file))
                        {
                            scd_list.push_back(file);
                        }
                    }
                }
            }
            std::sort(scd_list.begin(), scd_list.end(), ScdParser::compareSCD);
        }
    };

}

#endif

