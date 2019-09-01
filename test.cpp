#include <memory>

std::unique_ptr<int> f(std::unique_ptr<int>&& a) {
	return std::move(a);
}

int main() {
	std::unique_ptr<int> a(new int(3));
	f(std::move(a));
	return *a;
}
