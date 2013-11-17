#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


#include "../sparsearray3.hpp"

TEST_CASE("SA3 - 1000 elements - iteration") {
	SparseArray3<int> arr;
	for(int i = 0; i < 1000; i++)
		arr.add(i);
	REQUIRE(arr.size() == 1000);
	REQUIRE_FALSE(arr.empty());

	SECTION("Simple forward iteration") {
		auto it = arr.begin();
		for(int i = 0; i < 1000; i++, it++)
			REQUIRE(*it == i);
	}

	SECTION("Remove every other element then iterate") {
		auto it = arr.begin();
		for(int i = 0; i < 1000; i++, it++) {
			REQUIRE(*it == i);
			if(i % 2 == 0)
				REQUIRE_NOTHROW(arr.remove(it));
		}

		it = arr.begin();
		for(int i = 1; i < 1000; i += 2, it++) {
			REQUIRE(*it == i);
		}
	}

	SECTION("Remove all") {
		auto it = arr.begin();
		for(int i = 0; i < 1000; i++, it++) {
			REQUIRE(*it == i);
			arr.remove(it);
		}
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE(arr.begin() == arr.end());
	}

	SECTION("Remove from middle and iterate") {
		auto it = arr.begin();
		for(int i = 0; i < 500; i++, it++)
			;
		arr.remove(it);
		REQUIRE(arr.size() == 999);

		it = arr.begin();
		for(int i = 0; i < 999; i++, it++) {
			REQUIRE(*it == (i < 500? i: i + 1));
		}
		REQUIRE(it == arr.end());
	}

	SECTION("Remove 2 from middle and iterate") {
		auto it = arr.begin();
		for(int i = 0; i < 500; i++, it++)
			;
		arr.remove(it);
		arr.remove(++it);
		REQUIRE(arr.size() == 998);

		it = arr.begin();
		for(int i = 0; i < 998; i++, it++) {
			REQUIRE(*it == (i < 500? i: i + 2));
		}
		REQUIRE(it == arr.end());
	}
}

TEST_CASE("SA3 - 1000 elements after removals") {
	SparseArray3<int> arr;
	for(int i = 0; i < 1000; i++) {
		arr.remove(arr.add(i));
		arr.add(i);
	}
	REQUIRE(arr.size() == 1000);
	REQUIRE_FALSE(arr.empty());

	SECTION("Simple forward iteration") {
		auto it = arr.begin();
		for(int i = 0; i < 1000; i++, it++)
			REQUIRE(*it == i);
	}

	SECTION("Remove every other element then iterate") {
		auto it = arr.begin();
		for(int i = 0; i < 1000; i++, it++) {
			REQUIRE(*it == i);
			if(i % 2 == 0)
				REQUIRE_NOTHROW(arr.remove(it));
		}

		it = arr.begin();
		for(int i = 1; i < 1000; i += 2, it++) {
			REQUIRE(*it == i);
		}
	}

	SECTION("Remove all") {
		auto it = arr.begin();
		for(int i = 0; i < 1000; i++, it++) {
			REQUIRE(*it == i);
			arr.remove(it);
		}
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE(arr.begin() == arr.end());
	}

	SECTION("Remove from middle and iterate") {
		auto it = arr.begin();
		for(int i = 0; i < 500; i++, it++)
			;
		arr.remove(it);
		REQUIRE(arr.size() == 999);

		it = arr.begin();
		for(int i = 0; i < 999; i++, it++) {
			REQUIRE(*it == (i < 500? i: i + 1));
		}
		REQUIRE(it == arr.end());
	}

	SECTION("Remove 2 from middle and iterate") {
		auto it = arr.begin();
		for(int i = 0; i < 500; i++, it++)
			;
		arr.remove(it);
		arr.remove(++it);
		REQUIRE(arr.size() == 998);

		it = arr.begin();
		for(int i = 0; i < 998; i++, it++) {
			REQUIRE(*it == (i < 500? i: i + 2));
		}
		REQUIRE(it == arr.end());
	}
}

