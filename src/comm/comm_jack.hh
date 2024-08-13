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

	float getId() {
		return idx;
	}

	bool isConnected() {
		return port.isConnected();
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

	float getValue() {
		return port.getVoltage();
	}
};

struct CommOutputJack : CommJack {
	CommOutputJack(rack::Output &port, unsigned idx)
		: CommJack{port, idx} {
	}

	void setValue(float val) {
		return port.setVoltage(val);
	}
};
