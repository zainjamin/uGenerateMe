#include <coroutine>
#include <exception>
#include <string>
#include <print>

template<typename T> struct input_generator {
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;
	handle_type handle;
	
	struct await_input{};
	struct promise_type {
		T in;
		
		input_generator<T> get_return_object() {
			return input_generator<T>{
				.handle = handle_type::from_promise(*this)
			};
		}
		std::suspend_never initial_suspend() const noexcept { return {}; }
		std::suspend_always final_suspend() const noexcept { return {}; }
		
        void return_void() const noexcept {}
		void unhandled_exception() const noexcept { std::terminate(); }
		
		auto await_transform(await_input) noexcept {
			struct awaiter {
				promise_type& promise;
				bool await_ready() const noexcept { return false; }
				void await_suspend(handle_type) noexcept {}
				T await_resume() const noexcept {
					return promise.in;
				}
			};
			return awaiter{*this};
		}
	};

	~input_generator() {
        std::print("\n");
		handle.destroy();
	}
	
	void send(T in) {
		handle.promise().in = in;
		handle.resume();
	}	
};

input_generator<char> format() {
	for (;;) {
		for (int g = 0; g < 5; g += 1) {
			for (int b = 0; b < 4; b += 1) {
				char ch;
				do {
					ch = co_await input_generator<char>::await_input{};
				} while (ch == '\n');
				std::print("{}", ch);
			}
			std::print(" ");
		}
		std::print("\n");
	}
}

int main() {
	std::string stream = "abcdefghijklmnopqrstuvwxyz\nabcdefghijklmnopqrstuvwxyz";
	input_generator<char> fmt = format();
	for (char c : stream) {
		fmt.send(c);
	}
}

