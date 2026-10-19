#include "doctest.h"
#include "hub/rtti_derives.hh"

namespace
{
struct Base {
	virtual ~Base() = default;
};
struct Other {
	virtual ~Other() = default;
};
struct Single : Base {};
struct Grandchild : Single {};
struct Multiple : Other, Single {};
struct Unrelated {
	virtual ~Unrelated() = default;
};
} // namespace

TEST_CASE("derivesFrom walks base classes by name") {
	using MetaModule::derivesFrom;

	CHECK(derivesFrom(typeid(Base), typeid(Base)));
	CHECK(derivesFrom(typeid(Single), typeid(Base)));
	CHECK(derivesFrom(typeid(Grandchild), typeid(Base)));
	CHECK(derivesFrom(typeid(Multiple), typeid(Base)));
	CHECK(derivesFrom(typeid(Multiple), typeid(Other)));

	CHECK_FALSE(derivesFrom(typeid(Base), typeid(Single)));
	CHECK_FALSE(derivesFrom(typeid(Unrelated), typeid(Base)));

	Base *obj = new Grandchild;
	CHECK(derivesFrom(typeid(*obj), typeid(Single)));
	delete obj;
}
