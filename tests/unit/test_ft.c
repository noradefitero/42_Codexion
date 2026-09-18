/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ft.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:46:48 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:46:52 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"
#include "../../src/utils/ft/ft.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * NOTE: ft_memset is declared in ft.h but has NO implementation anywhere in
 * src/ (there is no ft_memset.c), so it cannot be tested: any call would
 * fail at link time.
 */

/* Unity calls these around every test; no shared fixtures are used here. */
void	setUp(void)
{
}

void	tearDown(void)
{
}

static void	test_ft_isalpha(void)
{
	static const struct
	{
		int	c;
		int	expected;
	}	cases[] = {
		{'A', 1}, {'Z', 1}, {'a', 1}, {'z', 1}, {'M', 1}, {'n', 1},
		{'0', 0}, {'9', 0}, {' ', 0}, {'\n', 0}, {'\t', 0}, {'!', 0},
		{'@', 0}, {'[', 0}, {'`', 0}, {'{', 0}, {'~', 0},
		{0, 0}, {-1, 0}, {127, 0}, {128, 0}, {200, 0}, {255, 0}
	};
	size_t	i;

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
		TEST_ASSERT_EQUAL_INT(cases[i].expected, ft_isalpha(cases[i].c));
}

static void	test_ft_strisdigit(void)
{
	static const char	*digits[] = {
		"0", "1234567890", "000"
	};
	static const char	*not_digits[] = {
		" 1", "1 ", "-1", "+1", "1a", "a1", "12.5", "\t1", "1\n",
		"\331\24123", "."
	};
	char	big[201];
	size_t	i;

	for (i = 0; i < sizeof(digits) / sizeof(digits[0]); i++)
		TEST_ASSERT_EQUAL_INT(1, ft_strisdigit(digits[i]));

	/* 200-char all-digit string */
	for (i = 0; i < 200; i++)
		big[i] = '7';
	big[200] = '\0';
	TEST_ASSERT_EQUAL_INT(1, ft_strisdigit(big));

	TEST_ASSERT_EQUAL_INT(0, ft_strisdigit(NULL));

	/*
	 * Quirk: the implementation returns 1 for the empty string because the
	 * while loop body never runs. We assert the ACTUAL behavior.
	 */
	TEST_ASSERT_EQUAL_INT(1, ft_strisdigit(""));

	for (i = 0; i < sizeof(not_digits) / sizeof(not_digits[0]); i++)
		TEST_ASSERT_EQUAL_INT(0, ft_strisdigit(not_digits[i]));
}

static void	test_ft_calloc(void)
{
	void			*p;
	int				*arr;
	unsigned char	*b;
	size_t			i;

	/* zero-size requests must not crash; malloc(0) is platform-dependent,
	 * so NULL-ness is not asserted. */
	p = ft_calloc(0, 0);
	free(p);
	p = ft_calloc(0, 10);
	free(p);
	p = ft_calloc(10, 0);
	free(p);

	/* (5, sizeof(int)): every byte zero */
	arr = (int *)ft_calloc(5, sizeof(int));
	TEST_ASSERT_NOT_NULL(arr);
	b = (unsigned char *)arr;
	TEST_ASSERT_EQUAL_UINT8(0, b[0]);
	TEST_ASSERT_EQUAL_UINT8(0, b[5 * sizeof(int) - 1]);
	for (i = 0; i < 5 * sizeof(int); i++)
		TEST_ASSERT_EQUAL_UINT8(0, b[i]);
	free(arr);

	/* overflow must return NULL */
	TEST_ASSERT_NULL(ft_calloc(SIZE_MAX, 2));
	TEST_ASSERT_NULL(ft_calloc(2, SIZE_MAX));
	TEST_ASSERT_NULL(ft_calloc(SIZE_MAX / 2 + 1, 2));

	/* (1, 100000): first and last byte zero */
	b = (unsigned char *)ft_calloc(1, 100000);
	TEST_ASSERT_NOT_NULL(b);
	TEST_ASSERT_EQUAL_UINT8(0, b[0]);
	TEST_ASSERT_EQUAL_UINT8(0, b[99999]);
	free(b);
}

