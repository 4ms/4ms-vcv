#include "doctest.h"
#include "mapping/range_value_parser.hh"

TEST_CASE("normalizeRangeValueExpression removes percent signs") {
	CHECK(MetaModule::normalizeRangeValueExpression("50%") == "50");
	CHECK(MetaModule::normalizeRangeValueExpression("100/7%") == "100/7");
	CHECK(MetaModule::normalizeRangeValueExpression(" 50 % ") == " 50  ");
	CHECK(MetaModule::normalizeRangeValueExpression("25") == "25");
}
