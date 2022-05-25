#pragma once

#ifdef _WIN32
#error "Windows isn't supported"
#elif defined __APPLE__
#warning "MacOS has very limited support at this time. Please consider contributing to make Mac support better."
#define MACOS
#elif defined __linux__
#define LINUX
#else
#warning "Unable to detect OS configuration. Using generic packages..."
#define OTHER
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define X86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define X86_32
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__) || defined(__ARM_ARCH_6__)
#define ARM7
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ARM64
#elif defined(__arm__)
#warning "assuming __arm__ means armv7. Added for RPi B3+ compatibility"
#define ARM7
#else
#error "Unknown or unsupported architecture."
#endif
