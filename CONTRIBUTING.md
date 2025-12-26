# Contribution guidelines

This file is mostly aimed at developers, and primarily describes the setup required for development, and project-specific things to think about when contributing code. For general open-source contribution guidelines, see [opensource.guide](//opensource.guide). The guidelines listed under "Basic guidelines" do apply to all forms of contributions, including issues.

This file will not go into detail on how to write issues. Any important details that need to be included (if any) will be part of an issue template, selectable when you create an issue. If none exists for your use-case (or at all), use common sense. I do strongly suggest reading [the section on communicating effectively on opensource.guide](https://opensource.guide/how-to-contribute/#communicating-effectively) if you're wondering how to write good issues. There's nothing anyone could write here that isn't covered there and in thousands of other resources around the internet in far greater detail.

## Basic guidelines

### Use of generative AI is banned

Generative AI uses training data [based on plagiarism and piracy](https://web.archive.org/web/20250000000000*/https://www.theatlantic.com/technology/archive/2025/03/libgen-meta-openai/682093/), has [significant environmental costs associated with it](https://doi.org/10.21428/e4baedd9.9070dfe7), and [generates fundamentally insecure code](https://doi.org/10.1007/s10664-024-10590-1). GenAI is not ethically built, ethical to use, nor safe to use for programming applications. When caught, you will be permanently banned from contributing to the project, and any prior contributions will be checked and potentially reverted. Any and all contributions you've made cannot be trusted if AI slop machines were involved.

## Development setup

TBA

### Running tests

TBA

### Test policy

As much of the code should be tested as possible, within reason.

The primary goal of tests is to ensure there's a support framework that prevents backsliding in code quality. With enough tests, you don't need to worry as much about breaking something unrelated to what you were working on. 100% coverage is a pointless metric, but coverage tools can be useful to tell what critical paths aren't being tested. In real code, many paths may legitimately be unreachable without doing an awful lot of fucking around, particularly in exception handlers. Doing elaborate bullshit to test every possible path in the code, including trivial paths, is a waste of time and effort.

In practice, this means:

* If you're writing new functionality, write tests for the core parts of it
* If you're fixing a bug that was reported, write regression tests
* If you're working with edge-cases, test them

Writing tests isn't always feasible, but it should be attempted whereever possible. However, if any tests break, they must be fixed. Removal should only be done if the corresponding functionality is removed, and not as a way to bypass the test failures to maybe perhaps fix later.
