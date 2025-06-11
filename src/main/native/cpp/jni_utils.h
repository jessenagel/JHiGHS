// jni_utils.h - Header file for JNI utility functions
#ifndef JNI_UTILS_H
#define JNI_UTILS_H

#include <jni.h>
#include <vector>
#include <string>
#include "Highs.h"  // HiGHS library header

namespace jni_utils {

// ============================================================================
// Exception Handling Utilities
// ============================================================================

/**
 * Throw a Java exception with the specified class and message
 * @param env JNI environment
 * @param exceptionClass Fully qualified Java exception class name
 * @param message Exception message
 */
void throwJavaException(JNIEnv* env, const char* exceptionClass, const char* message);

/**
 * Throw a Java RuntimeException
 * @param env JNI environment
 * @param message Exception message
 */
void throwRuntimeException(JNIEnv* env, const char* message);

/**
 * Throw a Java IllegalArgumentException
 * @param env JNI environment
 * @param message Exception message
 */
void throwIllegalArgumentException(JNIEnv* env, const char* message);

/**
 * Throw a Java OutOfMemoryError
 * @param env JNI environment
 * @param message Exception message
 */
void throwOutOfMemoryError(JNIEnv* env, const char* message);

/**
 * Throw a custom HighsException
 * @param env JNI environment
 * @param message Exception message
 */
void throwHighsException(JNIEnv* env, const char* message);

/**
 * Check if there's a pending Java exception
 * @param env JNI environment
 * @return true if there's a pending exception
 */
bool hasJavaException(JNIEnv* env);

/**
 * Clear any pending Java exception
 * @param env JNI environment
 */
void clearJavaException(JNIEnv* env);

// ============================================================================
// Array Conversion Utilities
// ============================================================================

/**
 * Convert Java double array to C++ vector
 * @param env JNI environment
 * @param jarray Java double array
 * @return C++ vector of doubles
 */
std::vector<double> jdoubleArrayToVector(JNIEnv* env, jdoubleArray jarray);

/**
 * Convert Java int array to C++ vector
 * @param env JNI environment
 * @param jarray Java int array
 * @return C++ vector of ints
 */
std::vector<int> jintArrayToVector(JNIEnv* env, jintArray jarray);

/**
 * Convert C++ vector to Java double array
 * @param env JNI environment
 * @param vec C++ vector of doubles
 * @return Java double array
 */
jdoubleArray vectorToJdoubleArray(JNIEnv* env, const std::vector<double>& vec);

/**
 * Convert C++ vector to Java int array
 * @param env JNI environment
 * @param vec C++ vector of ints
 * @return Java int array
 */
jintArray vectorToJintArray(JNIEnv* env, const std::vector<int>& vec);

// ============================================================================
// String Conversion Utilities
// ============================================================================

/**
 * Convert Java string to C++ string
 * @param env JNI environment
 * @param jstr Java string
 * @return C++ string
 */
std::string jstringToString(JNIEnv* env, jstring jstr);

/**
 * Convert C++ string to Java string
 * @param env JNI environment
 * @param str C++ string
 * @return Java string
 */
jstring stringToJstring(JNIEnv* env, const std::string& str);

// ============================================================================
// Solver Pointer Utilities
// ============================================================================

/**
 * Convert JNI handle to HiGHS solver pointer
 * @param env JNI environment
 * @param handle JNI handle (jlong)
 * @return Pointer to HiGHS solver instance
 */
Highs* getSolverFromHandle(JNIEnv* env, jlong handle);

/**
 * Convert HiGHS solver pointer to JNI handle
 * @param solver Pointer to HiGHS solver instance
 * @return JNI handle (jlong)
 */
jlong solverToHandle(Highs* solver);

// ============================================================================
// Status Conversion Utilities
// ============================================================================

/**
 * Convert HiGHS status to Java int
 * @param status HiGHS status enum
 * @return Java int representation
 */
jint highsStatusToJint(HighsStatus status);

/**
 * Convert HiGHS model status to Java int
 * @param status HiGHS model status enum
 * @return Java int representation
 */
jint modelStatusToJint(HighsModelStatus status);

// ============================================================================
// Validation Utilities
// ============================================================================

/**
 * Validate that coefficient and indices arrays have the same size
 * @param env JNI environment
 * @param coeffs Coefficient vector
 * @param indices Indices vector
 * @param context Context string for error messages
 * @return true if arrays are valid
 */
bool validateArraySizes(JNIEnv* env, const std::vector<double>& coeffs,
                       const std::vector<int>& indices, const char* context);

/**
 * Validate that lower bound is not greater than upper bound
 * @param env JNI environment
 * @param lowerBound Lower bound value
 * @param upperBound Upper bound value
 * @param context Context string for error messages
 * @return true if bounds are valid
 */
bool validateBounds(JNIEnv* env, double lowerBound, double upperBound, const char* context);

/**
 * Validate that index is non-negative
 * @param env JNI environment
 * @param index Index value to validate
 * @param context Context string for error messages
 * @return true if index is valid
 */
bool validateNonNegativeIndex(JNIEnv* env, int index, const char* context);

// ============================================================================
// Logging Utilities
// ============================================================================

/**
 * Log an informational message
 * @param message Message to log
 */
void logInfo(const char* message);

/**
 * Log a warning message
 * @param message Message to log
 */
void logWarning(const char* message);

/**
 * Log an error message
 * @param message Message to log
 */
void logError(const char* message);

/**
 * Log a debug message (only in debug builds)
 * @param message Message to log
 */
void logDebug(const char* message);

// ============================================================================
// Memory Management Utilities
// ============================================================================

/**
 * Safely delete a local JNI reference
 * @param env JNI environment
 * @param obj Object reference to delete
 */
void safeDeleteLocalRef(JNIEnv* env, jobject obj);

/**
 * Safely delete a global JNI reference
 * @param env JNI environment
 * @param obj Object reference to delete
 */
void safeDeleteGlobalRef(JNIEnv* env, jobject obj);

// ============================================================================
// HiGHS-Specific Utilities
// ============================================================================

/**
 * Check if model status indicates optimal solution
 * @param status HiGHS model status
 * @return true if status is optimal
 */
bool isOptimalStatus(HighsModelStatus status);

/**
 * Check if model status indicates infeasible problem
 * @param status HiGHS model status
 * @return true if status is infeasible
 */
bool isInfeasibleStatus(HighsModelStatus status);

/**
 * Check if model status indicates unbounded problem
 * @param status HiGHS model status
 * @return true if status is unbounded
 */
bool isUnboundedStatus(HighsModelStatus status);

/**
 * Check if model status indicates an error
 * @param status HiGHS model status
 * @return true if status is an error
 */
bool isErrorStatus(HighsModelStatus status);

/**
 * Convert model status to human-readable string
 * @param status HiGHS model status
 * @return String representation
 */
const char* modelStatusToString(HighsModelStatus status);

/**
 * Convert HiGHS status to human-readable string
 * @param status HiGHS status
 * @return String representation
 */
const char* highsStatusToString(HighsStatus status);

// ============================================================================
// Performance Utilities
// ============================================================================

/**
 * Mark the start of a benchmarked operation (debug builds only)
 * @param operation Operation name
 */
void benchmarkStart(const char* operation);

/**
 * Mark the end of a benchmarked operation (debug builds only)
 * @param operation Operation name
 */
void benchmarkEnd(const char* operation);

// ============================================================================
// Convenience Macros
// ============================================================================

#define JNI_CHECK_NULL_RETURN(ptr, retval) \
    if ((ptr) == nullptr) { \
        throwIllegalArgumentException(env, "Null pointer argument"); \
        return (retval); \
    }

#define JNI_CHECK_SOLVER_RETURN(handle, retval) \
    Highs* solver = getSolverFromHandle(env, handle); \
    if (solver == nullptr || hasJavaException(env)) { \
        return (retval); \
    }

#define JNI_SAFE_CALL(call) \
    try { \
        call; \
    } catch (const std::exception& e) { \
        throwRuntimeException(env, e.what()); \
        return; \
    } catch (...) { \
        throwRuntimeException(env, "Unknown C++ exception occurred"); \
        return; \
    }

#define JNI_SAFE_CALL_RETURN(call, retval) \
    try { \
        return call; \
    } catch (const std::exception& e) { \
        throwRuntimeException(env, e.what()); \
        return (retval); \
    } catch (...) { \
        throwRuntimeException(env, "Unknown C++ exception occurred"); \
        return (retval); \
    }

} // namespace jni_utils

#endif // JNI_UTILS_H