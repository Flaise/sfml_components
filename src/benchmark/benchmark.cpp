#include <iostream>
#include <cstdlib>
#include <time.h>

#include "../sparsearray.hpp"
#include "../sparsearray2.hpp"
#include "../sparsearray3.hpp"

const int iterations = 30000000;

clock_t sa_singleAddRemove() {
	clock_t start = clock();

	SparseArray<char> arr;
	for(int i = 0; i < iterations; i++) {
		arr.remove(arr.add('a'));
	}

	clock_t stop = clock();
	return stop - start;
}
clock_t sa2_singleAddRemove() {
	clock_t start = clock();

	SparseArray2<char> arr;
	for(int i = 0; i < iterations; i++) {
		arr.remove(arr.add('a'));
	}

	clock_t stop = clock();
	return stop - start;
}
clock_t sa3_singleAddRemove() {
	clock_t start = clock();

	SparseArray3<char> arr;
	for(int i = 0; i < iterations; i++) {
		arr.remove(arr.add('a'));
	}

	clock_t stop = clock();
	return stop - start;
}

clock_t sa_doubleAddRemove() {
	clock_t start = clock();

	SparseArray<char> arr;
	for(int i = 0; i < iterations; i++) {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}

	clock_t stop = clock();
	return stop - start;
}
clock_t sa2_doubleAddRemove() {
	clock_t start = clock();

	SparseArray2<char> arr;
	for(int i = 0; i < iterations; i++) {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}

	clock_t stop = clock();
	return stop - start;
}
clock_t sa3_doubleAddRemove() {
	clock_t start = clock();

	SparseArray3<char> arr;
	for(int i = 0; i < iterations; i++) {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		arr.remove(handle1);
		arr.remove(handle2);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		arr.remove(handle2);
		arr.remove(handle1);
	}

	clock_t stop = clock();
	return stop - start;
}

clock_t sa_tripleAddRemove() {
	clock_t start = clock();

	SparseArray<char> arr;
	for(int i = 0; i < iterations; i++) {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		auto handle3 = arr.add('c');
		arr.remove(handle1);
		arr.remove(handle2);
		arr.remove(handle3);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		handle3 = arr.add('c');
		arr.remove(handle3);
		arr.remove(handle2);
		arr.remove(handle1);
	}

	clock_t stop = clock();
	return stop - start;
}
clock_t sa2_tripleAddRemove() {
	clock_t start = clock();

	SparseArray2<char> arr;
	for(int i = 0; i < iterations; i++) {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		auto handle3 = arr.add('c');
		arr.remove(handle1);
		arr.remove(handle2);
		arr.remove(handle3);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		handle3 = arr.add('c');
		arr.remove(handle3);
		arr.remove(handle2);
		arr.remove(handle1);
	}

	clock_t stop = clock();
	return stop - start;
}
clock_t sa3_tripleAddRemove() {
	clock_t start = clock();

	SparseArray3<char> arr;
	for(int i = 0; i < iterations; i++) {
		auto handle1 = arr.add('a');
		auto handle2 = arr.add('b');
		auto handle3 = arr.add('c');
		arr.remove(handle1);
		arr.remove(handle2);
		arr.remove(handle3);

		handle1 = arr.add('a');
		handle2 = arr.add('b');
		handle3 = arr.add('c');
		arr.remove(handle3);
		arr.remove(handle2);
		arr.remove(handle1);
	}

	clock_t stop = clock();
	return stop - start;
}

clock_t sa_iteration() {
	srand(0);

	clock_t accumulator = 0;
	int valueAccumulator = 0;

	for(int i = 0; i < 20; i++) {
		SparseArray<int> arr;
		for(int j = 0; j <= 1000; j++)
			if(rand() % 3 == 0)
				arr.remove(arr.add(j));
			else
				arr.add(j);

		clock_t start = clock();
		for(int j = 0; j < 1000; j++)
			for(auto r = arr.begin(); r != arr.end(); r++)
				valueAccumulator += *r;
		clock_t stop = clock();
		accumulator += stop - start;
	}

	return accumulator;
}
clock_t sa2_iteration() {
	srand(0);

	clock_t accumulator = 0;
	int valueAccumulator = 0;

	for(int i = 0; i < 20; i++) {
		SparseArray2<int> arr;
		for(int j = 0; j <= 1000; j++)
			if(rand() % 3 == 0)
				arr.remove(arr.add(j));
			else
				arr.add(j);

		clock_t start = clock();
		for(int j = 0; j < 1000; j++)
			for(auto r = arr.begin(); r != arr.end(); r++)
				valueAccumulator += *r;
		clock_t stop = clock();
		accumulator += stop - start;
	}

	return accumulator;
}
clock_t sa3_iteration() {
	srand(0);

	clock_t accumulator = 0;
	int valueAccumulator = 0;

	for(int i = 0; i < 20; i++) {
		SparseArray3<int> arr;
		for(int j = 0; j <= 1000; j++)
			if(rand() % 3 == 0)
				arr.remove(arr.add(j));
			else
				arr.add(j);

		clock_t start = clock();
		for(int j = 0; j < 1000; j++)
			for(auto r = arr.begin(); r != arr.end(); r++)
				valueAccumulator += *r;
		clock_t stop = clock();
		accumulator += stop - start;
	}

	return accumulator;
}

int main() {
	std::cout << "RAND_MAX = " <<  RAND_MAX << "\n";


	float duration;

	duration = sa_singleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa1 singleAddRemove: " << duration << "\n";
	duration = sa2_singleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa2 singleAddRemove: " << duration << "\n";
	duration = sa3_singleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa3 singleAddRemove: " << duration << "\n\n";

	duration = sa_doubleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa1 doubleAddRemove: " << duration << "\n";
	duration = sa2_doubleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa2 doubleAddRemove: " << duration << "\n";
	duration = sa3_doubleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa3 doubleAddRemove: " << duration << "\n\n";

	duration = sa_tripleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa1 doubleAddRemove: " << duration << "\n";
	duration = sa2_tripleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa2 doubleAddRemove: " << duration << "\n";
	duration = sa3_tripleAddRemove() / float(CLOCKS_PER_SEC);
	std::cout << "sa3 doubleAddRemove: " << duration << "\n\n";

	duration = sa_iteration() / float(CLOCKS_PER_SEC);
	std::cout << "sa1 iteration: " << duration << "\n";
	duration = sa2_iteration() / float(CLOCKS_PER_SEC);
	std::cout << "sa2 iteration: " << duration << "\n";
	duration = sa3_iteration() / float(CLOCKS_PER_SEC);
	std::cout << "sa3 iteration: " << duration << "\n\n";
}
