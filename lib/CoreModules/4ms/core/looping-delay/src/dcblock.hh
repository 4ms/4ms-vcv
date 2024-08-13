
template<unsigned CoefRecip, typename T>
class DCBlock {
	static constexpr float Coef = 1.f / CoefRecip;
	float lpf = 0.f;

public:
	DCBlock() = default;

	T update(T cur) {
		lpf += Coef * (cur - lpf);
		T hpf = cur - lpf;
		return hpf;
	}
};
