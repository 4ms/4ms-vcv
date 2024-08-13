// Copyright 2016 Matthias Puech.
//
// Author: Matthias Puech <matthias.puech@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Persistent data

#pragma once
#include <array>
#include <span>
#include <vector>
#include "parameters.hh"
#include "resources.h"

namespace TapoDelay {

const int kNumbBanks = 4;
const int kSlotsPerBank = 6;
const int kNumSlots = kSlotsPerBank * kNumbBanks;    // 6 buttons, 4 banks

struct CalibrationData {
  float offset[4];
};

class Persistent 
{
public:
  struct Data {
    uint8_t velocity_parameter;
    uint8_t current_bank;
    uint8_t panning_mode;
    uint8_t sequencer_mode;
    uint8_t current_slot;
    uint8_t repeat;
    uint8_t sync;
    uint8_t padding[1];
    CalibrationData calibration_data;
  };

  void Init(size_t buffer_size) {

    for (size_t i=0; i<4; i++) {
      data_.calibration_data.offset[i] = 0.5f;
    }
    // default settings:
    data_.velocity_parameter = 2;
    data_.current_bank = 0;
    data_.panning_mode = 1;
    data_.sequencer_mode = 0;
    data_.current_slot = 0;
    data_.repeat = 0;
    data_.sync = 0;
    SaveData();

    // sanitize settings
    CONSTRAIN(data_.velocity_parameter, 0, 4);
    CONSTRAIN(data_.current_bank, 0, 3);
    CONSTRAIN(data_.panning_mode, 0, 2);
    CONSTRAIN(data_.sequencer_mode, 0, 1);
    CONSTRAIN(data_.current_slot, 0, 6 * 4);
    if (data_.repeat != 1) data_.repeat = 0;
    if (data_.sync != 1) data_.sync = 0;

    // sanitize slots
    for (int slot=0; slot<kNumSlots; slot++) {
      CONSTRAIN(slots_[slot].size, 0, kMaxTaps);
      for (int tap=0; tap<slots_[slot].size; tap++) {
        TapParameters *t = &slots_[slot].taps[tap];
        CONSTRAIN(t->velocity, 0.0f, 1.0f);
        CONSTRAIN(t->time, 0.0f, buffer_size);
        CONSTRAIN(t->velocity_type, VELOCITY_AMP, VELOCITY_BP);
        CONSTRAIN(t->panning, 0.0f, 1.0f);
      }
    }
  }

  void SaveData() {
  }

  Data* mutable_data() { return &data_; }

  // Mark the slot to be saved
  void SaveSlot(int slot_nr) {
    slot_is_customized_[slot_nr] = true;
  }

  // Restore slot to factory default
  void ResetSlot(int slot) {
    slots_[slot].size = lut_preset_sizes[slot];

    for (int tap=0; tap<slots_[slot].size; tap++) {
      int index = tap + kMaxTaps * slot;
      TapParameters *t = &slots_[slot].taps[tap];
      t->time = lut_preset_times[index];
      t->velocity = lut_preset_velos[index];
      t->velocity_type = static_cast<VelocityType>(lut_preset_types[index]);
      t->panning = lut_preset_pans[index];
    }
    slot_is_customized_[slot] = false;
  }

  // Restore all slots in a bank to factory default
  void ResetBank(int bank) {
    for(int slot=bank*6; slot<(bank+1)*6; slot++)
      ResetSlot(slot);
  }

  uint8_t current_bank() { return data_.current_bank; }
  uint8_t current_slot() { return data_.current_slot; }
  uint8_t velocity_parameter() { return data_.velocity_parameter; };
  uint8_t panning_mode() { return data_.panning_mode; };
  uint8_t sequencer_mode() { return data_.sequencer_mode; };
  uint8_t repeat() { return data_.repeat; };
  uint8_t sync() { return data_.sync; };

  void ResetCurrentBank() { ResetBank(current_bank()); }

  Slot* mutable_slot(int nr) { return &slots_[nr]; }

  struct SavedSlot{
    uint32_t index;
    Slot contents;
  };

  void load_custom_slots(std::span<const SavedSlot> saved_slots) {
    for (auto const &slot : saved_slots) {
      if (slot.index < kNumSlots) {
        slots_[slot.index] = slot.contents;
        slot_is_customized_[slot.index] = true;
      }
    }
  }

  std::vector<SavedSlot> get_custom_slots() {
    std::vector<SavedSlot> saved_slots;

    for (auto slot_i = 0u; auto custom : slot_is_customized_) {
      if (custom) {
        saved_slots.push_back({slot_i, slots_[slot_i]});
      }
      slot_i++;
    }
    return saved_slots;
  }

private:
  Data data_{};
  std::array<Slot, kNumSlots> slots_{};
  std::array<bool, kNumSlots> slot_is_customized_{};
};

}

