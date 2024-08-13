namespace std
{
void __throw_bad_function_call() {
	return;
	// while (1)
	// 	;
}
void __throw_bad_alloc() {
	while (true)
		;
}
void __throw_length_error(char const *) {
	while (true)
		;
}

//Needed for std::string constructor and function to return std::string
void __throw_logic_error(char const *) {
	while (true)
		;
}

void __throw_out_of_range_fmt(char const *, ...) {
}

//Needed to compile at -O0 when using std::string
template<typename T>
struct allocator {
	allocator();
	allocator(const std::allocator<char> &);
	~allocator();
};
template<>
allocator<char>::allocator() = default;
template<>
allocator<char>::~allocator() = default;
template<>
allocator<char>::allocator(const std::allocator<char> &) = default;

} // namespace std

extern "C" void __cxa_pure_virtual() {
	while (true)
		;
}
