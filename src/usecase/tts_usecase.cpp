#include "tts_usecase.h"
#include "../infrastructure/rhvoice_synthesizer.h"
#include <queue>
#include <thread>
#include <memory>

struct TTSJob {
    TTSRequest request;
    std::promise <std::vector<uint8_t>> result;
};

class TTSUseCase : public ITTSUseCase {
public:
    explicit TTSUseCase(size_t worker_count) {
        for (size_t i = 0; i < worker_count; ++i) {
            workers_.emplace_back(&TTSUseCase::WorkerLoop, this);
        }
    }

    ~TTSUseCase() {
        running_ = false;
        cv_.notify_all();
        for (auto &t: workers_) if (t.joinable()) t.join();
    }

    std::future <std::vector<uint8_t>> ProcessRequest(const TTSRequest &request) override {
        auto job = std::make_shared<TTSJob>();
        job->request = request;
        auto fut = job->result.get_future();

        {
            std::lock_guard <std::mutex> lock(mutex_);
            queue_.push(job);
        }
        cv_.notify_one();
        return fut;
    }

private:
    void WorkerLoop() {
        RHVoiceSynthesizer synth;
        while (running_) {
            std::shared_ptr <TTSJob> job;
            {
                std::unique_lock <std::mutex> lock(mutex_);
                cv_.wait(lock, [&]() { return !queue_.empty() || !running_; });
                if (!running_ && queue_.empty()) return;
                job = queue_.front();
                queue_.pop();
            }
            try {
                auto data = synth.Synthesize(job->request);
                job->result.set_value(std::move(data));
            } catch (...) {
                job->result.set_exception(std::current_exception());
            }
        }
    }

    std::vector <std::thread> workers_;
    std::queue <std::shared_ptr<TTSJob>> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{true};
};