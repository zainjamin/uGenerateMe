#include <coroutine>
#include <print>
#include <assert.h>

struct PingPong {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type handle;

    struct resume_partner{};
    struct promise_type {
        PingPong* partner;

        PingPong get_return_object() noexcept {
            return PingPong {
                .handle = handle_type::from_promise(*this),
            };
        }

        std::suspend_always initial_suspend() const noexcept { return {}; }
        std::suspend_always final_suspend() const noexcept { return {}; }
        void unhandled_exception() const noexcept { std::terminate(); }
        void return_void() {}

        auto await_transform(resume_partner) {
            struct awaiter {
                PingPong* partner;
                bool await_ready() const noexcept { return false; }
                handle_type await_suspend(handle_type) const noexcept {
                    assert(partner->handle);
                    return partner->handle;
                }
                void await_resume() const noexcept {}
            };
            return awaiter{partner};
        }
    };

    ~PingPong() {
        handle.destroy();
    }

    void set_partner(PingPong* partner) {
        handle.promise().partner = partner;
    }

    void resume() {
        handle.resume();
    }
};

PingPong ping_pong(const char* name, const int N) {
    for (int i = 0; i < N; i += 1) {
        std::println("{} {}", name, i);
        co_await PingPong::resume_partner{};
    }
}

int main() {
    constexpr int N = 5;
    PingPong ping = ping_pong("ping", N), pong = ping_pong("pong", N);
    ping.set_partner(&pong);
    pong.set_partner(&ping);

    ping.resume();
}

