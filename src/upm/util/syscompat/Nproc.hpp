namespace upm::util {

/**
 * Returns the number of max threads to use for concurrency, when not
 * auto-detected by the build system or whatever that's beign used.
 */
extern int getHardwareConcurrency();

}
