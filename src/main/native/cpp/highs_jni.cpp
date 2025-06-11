// highs_jni.cpp - JNI bridge between Java and HiGHS C++ library
#include <jni.h>
#include "Highs.h"  // HiGHS C++ header
#include <memory>
#include <vector>

// Helper function to convert Java array to C++ vector
std::vector<double> jdoubleArrayToVector(JNIEnv* env, jdoubleArray jarray) {
    jsize length = env->GetArrayLength(jarray);
    jdouble* elements = env->GetDoubleArrayElements(jarray, nullptr);
    std::vector<double> result(elements, elements + length);
    env->ReleaseDoubleArrayElements(jarray, elements, JNI_ABORT);
    return result;
}

std::vector<int> jintArrayToVector(JNIEnv* env, jintArray jarray) {
    jsize length = env->GetArrayLength(jarray);
    jint* elements = env->GetIntArrayElements(jarray, nullptr);
    std::vector<int> result(elements, elements + length);
    env->ReleaseIntArrayElements(jarray, elements, JNI_ABORT);
    return result;
}

// JNI function implementations
extern "C" {

JNIEXPORT jlong JNICALL
Java_HiGHS_createSolver(JNIEnv* env, jobject obj) {
    try {
        Highs* solver = new Highs();
        return reinterpret_cast<jlong>(solver);
    } catch (const std::exception& e) {
        // Throw Java exception
        jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClass, e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_HiGHS_destroySolver(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr != 0) {
        Highs* solver = reinterpret_cast<Highs*>(solverPtr);
        delete solver;
    }
}

JNIEXPORT jint JNICALL
Java_HiGHS_readModel(JNIEnv* env, jobject obj, jlong solverPtr, jstring filename) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const char* filenameStr = env->GetStringUTFChars(filename, nullptr);
    
    HighsStatus status = solver->readModel(std::string(filenameStr));
    
    env->ReleaseStringUTFChars(filename, filenameStr);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_HiGHS_run(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->run();
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_HiGHS_getModelStatus(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsModelStatus status = solver->getModelStatus();
    return static_cast<jint>(status);
}

JNIEXPORT jdoubleArray JNICALL
Java_HiGHS_getSolutionValues(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return nullptr;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const HighsSolution& solution = solver->getSolution();
    
    jsize numVars = solution.col_value.size();
    jdoubleArray result = env->NewDoubleArray(numVars);
    
    if (result != nullptr) {
        jdouble* values = new jdouble[numVars];
        for (jsize i = 0; i < numVars; i++) {
            values[i] = solution.col_value[i];
        }
        env->SetDoubleArrayRegion(result, 0, numVars, values);
        delete[] values;
    }
    
    return result;
}

JNIEXPORT jdouble JNICALL
Java_HiGHS_getObjectiveValue(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return 0.0;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const HighsInfo& info = solver->getInfo();
    return info.objective_function_value;
}

JNIEXPORT jint JNICALL
Java_HiGHS_addVariable(JNIEnv* env, jobject obj, jlong solverPtr, 
                      jdouble lowerBound, jdouble upperBound, jdouble cost) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->addCol(cost, lowerBound, upperBound, 0, nullptr, nullptr);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_HiGHS_addConstraint(JNIEnv* env, jobject obj, jlong solverPtr,
                        jdoubleArray coefficients, jintArray variableIndices,
                        jdouble lowerBound, jdouble upperBound) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    
    // Convert Java arrays to C++ vectors
    std::vector<double> coeffs = jdoubleArrayToVector(env, coefficients);
    std::vector<int> indices = jintArrayToVector(env, variableIndices);
    
    if (coeffs.size() != indices.size()) {
        return -1; // Error: mismatched array sizes
    }
    
    HighsStatus status = solver->addRow(lowerBound, upperBound, 
                                       coeffs.size(), indices.data(), coeffs.data());
    return static_cast<jint>(status);
}

} // extern "C"
