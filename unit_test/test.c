#include <stdio.h>
#include <elvea/elvea.h>
#include "cutest/CuTest.h"


CuSuite* string_test_suite();
//CuSuite* set_test_suite();
//CuSuite* table_test_suite();

int main()
{
	elvea_runtime_t runtime;
	elvea_thread_t *thread = elvea_initialize(&runtime, NULL, NULL);

	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, string_test_suite());

	printf("Running unit tests:\n\n");
	CuSuiteRun(suite, thread);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	CuStringDelete(output);
	CuSuiteDelete(suite);
	elvea_finalize(&runtime);
	return 0;
}
