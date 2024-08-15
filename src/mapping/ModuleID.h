#pragma once

#include "patch/module_type_slug.hh"

#include <cstdint>

struct ModuleID {
	int64_t id;
	ModuleTypeSlug slug;

	bool operator==(const ModuleID &rhs) const {
		return (this->id == rhs.id) && (this->slug == rhs.slug);
	}
};

struct BrandModule {
	int64_t id;
	BrandModuleSlug slug;
};
