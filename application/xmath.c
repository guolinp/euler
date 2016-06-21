#include <stdio.h>

#define BUG() do { fprintf(stderr, "BUG: func %s, line %d\n", __func__,__LINE__); while(1); }while(0)

#define is_zero(b) not(b)

/* ! */
unsigned int not(unsigned int a)
{
	return a ? 0 : 1;
}

/* && */
unsigned int and(unsigned int a, unsigned int b)
{
	return a ? (b ? 1 : 0) : 0;
}

/* || */
unsigned int or(unsigned int a, unsigned int b)
{
	return a ? 1 : (b ? 1 : 0);
}

/* == */
unsigned int eq(unsigned int a, unsigned int b)
{
	while (and(a, b)) {
		a--;
		b--;
	}
	return or(a, b) ? 0 : 1;
}

/* > */
unsigned int gt(unsigned int a, unsigned int b)
{
	while (and(a, b)) {
		a--;
		b--;
	}
	return a ? 1 : 0;
}

/* < */
unsigned int lt(unsigned int a, unsigned int b)
{
	return gt(b, a);
}

/* >= */
unsigned int geq(unsigned int a, unsigned int b)
{
	while (and(a, b)) {
		a--;
		b--;
	}
	return a ? 1 : (b ? 0 : 1);
}

/* <= */
unsigned int leq(unsigned int a, unsigned int b)
{
	return geq(b, a);
}

/* + */
unsigned int add(unsigned int a, unsigned int b)
{
	while (b--)
		a++;
	return a;
}

/* - */
unsigned int sub(unsigned int a, unsigned int b)
{
	while (b--)
		a--;
	return a;
}

/* % */
unsigned int mod(unsigned int a, unsigned int b)
{
	if (is_zero(b))
		BUG();
	if (geq(a, b))
		return mod(sub(a, b), b);
	return a;
}

/* * */
unsigned int ____mul(unsigned int a, unsigned int b, unsigned int ret)
{
	if (b)
		return ____mul(a, --b, add(ret, a));
	return ret;
}

unsigned int mul(unsigned int a, unsigned int b)
{
	if (and(a, b))
		return ____mul(a, b, 0);
	return 0;
}

/* / */
unsigned int ____div(unsigned int a, unsigned int b, unsigned int ret)
{
	if (geq(a, b))
		return ____div(sub(a, b), b, ++ret);
	return ret;
}

unsigned int div(unsigned int a, unsigned int b)
{
	if (is_zero(b))
		BUG();
	return ____div(a, b, 0);
}

/*****************************************************************************/
#define TEST_CASE(fun, a, b, expect) \
do { \
	unsigned int ret = fun(a,b); \
	printf("CASE: " #fun "(%u,%u)=%u, expect=%u, %s\n", \
				(a), (b), ret, (expect), (ret == (expect)) ? "OK" : "FAILED!"); \
} while(0)

/* tast adapter */
unsigned int _not(unsigned int a, unsigned int unused)
{
	return not(a);
}

int main(int argc, char *argv[])
{
	TEST_CASE(_not, 0, 0, 1);
	TEST_CASE(_not, 1, 0, 0);

	printf("\n");

	TEST_CASE(and, 0, 0, 0 && 0);
	TEST_CASE(and, 0, 1, 0 && 1);
	TEST_CASE(and, 1, 0, 1 && 0);
	TEST_CASE(and, 1, 1, 1 && 1);

	printf("\n");

	TEST_CASE(or, 0, 0, 0 || 0);
	TEST_CASE(or, 0, 1, 0 || 1);
	TEST_CASE(or, 1, 0, 1 || 0);
	TEST_CASE(or, 1, 1, 1 || 1);

	printf("\n");

	TEST_CASE(eq, 0, 0, 0 == 0);
	TEST_CASE(eq, 0, 1, 0 == 1);
	TEST_CASE(eq, 1, 0, 1 == 0);
	TEST_CASE(eq, 1, 1, 1 == 1);

	printf("\n");

	TEST_CASE(gt, 0, 0, 0 > 0);
	TEST_CASE(gt, 0, 1, 0 > 1);
	TEST_CASE(gt, 1, 0, 1 > 0);
	TEST_CASE(gt, 1, 1, 1 > 1);

	printf("\n");

	TEST_CASE(geq, 0, 0, 0 >= 0);
	TEST_CASE(geq, 0, 1, 0 >= 1);
	TEST_CASE(geq, 1, 0, 1 >= 0);
	TEST_CASE(geq, 1, 1, 1 >= 1);

	printf("\n");

	TEST_CASE(lt, 0, 0, 0 < 0);
	TEST_CASE(lt, 0, 1, 0 < 1);
	TEST_CASE(lt, 1, 0, 1 < 0);
	TEST_CASE(lt, 1, 1, 1 < 1);

	printf("\n");

	TEST_CASE(leq, 0, 0, 0 <= 0);
	TEST_CASE(leq, 0, 1, 0 <= 1);
	TEST_CASE(leq, 1, 0, 1 <= 0);
	TEST_CASE(leq, 1, 1, 1 <= 1);

	printf("\n");

	TEST_CASE(add, 0, 0, 0 + 0);
	TEST_CASE(add, 0, 1, 0 + 1);
	TEST_CASE(add, 1, 0, 1 + 0);
	TEST_CASE(add, 1, 1, 1 + 1);

	printf("\n");

	TEST_CASE(sub, 0, 0, 0 - 0);
	TEST_CASE(sub, 0, 1, 0 - 1);
	TEST_CASE(sub, 1, 0, 1 - 0);
	TEST_CASE(sub, 1, 1, 1 - 1);

	printf("\n");

	TEST_CASE(mod, 0, 1, 0 % 1);
	TEST_CASE(mod, 1, 4, 1 % 4);
	TEST_CASE(mod, 4, 2, 4 % 2);
	TEST_CASE(mod, 4, 3, 4 % 3);
	TEST_CASE(mod, 7, 4, 7 % 4);

	printf("\n");

	TEST_CASE(mul, 0, 4, 0 * 4);
	TEST_CASE(mul, 1, 4, 1 * 4);
	TEST_CASE(mul, 4, 4, 4 * 4);
	TEST_CASE(mul, 5, 7, 5 * 7);

	printf("\n");

	TEST_CASE(div, 0, 4, 0 / 4);
	TEST_CASE(div, 1, 4, 1 / 4);
	TEST_CASE(div, 4, 2, 4 / 2);
	TEST_CASE(div, 5, 2, 5 / 2);
	TEST_CASE(div, 9, 3, 9 / 3);

	return 0;
}

/* gcc xmath.c -o xmath */
