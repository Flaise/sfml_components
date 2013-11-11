#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


#include "../sparsearray2.hpp"

TEST_CASE("SA2 - no elements") {
	SparseArray2<char> arr;

	REQUIRE(arr.size() == 0);
	REQUIRE(arr.empty());
	//REQUIRE_THROWS(arr[SparseArray<char>::Handle(0, 0)]);
	//REQUIRE_THROWS(arr.remove(SparseArray<char>::Handle(0, 0)));

	//REQUIRE(arr.begin() == arr.end());
}

TEST_CASE("SA2 - one element") {
	SparseArray2<char> arr;
	auto handle = arr.add('a');

	REQUIRE(arr.size() == 1);
	REQUIRE_FALSE(arr.empty());
	REQUIRE(arr[handle] == 'a');

	SECTION("remove once") {
		arr.remove(handle);

		REQUIRE(arr.empty());
		REQUIRE(arr.size() == 0);
		REQUIRE_THROWS(arr[handle]);
	}

	SECTION("remove twice to throw") {
		arr.remove(handle);
		REQUIRE_THROWS(arr.remove(handle));
	}

	SECTION("remove then add") {
		arr.remove(handle);
		auto handle2 = arr.add('b');

		REQUIRE(arr[handle2] == 'b');
		REQUIRE_THROWS(arr[handle]);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr.size() == 1);
	}
}

TEST_CASE("SA2 - two elements") {
	SparseArray2<char> arr;
	auto handle1 = arr.add('a');
	auto handle2 = arr.add('b');

	REQUIRE(arr.size() == 2);
	REQUIRE_FALSE(arr.empty());
	REQUIRE(arr[handle1] == 'a');
	REQUIRE(arr[handle2] == 'b');

	SECTION("remove first") {
		arr.remove(handle1);

		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr[handle2] == 'b');
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE_THROWS(arr.remove(handle1));
	}

	SECTION("remove second") {
		arr.remove(handle2);

		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr[handle1] == 'a');
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE_THROWS(arr.remove(handle2));
	}

	SECTION("remove both") {
		arr.remove(handle1);
		arr.remove(handle2);

		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE_THROWS(arr.remove(handle1));
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE_THROWS(arr.remove(handle2));
	}

	SECTION("remove first then add") {
		arr.remove(handle1);
		auto handle3 = arr.add('c');

		REQUIRE(arr.size() == 2);
		REQUIRE_FALSE(arr.empty());
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE(arr[handle2] == 'b');
		REQUIRE(arr[handle3] == 'c');
	}

	SECTION("remove second then add") {
		arr.remove(handle2);
		auto handle3 = arr.add('c');

		REQUIRE(arr.size() == 2);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr[handle1] == 'a');
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE(arr[handle3] == 'c');
	}

	SECTION("remove both then add") {
		arr.remove(handle1);
		arr.remove(handle2);
		auto handle3 = arr.add('c');

		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE(arr[handle3] == 'c');
	}
}



#include "../sparsearray.hpp"

TEST_CASE("Sparse Array - no elements") {
	SparseArray<char> arr;

	REQUIRE(arr.size() == 0);
	REQUIRE(arr.empty());
	REQUIRE_THROWS(arr[SparseArray<char>::Handle(0, 0)]);
	REQUIRE_THROWS(arr.remove(SparseArray<char>::Handle(0, 0)));

	REQUIRE(arr.begin() == arr.end());
}

TEST_CASE("Sparse Array - one element") {
	SparseArray<char> arr;
	auto handle = arr.add('a');

	REQUIRE(arr.size() == 1);
	REQUIRE_FALSE(arr.empty());
	REQUIRE(arr[handle] == 'a');

	SECTION("remove once") {
		arr.remove(handle);

		REQUIRE(arr.empty());
		REQUIRE(arr.size() == 0);
		REQUIRE_THROWS(arr[handle]);
	}

	SECTION("remove twice to throw") {
		arr.remove(handle);
		REQUIRE_THROWS(arr.remove(handle));
	}

	SECTION("remove then add") {
		arr.remove(handle);
		auto handle2 = arr.add('b');

		REQUIRE(arr[handle2] == 'b');
		REQUIRE_THROWS(arr[handle]);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr.size() == 1);
	}
}

TEST_CASE("Sparse Array - two elements") {
	SparseArray<char> arr;
	auto handle1 = arr.add('a');
	auto handle2 = arr.add('b');

	REQUIRE(arr.size() == 2);
	REQUIRE_FALSE(arr.empty());
	REQUIRE(arr[handle1] == 'a');
	REQUIRE(arr[handle2] == 'b');

	SECTION("remove first") {
		arr.remove(handle1);

		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr[handle2] == 'b');
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE_THROWS(arr.remove(handle1));
	}

	SECTION("remove second") {
		arr.remove(handle2);

		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr[handle1] == 'a');
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE_THROWS(arr.remove(handle2));
	}

	SECTION("remove both") {
		arr.remove(handle1);
		arr.remove(handle2);

		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE_THROWS(arr.remove(handle1));
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE_THROWS(arr.remove(handle2));
	}

	SECTION("remove first then add") {
		arr.remove(handle1);
		auto handle3 = arr.add('c');

		REQUIRE(arr.size() == 2);
		REQUIRE_FALSE(arr.empty());
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE(arr[handle2] == 'b');
		REQUIRE(arr[handle3] == 'c');
	}

	SECTION("remove second then add") {
		arr.remove(handle2);
		auto handle3 = arr.add('c');

		REQUIRE(arr.size() == 2);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr[handle1] == 'a');
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE(arr[handle3] == 'c');
	}

	SECTION("remove both then add") {
		arr.remove(handle1);
		arr.remove(handle2);
		auto handle3 = arr.add('c');

		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE_THROWS(arr[handle1]);
		REQUIRE_THROWS(arr[handle2]);
		REQUIRE(arr[handle3] == 'c');
	}
}
