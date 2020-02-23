#include "CacheTest.hpp"

#include <unistd.h>

Test*
CacheTest::suite() {
	TestSuite* res = new TestSuite("UtilsTest");
	res->addTest(new TestCaller<CacheTest>(
				"sample_test",
				&CacheTest::simpleTest));
	return res;
}

void
CacheTest::simpleTest() {
	LRUCache l(4, -1);

	for (int a : {1,3,2,3,5,7,1,9}) {
		l.set(a, a);
		int cont = 0;
		printf("BEGIN\n");
		for (const std::pair<int, int>& v : l.data) {
			printf("%d - %d\n", v.first, v.second);
			cont++;

			if (cont == 20) {
				exit(1);
			}
		}
		printf("END\n");
	}
}