TEST_CASE("SA3 - no elements") {
	SparseArray3<char> arr;

	REQUIRE(arr.size() == 0);
	REQUIRE(arr.empty());
	//REQUIRE_THROWS(arr[SparseArray<char>::Handle(0, 0)]);
	//REQUIRE_THROWS(arr.remove(SparseArray<char>::Handle(0, 0)));

	REQUIRE(arr.begin() == arr.end());
	REQUIRE_THROWS(*arr.begin());
	//REQUIRE_THROWS(arr.begin()++);

	SECTION("add->get") {
		auto handle = arr.add('a');
		REQUIRE(*handle == 'a');
		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
	}

	SECTION("add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE(arr.begin() == arr.end());
	}

	SECTION("2x add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE(arr.begin() == arr.end());
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE(arr.begin() == arr.end());
	}

	SECTION("add1->add2->remove1->remove2") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);
	}

	SECTION("add1->add2->remove2->remove1") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}
}

TEST_CASE("SA3 - one element") {
	SparseArray3<char> arr;
	auto handle = arr.add('a');

	REQUIRE(arr.size() == 1);
	REQUIRE_FALSE(arr.empty());
	//REQUIRE(arr[handle] == 'a');
	REQUIRE(*handle == 'a');

	REQUIRE(arr.begin() != arr.end());
	REQUIRE(++arr.begin() == arr.end());
	REQUIRE(*arr.begin() == 'a');

	SECTION("remove once") {
		arr.remove(handle);

		REQUIRE(arr.empty());
		REQUIRE(arr.size() == 0);
		//REQUIRE_THROWS(arr[handle]);
		REQUIRE_THROWS(*handle);
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

	SECTION("add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr.begin() != arr.end());
	}

	SECTION("2x add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr.begin() != arr.end());
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 1);
		REQUIRE_FALSE(arr.empty());
		REQUIRE(arr.begin() != arr.end());
	}
}

TEST_CASE("SA3 - two elements") {
	SparseArray3<char> arr;
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



#include "../sparsearray2.hpp"

TEST_CASE("SA2 - no elements") {
	SparseArray2<char> arr;

	REQUIRE(arr.size() == 0);
	REQUIRE(arr.empty());
	//REQUIRE_THROWS(arr[SparseArray<char>::Handle(0, 0)]);
	//REQUIRE_THROWS(arr.remove(SparseArray<char>::Handle(0, 0)));

	//REQUIRE(arr.begin() == arr.end());
	REQUIRE_THROWS(*arr.begin());
	//REQUIRE_THROWS(arr.begin()++);

	SECTION("add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
	}

	SECTION("2x add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
	}

	SECTION("add1->add2->remove1->remove2") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);
	}

	SECTION("add1->add2->remove2->remove1") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}
}

TEST_CASE("SA2 - one element") {
	SparseArray2<char> arr;
	auto handle = arr.add('a');

	REQUIRE(arr.size() == 1);
	REQUIRE_FALSE(arr.empty());
	REQUIRE(arr[handle] == 'a');

	REQUIRE(arr.begin() != arr.end());
	REQUIRE(++arr.begin() == arr.end());
	REQUIRE(*arr.begin() == 'a');

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

	SECTION("add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
	}

	SECTION("2x add->remove") {
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
		REQUIRE_NOTHROW(arr.remove(arr.add('a')));
		REQUIRE(arr.size() == 0);
		REQUIRE(arr.empty());
	}

	SECTION("add1->add2->remove1->remove2") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);
	}

	SECTION("add1->add2->remove2->remove1") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}

	SECTION("multiple add/remove") {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}
}

TEST_CASE("Sparse Array - one element") {
	SparseArray<char> arr;
	auto handle = arr.add('a');

	REQUIRE(arr.size() == 1);
	REQUIRE_FALSE(arr.empty());
	REQUIRE(arr[handle] == 'a');

	REQUIRE(arr.begin() != arr.end());
	REQUIRE(++arr.begin() == arr.end());
	REQUIRE(*arr.begin() == 'a');

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

	REQUIRE(arr.begin() != arr.end());
	REQUIRE(++arr.begin() != arr.end());
	REQUIRE(*arr.begin() == 'a');
	REQUIRE(*(++arr.begin()) == 'b');

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
