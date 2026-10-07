#include <generator>
#include <print>

using std::generator;
using std::print;

generator<int> fibonacci(int num_terms) {
	int fn = 0, fn1 = 1;
	for (; num_terms > 0; num_terms -= 1) {
		co_yield fn;
		fn1 = fn + fn1;
		fn = fn1 - fn;
	}
}

int main() {
	generator<int> fib = fibonacci(10);
    for (int f : fib) {
        print("f = {0}\n", f);
    }
}

