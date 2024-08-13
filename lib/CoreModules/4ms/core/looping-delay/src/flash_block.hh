#include "flash.hh"
#include <cstdint>
#include <span>

// Wrapper reader/writer inside a flash block. The block is split into
// [cell_nr_] cells of equal size, each potentially containing an
// object of Data with specified Alignment (e.g. AlignmentBits == 3 means align to 8-bytes)
template<size_t FlashAddr, class DataT, size_t AlignmentBits = 2>
struct FlashBlock {
	using data_t = DataT;
	static constexpr unsigned size_ = 16 * 1024;
	static constexpr size_t data_size_ = sizeof(DataT);

	static constexpr size_t aligned_data_size_ = ((data_size_ >> AlignmentBits) + 1) << AlignmentBits;
	static_assert(aligned_data_size_ < size_);

	static constexpr int cell_nr_ = size_ / aligned_data_size_;

	bool read(data_t &data, int cell) {
		if (cell >= cell_nr_)
			return false;

		uint32_t addr = FlashAddr + cell * aligned_data_size_;

		// std::span<uint8_t> stored_data{reinterpret_cast<uint8_t *>(&data), data_size_};
		// return flash_read(stored_data, addr);

		std::span<uint32_t> stored_data{reinterpret_cast<uint32_t *>(&data), data_size_ / 4};
		return flash_read(stored_data, addr);
	}

	bool write(const data_t &data, int cell) {
		if (cell >= cell_nr_)
			return false;

		uint32_t addr = FlashAddr + cell * aligned_data_size_;
		std::span<const uint32_t> stored_data{reinterpret_cast<const uint32_t *>(&data), data_size_ / 4};
		return flash_write(stored_data, addr);
	}

	bool erase() {
		// Erase the entire block
		return flash_erase_sector(FlashAddr);
	}

	// Verify all bits are 1's
	bool is_writeable(int cell) {
		if (cell >= cell_nr_)
			return false;

		data_t check_data;

		if (read(check_data, cell)) {
			uint8_t *p = reinterpret_cast<uint8_t *>(&check_data);
			for (unsigned i = 0; i < data_size_; i++) {
				if (p[i] != 0xFF)
					return false;
			}
			return true;
		}
		return false;
	}

	// simple wrappers to read/write in 1st cell
	// bool read(data_t *data) { return read(data, 0); }
	// bool write(data_t *data) { return erase() && write(data, 0); }
};
