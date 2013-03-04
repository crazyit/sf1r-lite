#ifndef SF1R_DISTRIBUTE_REQUEST_HOOKER_H
#define SF1R_DISTRIBUTE_REQUEST_HOOKER_H

#include "RequestLog.h"
#include <string>
#include <configuration-manager/CollectionPath.h>
#include <util/singleton.h>
#include <set>

namespace sf1r
{
class ReqLogMgr;
class DistributeRequestHooker
{
public:
    static DistributeRequestHooker* get()
    {
        return izenelib::util::Singleton<DistributeRequestHooker>::get();
    }

    enum ChainStatus
    {
        NoChain = 0,
        ChainBegin,
        ChainMiddle,
        ChainEnd,
        ChainStop,
        Unknown
    };

    DistributeRequestHooker();
    void init();
    bool isValid();
    void hookCurrentReq(const std::string& reqdata);
    bool isHooked();
    void setHook(int calltype, const std::string& addition_data);
    int  getHookType();

    inline bool setChainStatus(ChainStatus status)
    {
        if (!isHooked())
            return true;
        if (chain_status_ > status)
        {
            // chain request can only handle from begin to end.
            // you can not set status to an older status. 
            return false;
        }
        chain_status_ = status;
        return true;
    }
    ChainStatus getChainStatus()
    {
        return chain_status_;
    }
    bool readPrevChainData(CommonReqData& reqlogdata);

    void clearHook(bool force = false);
    const std::string& getAdditionData()
    {
        return current_req_;
    }

    bool prepare(ReqLogType type, CommonReqData& prepared_req);
    void processLocalBegin();
    void processLocalFinished(bool finishsuccess);
    void processLocalFinished(bool finishsuccess, CommonReqData& updated_preparedata);
    bool processFinishedBeforePrepare(bool finishsuccess);
    bool onRequestFromPrimary(int type, const std::string& packed_reqdata);

    void waitReplicasProcessCallback();
    void waitPrimaryCallback();

    void abortRequest();
    void abortRequestCallback();
    void waitReplicasAbortCallback();

    void writeLocalLog();
    void waitReplicasLogCallback();

    void onElectingFinished();

private:
    static bool isNeedBackup(ReqLogType type);
    void finish(bool success);
    void forceExit();

    //std::string colname_;
    //CollectionPath colpath_;
    // for primary worker, this is raw json request data.
    // for replica worker, this is packed request data with addition data from primary.
    std::string current_req_;
    ReqLogType type_;
    boost::shared_ptr<ReqLogMgr> req_log_mgr_;
    int hook_type_;
    ChainStatus chain_status_;
    static std::set<ReqLogType> need_backup_types_;

};

class DistributeWriteGuard
{
public:
    DistributeWriteGuard();
    ~DistributeWriteGuard();
    void setSuccess();
    bool isValid();
private:
    bool result_;
};

#define DISTRIBUTE_WRITE_BEGIN DistributeWriteGuard distribute_write_guard;
#define DISTRIBUTE_WRITE_CHECK_VALID_RETURN  if (!distribute_write_guard.isValid()) { LOG(ERROR) << __FUNCTION__ << " call invalid."; return false; }
#define DISTRIBUTE_WRITE_CHECK_VALID_RETURN2  if (!distribute_write_guard.isValid()) { LOG(ERROR) << __FUNCTION__ << " call invalid."; return; }
#define DISTRIBUTE_WRITE_SUCCESS  distribute_write_guard.setSuccess();

}

#endif
