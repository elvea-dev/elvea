#include "test.h"


static
void test_string_utf8(CuTest *tc)
{
	GET_RUNTIME(thread, tc);
	STR(s1, "안녕");
	CuAssertTrue(tc, s1->base.meta.ref_count == 1);
	CuAssertIntEquals(tc, 2, (int)elvea_string_length(thread, s1));
	elvea_object_release(thread, s1);
}

static
void test_string_append(CuTest *tc)
{
	GET_RUNTIME(thread, tc);
	STR(s1, "안녕");
	elvea_string_t *s2 = s1;
	elvea_object_retain(thread, s2);

	elvea_string_append(thread, &s1, "하세요", -1);
	CuAssertTrue(tc, s1->base.meta.ref_count == 1);
	CuAssertTrue(tc, s2->base.meta.ref_count == 1);
	CuAssertTrue(tc, s1 != s2);

	CuAssertStrEquals(tc, "안녕하세요", s1->data);
	CuAssertStrEquals(tc, "안녕", s2->data);
	elvea_object_release(thread, s1);
	elvea_object_release(thread, s2);
}

static
void test_string_prepend(CuTest *tc)
{
	GET_RUNTIME(thread, tc);
	STR(s1, "하세요");
	elvea_string_t *s2 = s1;
	elvea_object_retain(thread, s2);

	elvea_string_prepend(thread, &s1, "안녕", -1);
	CuAssertTrue(tc, s1->base.meta.ref_count == 1);
	CuAssertTrue(tc, s2->base.meta.ref_count == 1);
	CuAssertTrue(tc, s1 != s2);
	CuAssertStrEquals(tc, "안녕하세요", s1->data);
	CuAssertStrEquals(tc, "하세요", s2->data);
	elvea_object_release(thread, s1);
	elvea_object_release(thread, s2);
}

static
void test_string_insert(CuTest *tc)
{
	GET_RUNTIME(thread, tc);
	STR(s1, "hello!");
	elvea_string_t *s2 = s1;
	elvea_object_retain(thread, s2);

	elvea_string_insert(thread, &s1, 6, " world", -1);
	CuAssertTrue(tc, s1->base.meta.ref_count == 1);
	CuAssertTrue(tc, s2->base.meta.ref_count == 1);
	CuAssertTrue(tc, s1 != s2);
	CuAssertStrEquals(tc, "hello world!", s1->data);
	CuAssertStrEquals(tc, "hello!", s2->data);
	elvea_object_release(thread, s1);
	elvea_object_release(thread, s2);
}

static
void test_string_startend(CuTest *tc)
{
	GET_RUNTIME(thread, tc);
	STR(s1, "안녕하세요");
	CuAssertTrue(tc, elvea_string_starts_with(thread, s1, "안녕", -1));
	CuAssertTrue(tc, elvea_string_ends_with(thread, s1, "하세요", -1));
	elvea_object_release(thread, s1);
}



#if 0
static
void test_string_case(CuTest *tc)
{
	GET_RUNTIME(thread, tc);

	STR(s1, "c'était ça");
	const char *result = "C'ÉTAIT ÇA";

	elvea_string_t *s2 = elvea_string_to_upper(thread, s1);
	CuAssertStrEquals(tc, result, elvea_string_data(thread, s2));

	elvea_string_t *s3 = elvea_string_to_lower(thread, s2);
	CuAssertStrEquals(tc, elvea_string_data(thread, s1), elvea_string_data(thread, s3));
}
#endif

CuSuite* string_test_suite()
{
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_string_utf8);
	SUITE_ADD_TEST(suite, test_string_append);
	SUITE_ADD_TEST(suite, test_string_prepend);
	SUITE_ADD_TEST(suite, test_string_startend);
	SUITE_ADD_TEST(suite, test_string_insert);
	//SUITE_ADD_TEST(suite, test_string_case);

	return suite;
}