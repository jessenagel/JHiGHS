// jni_utils.cpp - Utility functions for JNI operations
#include "jni_utils.h"
#include <iostream>
#include <sstream>
#include <cstring>

namespace jni_utils {

// ============================================================================
// Exception Handling Utilities
// ============================================================================

void throwJavaException(JNIEnv* env, const char* exceptionClass, const char* message) {
    jclass clazz = env->FindClass(exceptionClass);
    if (clazz != nullptr) {
        env->ThrowNew(clazz, message);
        env->DeleteLocalRef(clazz);
    }
}

void throwRuntimeException(JNIEnv* env, const char* message) {
    throwJavaException(env, "java/lang/RuntimeException", message);
}

void throwIllegalArgumentException(JNIEnv* env, const char* message) {
    throwJavaException(env, "java/lang/IllegalArgumentException", message);
}

void throwOutOfMemoryError(JNIEnv* env, const char* message) {
    throwJavaException(env, "java/lang/OutOfMemoryError", message);
}

void throwHighsException(JNIEnv* env, const char* message) {
    throwJavaException(env, "org/highs/java/HighsException", message);
}

bool hasJavaException(JNIEnv* env) {
    return env->ExceptionCheck() == JNI_TRUE;
}

void clearJavaException(JNIEnv* env) {
    if (hasJavaException(env)) {
        env->ExceptionClear();
    }
}

// ============================================================================
// Array Conversion Utilities
// ============================================================================

std::vector<double> jdoubleArrayToVector(JNIEnv* env, jdoubleArray jarray) {
    if (jarray == nullptr) {
        return std::vector<double>();
    }

    jsize length = env->GetArrayLength(jarray);
    if (length == 0) {
        return std::vector<double>();
    }

    jdouble* elements = env->GetDoubleArrayElements(jarray, nullptr);
    if (elements == nullptr) {
        throwOutOfMemoryError(env, "Failed to access double array elements");
        return std::vector<double>();
    }

    std::vector<double> result(elements, elements + length);
    env->ReleaseDoubleArrayElements(jarray, elements, JNI_ABORT);

    return result;
}

std::vector<int> jintArrayToVector(JNIEnv* env, jintArray jarray) {
    if (jarray == nullptr) {
        return std::vector<int>();
    }

    jsize length = env->GetArrayLength(jarray);
    if (length == 0) {
        return std::vector<int>();
    }

    jint* elements = env->GetIntArrayElements(jarray, nullptr);
    if (elements == nullptr) {
        throwOutOfMemoryError(env, "Failed to access int array elements");
        return std::vector<int>();
    }

    std::vector<int> result(elements, elements + length);
    env->ReleaseIntArrayElements(jarray, elements, JNI_ABORT);

    return result;
}

jdoubleArray vectorToJdoubleArray(JNIEnv* env, const std::vector<double>& vec) {
    jsize length = static_cast<jsize>(vec.size());
    jdoubleArray result = env->NewDoubleArray(length);

    if (result == nullptr) {
        throwOutOfMemoryError(env, "Failed to create double array");
        return nullptr;
    }

    if (length > 0) {
        env->SetDoubleArrayRegion(result, 0, length, vec.data());
        if (hasJavaException(env)) {
            env->DeleteLocalRef(result);
            return nullptr;
        }
    }

    return result;
}

jintArray vectorToJintArray(JNIEnv* env, const std::vector<int>& vec) {
    jsize length = static_cast<jsize>(vec.size());
    jintArray result = env->NewIntArray(length);

    if (result == nullptr) {
        throwOutOfMemoryError(env, "Failed to create int array");
        return nullptr;
    }

    if (length > 0) {
        env->SetIntArrayRegion(result, 0, length, vec.data());
        if (hasJavaException(env)) {
            env->DeleteLocalRef(result);
            return nullptr;
        }
    }

    return result;
}

// ============================================================================
// String Conversion Utilities
// ============================================================================

std::string jstringToString(JNIEnv* env, jstring jstr) {
    if (jstr == nullptr) {
        return std::string();
    }

    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    if (chars == nullptr) {
        throwOutOfMemoryError(env, "Failed to access string characters");
        return std::string();
    }

    std::string result(chars);
    env->ReleaseStringUTFChars(jstr, chars);

    return result;
}

jstring stringToJstring(JNIEnv* env, const std::string& str) {
    jstring result = env->NewStringUTF(str.c_str());
    if (result == nullptr) {
        throwOutOfMemoryError(env, "Failed to create Java string");
    }
    return result;
}

// ============================================================================
// Solver Pointer Utilities
// ============================================================================

Highs* getSolverFromHandle(JNIEnv* env, jlong handle) {
    if (handle == 0) {
        throwIllegalArgumentException(env, "Invalid solver handle: null pointer");
        return nullptr;
    }

    return reinterpret_cast<Highs*>(handle);
}

jlong solverToHandle(Highs* solver) {
    return reinterpret_cast<jlong>(solver);
}

// ============================================================================
// Status Conversion Utilities
// ============================================================================

jint highsStatusToJint(HighsStatus status) {
    return static_cast<jint>(status);
}

jint modelStatusToJint(HighsModelStatus status) {
    return static_cast<jint>(status);
}

// ============================================================================
// Validation Utilities
// ============================================================================

bool validateArraySizes(JNIEnv* env, const std::vector<double>& coeffs,
                       const std::vector<int>& indices, const char* context) {
    if (coeffs.size() != indices.size()) {
        std::ostringstream oss;
        oss << context << ": coefficient array size (" << coeffs.size()
            << ") does not match indices array size (" << indices.size() << ")";
        throwIllegalArgumentException(env, oss.str().c_str());
        return false;
    }
    return true;
}

bool validateBounds(JNIEnv* env, double lowerBound, double upperBound, const char* context) {
    if (lowerBound > upperBound) {
        std::ostringstream oss;
        oss << context << ": lower bound (" << lowerBound
            << ") cannot be greater than upper bound (" << upperBound << ")";
        throwIllegalArgumentException(env, oss.str().c_str());
        return false;
    }
    return true;
}

bool validateNonNegativeIndex(JNIEnv* env, int index, const char* context) {
    if (index < 0) {
        std::ostringstream oss;
        oss << context << ": index (" << index << ") cannot be negative";
        throwIllegalArgumentException(env, oss.str().c_str());
        return false;
    }
    return true;
}

// ============================================================================
// Logging Utilities
// ============================================================================

void logInfo(const char* message) {
    std::cout << "[jhighs INFO] " << message << std::endl;
}

void logWarning(const char* message) {
    std::cerr << "[jhighs WARNING] " << message << std::endl;
}

void logError(const char* message) {
    std::cerr << "[jhighs ERROR] " << message << std::endl;
}

void logDebug(const char* message) {
    #ifdef DEBUG
    std::cout << "[jhighs DEBUG] " << message << std::endl;
    #endif
}

// ============================================================================
// Memory Management Utilities
// ============================================================================

void safeDeleteLocalRef(JNIEnv* env, jobject obj) {
    if (obj != nullptr) {
        env->DeleteLocalRef(obj);
    }
}

void safeDeleteGlobalRef(JNIEnv* env, jobject obj) {
    if (obj != nullptr) {
        env->DeleteGlobalRef(obj);
    }
}

// ============================================================================
// HiGHS-Specific Utilities
// ============================================================================

bool isOptimalStatus(HighsModelStatus status) {
    return status == HighsModelStatus::kOptimal;
}

bool isInfeasibleStatus(HighsModelStatus status) {
    return status == HighsModelStatus::kInfeasible;
}

bool isUnboundedStatus(HighsModelStatus status) {
    return status == HighsModelStatus::kUnbounded ||
           status == HighsModelStatus::kUnboundedOrInfeasible;
}

bool isErrorStatus(HighsModelStatus status) {
    return status == HighsModelStatus::kLoadError ||
           status == HighsModelStatus::kModelError ||
           status == HighsModelStatus::kPresolveError ||
           status == HighsModelStatus::kSolveError ||
           status == HighsModelStatus::kPostsolveError;
}

const char* modelStatusToString(HighsModelStatus status) {
    switch (status) {
        case HighsModelStatus::kNotset: return "Not set";
        case HighsModelStatus::kLoadError: return "Load error";
        case HighsModelStatus::kModelError: return "Model error";
        case HighsModelStatus::kPresolveError: return "Presolve error";
        case HighsModelStatus::kSolveError: return "Solve error";
        case HighsModelStatus::kPostsolveError: return "Postsolve error";
        case HighsModelStatus::kModelEmpty: return "Model empty";
        case HighsModelStatus::kOptimal: return "Optimal";
        case HighsModelStatus::kInfeasible: return "Infeasible";
        case HighsModelStatus::kUnboundedOrInfeasible: return "Unbounded or infeasible";
        case HighsModelStatus::kUnbounded: return "Unbounded";
        case HighsModelStatus::kObjectiveBound: return "Objective bound reached";
        case HighsModelStatus::kObjectiveTarget: return "Objective target reached";
        case HighsModelStatus::kTimeLimit: return "Time limit reached";
        case HighsModelStatus::kIterationLimit: return "Iteration limit reached";
        case HighsModelStatus::kUnknown: return "Unknown";
        default: return "Unrecognized status";
    }
}

const char* highsStatusToString(HighsStatus status) {
    switch (status) {
        case HighsStatus::kOk: return "OK";
        case HighsStatus::kWarning: return "Warning";
        case HighsStatus::kError: return "Error";
        default: return "Unrecognized status";
    }
}

// ============================================================================
// Performance Utilities
// ============================================================================

void benchmarkStart(const char* operation) {
    #ifdef BENCHMARK
    logInfo(("Starting: " + std::string(operation)).c_str());
    #endif
}

void benchmarkEnd(const char* operation) {
    #ifdef BENCHMARK
    logInfo(("Completed: " + std::string(operation)).c_str());
    #endif
}

} // namespace jni_utils