static void	test_ft_memcpy(void)
{
	char	dest[8];
	char	*buf_a;
	char	*buf_b;
	void	*ret;
	size_t	i;

	/* 5-byte copy of "hello" */
	memset(dest, 'X', sizeof(dest));
	ret = ft_memcpy(dest, "hello", 5);
	TEST_ASSERT_EQUAL_PTR(dest, ret);
	TEST_ASSERT_EQUAL_MEMORY("hello", dest, 5);
	TEST_ASSERT_EQUAL_UINT8('X', (unsigned char)dest[5]);

	/* n == 0 with valid dest/src: returns dest, dest unchanged */
	memset(dest, 'A', sizeof(dest));
	ret = ft_memcpy(dest, "xyz", 0);
	TEST_ASSERT_EQUAL_PTR(dest, ret);
	TEST_ASSERT_EQUAL_MEMORY("AAAAAAA", dest, 7);

	/* both NULL */
	TEST_ASSERT_NULL(ft_memcpy(NULL, NULL, 0));
	TEST_ASSERT_NULL(ft_memcpy(NULL, NULL, 5));

	/* 1-byte copy */
	dest[0] = 0;
	dest[1] = 0;
	ret = ft_memcpy(dest, "Q", 1);
	TEST_ASSERT_EQUAL_PTR(dest, ret);
	TEST_ASSERT_EQUAL_UINT8('Q', (unsigned char)dest[0]);
	TEST_ASSERT_EQUAL_UINT8(0, (unsigned char)dest[1]);

	/*
	 * WARNING: calling ft_memcpy with exactly one of dest/src NULL and
	 * n > 0 dereferences the NULL pointer (the implementation only guards
	 * the both-NULL case), so that combination is unsafe and untestable.
	 */
	buf_a = (char *)malloc(1024 * 1024);
	buf_b = (char *)malloc(1024 * 1024);
	TEST_ASSERT_NOT_NULL(buf_a);
	TEST_ASSERT_NOT_NULL(buf_b);
	for (i = 0; i < 1024 * 1024; i++)
		buf_a[i] = (char)(i % 251);
	ret = ft_memcpy(buf_b, buf_a, 1024 * 1024);
	TEST_ASSERT_EQUAL_PTR(buf_b, ret);
	TEST_ASSERT_EQUAL_UINT8((unsigned char)buf_a[0], (unsigned char)buf_b[0]);
	TEST_ASSERT_EQUAL_UINT8((unsigned char)buf_a[1024 * 1024 / 2],
		(unsigned char)buf_b[1024 * 1024 / 2]);
	TEST_ASSERT_EQUAL_UINT8((unsigned char)buf_a[1024 * 1024 - 1],
		(unsigned char)buf_b[1024 * 1024 - 1]);
	free(buf_a);
	free(buf_b);
}

static void	test_ft_realloc(void)
{
	unsigned char	*p;
	unsigned char	*new;
	size_t			i;

	/* NULL ptr: behaves like malloc */
	p = (unsigned char *)ft_realloc(NULL, 0, 64);
	TEST_ASSERT_NOT_NULL(p);
	free(p);

	/* new_size 0: frees ptr and returns NULL */
	p = (unsigned char *)malloc(8);
	TEST_ASSERT_NOT_NULL(p);
	new = (unsigned char *)ft_realloc(p, 8, 0);
	TEST_ASSERT_NULL(new); /* p was freed by ft_realloc */

	/* grow 8 -> 16: original 8 bytes preserved */
	p = (unsigned char *)malloc(8);
	TEST_ASSERT_NOT_NULL(p);
	for (i = 0; i < 8; i++)
		p[i] = (unsigned char)(0x10 + i);
	new = (unsigned char *)ft_realloc(p, 8, 16);
	TEST_ASSERT_NOT_NULL(new);
	for (i = 0; i < 8; i++)
		TEST_ASSERT_EQUAL_UINT8((unsigned char)(0x10 + i), new[i]);
	free(new);

	/* shrink 16 -> 8: first 8 bytes preserved */
	p = (unsigned char *)malloc(16);
	TEST_ASSERT_NOT_NULL(p);
	for (i = 0; i < 16; i++)
		p[i] = (unsigned char)(0x20 + i);
	new = (unsigned char *)ft_realloc(p, 16, 8);
	TEST_ASSERT_NOT_NULL(new);
	for (i = 0; i < 8; i++)
		TEST_ASSERT_EQUAL_UINT8((unsigned char)(0x20 + i), new[i]);
	free(new);

	/* actual_size 0 with valid ptr: still a valid allocation */
	p = (unsigned char *)malloc(16);
	TEST_ASSERT_NOT_NULL(p);
	memset(p, 'x', 16);
	new = (unsigned char *)ft_realloc(p, 0, 8);
	TEST_ASSERT_NOT_NULL(new);
	free(new);

	/* 4 -> 4: bytes preserved */
	p = (unsigned char *)malloc(4);
	TEST_ASSERT_NOT_NULL(p);
	for (i = 0; i < 4; i++)
		p[i] = (unsigned char)('a' + i);
	new = (unsigned char *)ft_realloc(p, 4, 4);
	TEST_ASSERT_NOT_NULL(new);
	TEST_ASSERT_EQUAL_MEMORY("abcd", new, 4);
	free(new);
}

static void	test_ft_minsize(void)
{
	TEST_ASSERT_EQUAL_UINT64(5, (uint64_t)ft_minsize(5, 10));
	TEST_ASSERT_EQUAL_UINT64(5, (uint64_t)ft_minsize(10, 5));
	TEST_ASSERT_EQUAL_UINT64(0, (uint64_t)ft_minsize(0, 0));
	TEST_ASSERT_EQUAL_UINT64(1, (uint64_t)ft_minsize(1, 1));
	TEST_ASSERT_EQUAL_UINT64(3, (uint64_t)ft_minsize(SIZE_MAX, 3));
	TEST_ASSERT_EQUAL_UINT64(3, (uint64_t)ft_minsize(3, SIZE_MAX));
}

void	run_ft_tests(void)
{
	RUN_TEST(test_ft_isalpha);
	RUN_TEST(test_ft_strisdigit);
	RUN_TEST(test_ft_calloc);
	RUN_TEST(test_ft_memcpy);
	RUN_TEST(test_ft_realloc);
	RUN_TEST(test_ft_minsize);
}