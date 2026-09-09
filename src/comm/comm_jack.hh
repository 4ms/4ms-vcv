#pragma once
#include "util/edge_detector.hh"
#include <rack.hpp>

struct CommJack {
protected:
	rack::Port &port;
	unsigned idx;
	EdgeStateDetector plugDetect;

public:
	CommJack(rack::Port &port, unsigned idx)
		: port{port}
		, idx{idx} {
	}

	void updateInput() {
		plugDetect.update(port.isConnected());
	}

	unsigned getId() {
		return idx;
	}

	bool isConnected() {
		return port.isConnected();
	}

	unsigned getChannels() {
		return port.getChannels();
	}

	bool isJustPatched() {
		return plugDetect.went_high();
	}

	bool isJustUnpatched() {
		return plugDetect.went_low();
	}
};

struct CommInputJack : CommJack {
	CommInputJack(rack::Input &port, unsigned idx)
		: CommJack{port, idx} {
	}

	float getValue(unsigned channel = 0) {
		return port.getVoltage(channel);
	}
};

struct CommOutputJack : CommJack {
	CommOutputJack(rack::Output &port, unsigned idx)
		: CommJack{port, idx} {
	}

	void setValue(float val, unsigned channel = 0) {
		port.setVoltage(val, channel);
	}

	void setChannels(unsigned channels) {
		port.setChannels(channels);
	}
};
