#pragma once

#include <libnuraft/nuraft.hxx> // Y_IGNORE
#include <Coordination/InMemoryLogStore.h>
#include <Coordination/KeeperStateManager.h>
#include <Coordination/KeeperStateMachine.h>
#include <Coordination/KeeperStorage.h>
#include <Coordination/CoordinationSettings.h>
#include <unordered_map>

namespace DB
{

class KeeperServer
{
private:
    int server_id;

    CoordinationSettingsPtr coordination_settings;

    nuraft::ptr<KeeperStateMachine> state_machine;

    nuraft::ptr<KeeperStateManager> state_manager;

    nuraft::raft_launcher launcher;

    nuraft::ptr<nuraft::raft_server> raft_instance;

    std::mutex append_entries_mutex;

    ResponsesQueue & responses_queue;

    std::mutex initialized_mutex;
    std::atomic<bool> initialized_flag = false;
    std::condition_variable initialized_cv;
    std::atomic<bool> initial_batch_committed = false;

    nuraft::cb_func::ReturnCode callbackFunc(nuraft::cb_func::Type type, nuraft::cb_func::Param * param);

public:
    KeeperServer(
        int server_id_,
        const CoordinationSettingsPtr & coordination_settings_,
        const Poco::Util::AbstractConfiguration & config,
        ResponsesQueue & responses_queue_,
        SnapshotsQueue & snapshots_queue_);

    void startup();

    void putRequest(const KeeperStorage::RequestForSession & request);

    int64_t getSessionID(int64_t session_timeout_ms);

    std::unordered_set<int64_t> getDeadSessions();

    bool isLeader() const;

    bool isLeaderAlive() const;

    void waitInit();

    void shutdown();
};

}
