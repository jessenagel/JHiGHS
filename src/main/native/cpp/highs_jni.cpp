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
// Model Management Functions
JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_clearModel(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->clearModel();
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_clearSolver(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->clearSolver();
    return static_cast<jint>(status);
}


JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_writeModel(JNIEnv* env, jobject obj, jlong solverPtr, jstring filename) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const char* cFilename = env->GetStringUTFChars(filename, NULL);
    HighsStatus status = solver->writeModel(cFilename);
    env->ReleaseStringUTFChars(filename, cFilename);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_writeSolution(JNIEnv* env, jobject obj, jlong solverPtr, jstring filename) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const char* cFilename = env->GetStringUTFChars(filename, NULL);
    HighsStatus status = solver->writeSolution(cFilename);
    env->ReleaseStringUTFChars(filename, cFilename);
    return static_cast<jint>(status);
}

JNIEXPORT jlong JNICALL
Java_nl_jessenagel_jhighs_HiGHS_createSolver(JNIEnv* env, jobject obj) {
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
Java_nl_jessenagel_jhighs_HiGHS_destroySolver(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr != 0) {
        Highs* solver = reinterpret_cast<Highs*>(solverPtr);
        delete solver;
    }
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_readModel(JNIEnv* env, jobject obj, jlong solverPtr, jstring filename) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const char* filenameStr = env->GetStringUTFChars(filename, nullptr);
    
    HighsStatus status = solver->readModel(std::string(filenameStr));
    
    env->ReleaseStringUTFChars(filename, filenameStr);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_run(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->run();
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_getModelStatus(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsModelStatus status = solver->getModelStatus();
    return static_cast<jint>(status);
}

// Variable Manipulation Functions


JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_addVariable(JNIEnv* env, jobject obj, jlong solverPtr, 
                      jdouble lowerBound, jdouble upperBound) {
    if (solverPtr == 0) return -1;
    
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->addVar(lowerBound, upperBound);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_addVariables(JNIEnv* env, jobject obj, jlong solverPtr,
                        jdoubleArray lowerBounds, jdoubleArray upperBounds) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);

    // Convert Java arrays to C++ vectors
    std::vector<double> lower = jdoubleArrayToVector(env, lowerBounds);
    std::vector<double> upper = jdoubleArrayToVector(env, upperBounds);

    if (lower.size() != upper.size()) {
        return -1; // Error: mismatched array sizes
    }

    // Convert lower.size() to HighsInt
    HighsInt numVars = static_cast<HighsInt>(lower.size());

    HighsStatus status = solver->addVars(numVars, lower.data(), upper.data());
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_deleteVariable(JNIEnv* env, jobject obj, jlong solverPtr, jint variableIndex) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->deleteVars(variableIndex,variableIndex);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_changeVariableBounds(JNIEnv* env, jobject obj, jlong solverPtr,
                        jint variableIndex, jdouble lowerBound, jdouble upperBound) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->changeColBounds(variableIndex, lowerBound, upperBound);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_changeColIntegrality(JNIEnv* env, jobject obj, jlong solverPtr,
                        jint variableIndex, jint varTypeInt) {
    if (solverPtr == 0) return -1;
    HighsVarType varType = HighsVarType::kContinuous;
    switch (varTypeInt) {
        case 0: varType = HighsVarType::kContinuous; break;
        case 1: varType = HighsVarType::kInteger; break;
        case 2: varType = HighsVarType::kSemiContinuous; break;
        case 3: varType = HighsVarType::kSemiInteger; break;
        case 4: varType = HighsVarType::kImplicitInteger; break;
        default: return -1; // Invalid type
    }
    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->changeColIntegrality(variableIndex, varType);
    return static_cast<jint>(status);
}

// Constraint Manipulation Functions

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_addConstraint(JNIEnv* env, jobject obj, jlong solverPtr,
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

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_deleteConstraint(JNIEnv* env, jobject obj, jlong solverPtr, jint constraintIndex) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->deleteRows(constraintIndex,constraintIndex);
    return static_cast<jint>(status);
}

JNIEXPORT jint JNICALL
Java_nl_jessenagel_jhighs_HiGHS_changeConstraintBounds(JNIEnv* env, jobject obj, jlong solverPtr,
                        jint constraintIndex, jdouble lowerBound, jdouble upperBound) {
    if (solverPtr == 0) return -1;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    HighsStatus status = solver->changeRowBounds(constraintIndex, lowerBound, upperBound);
    return static_cast<jint>(status);
}

// Objective Function Manipulation Functions

JNIEXPORT jint JNICALL Java_nl_jessenagel_jhighs_HiGHS_setObjective
  (JNIEnv *env, jobject obj, jlong solverPtr, jdoubleArray coeffs, jintArray indices, jboolean minimize, jdouble offset) {

  // Convert solver pointer
  Highs *highs = (Highs *)(intptr_t)solverPtr;

  // Get arrays from Java
  jsize len = env->GetArrayLength(coeffs);
  jdouble *coefficients = env->GetDoubleArrayElements(coeffs, nullptr);
  jint *varIndices = env->GetIntArrayElements(indices, nullptr);

  // Set objective sense
  if (highs->changeObjectiveSense(minimize ? ObjSense::kMinimize : ObjSense::kMaximize) != HighsStatus::kOk) {
    env->ReleaseDoubleArrayElements(coeffs, coefficients, JNI_ABORT);
    env->ReleaseIntArrayElements(indices, varIndices, JNI_ABORT);
    return (jint)HighsStatus::kError;
  }

  // Set objective offset
  if (highs->changeObjectiveOffset(offset) != HighsStatus::kOk) {
    env->ReleaseDoubleArrayElements(coeffs, coefficients, JNI_ABORT);
    env->ReleaseIntArrayElements(indices, varIndices, JNI_ABORT);
    return (jint)HighsStatus::kError;
  }

  // Set coefficients one by one
  for (jsize i = 0; i < len; i++) {
    int col = varIndices[i];
    double cost = coefficients[i];
    if (highs->changeColCost(col, cost) != HighsStatus::kOk) {
      env->ReleaseDoubleArrayElements(coeffs, coefficients, JNI_ABORT);
      env->ReleaseIntArrayElements(indices, varIndices, JNI_ABORT);
      return (jint)HighsStatus::kError;
    }
  }

  // Release Java arrays
  env->ReleaseDoubleArrayElements(coeffs, coefficients, JNI_ABORT);
  env->ReleaseIntArrayElements(indices, varIndices, JNI_ABORT);

  return (jint)HighsStatus::kOk;
}

// Solution Management Functions

//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_getReducedCosts(JNIEnv* env, jobject obj, jlong solverPtr) {
//    if (solverPtr == 0) return 0.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const HighsSolution& solution = solver->getSolution();
//
//    jsize numVars = solution.col_value.size();
//    jdoubleArray result = env->NewDoubleArray(numVars);
//
//    if (result != nullptr) {
//        jdouble* reducedCosts = new jdouble[numVars];
//        for (jsize i = 0; i < numVars; i++) {
//            reducedCosts[i] = solution.col_dual[i];
//        }
//        env->SetDoubleArrayRegion(result, 0, numVars, reducedCosts);
//        delete[] reducedCosts;
//    }
//
//    return result;
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_getDualSolution(JNIEnv* env, jobject obj, jlong solverPtr) {
//    if (solverPtr == 0) return 0.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const HighsSolution& solution = solver->getSolution();
//
//    jsize numCons = solution.row_dual.size();
//    jdoubleArray result = env->NewDoubleArray(numCons);
//
//    if (result != nullptr) {
//        jdouble* dualValues = new jdouble[numCons];
//        for (jsize i = 0; i < numCons; i++) {
//            dualValues[i] = solution.row_dual[i];
//        }
//        env->SetDoubleArrayRegion(result, 0, numCons, dualValues);
//        delete[] dualValues;
//    }
//
//    return result;
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_getRowValues(JNIEnv* env, jobject obj, jlong solverPtr) {
//    if (solverPtr == 0) return 0.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const HighsSolution& solution = solver->getSolution();
//
//    jsize numCons = solution.row_value.size();
//    jdoubleArray result = env->NewDoubleArray(numCons);
//
//    if (result != nullptr) {
//        jdouble* rowValues = new jdouble[numCons];
//        for (jsize i = 0; i < numCons; i++) {
//            rowValues[i] = solution.row_value[i];
//        }
//        env->SetDoubleArrayRegion(result, 0, numCons, rowValues);
//        delete[] rowValues;
//    }
//
//    return result;
//}
//

JNIEXPORT jdouble JNICALL
Java_nl_jessenagel_jhighs_HiGHS_getObjectiveValue(JNIEnv* env, jobject obj, jlong solverPtr) {
    if (solverPtr == 0) return 0.0;

    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
    const HighsInfo& info = solver->getInfo();
    return info.objective_function_value;
}

JNIEXPORT jdoubleArray JNICALL
Java_nl_jessenagel_jhighs_HiGHS_getSolutionValues(JNIEnv* env, jobject obj, jlong solverPtr) {
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

//// Parameter Management Functions
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_getIntegerParameter(JNIEnv* env, jobject obj, jlong solverPtr, jstring paramName) {
//    if (solverPtr == 0) return -1.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const char* cParamName = env->GetStringUTFChars(paramName, NULL);
//
//    int value = solver->getIntParam(cParamName);
//
//    env->ReleaseStringUTFChars(paramName, cParamName);
//    return static_cast<jdouble>(value);
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_getDoubleParameter(JNIEnv* env, jobject obj, jlong solverPtr, jstring paramName) {
//    if (solverPtr == 0) return -1.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const char* cParamName = env->GetStringUTFChars(paramName, NULL);
//
//    double value = solver->getDoubleParam(cParamName);
//
//    env->ReleaseStringUTFChars(paramName, cParamName);
//    return value;
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_getStringParameter(JNIEnv* env, jobject obj, jlong solverPtr, jstring paramName) {
//    if (solverPtr == 0) return -1.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const char* cParamName = env->GetStringUTFChars(paramName, NULL);
//
//    std::string value = solver->getStringParam(cParamName);
//
//    env->ReleaseStringUTFChars(paramName, cParamName);
//    return env->NewStringUTF(value.c_str());
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_setIntegerParameter(JNIEnv* env, jobject obj, jlong solverPtr, jstring paramName, jint value) {
//    if (solverPtr == 0) return -1.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const char* cParamName = env->GetStringUTFChars(paramName, NULL);
//
//    HighsStatus status = solver->setIntParam(cParamName, value);
//
//    env->ReleaseStringUTFChars(paramName, cParamName);
//    return static_cast<jdouble>(status);
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_setDoubleParameter(JNIEnv* env, jobject obj, jlong solverPtr, jstring paramName, jdouble value) {
//    if (solverPtr == 0) return -1.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const char* cParamName = env->GetStringUTFChars(paramName, NULL);
//
//    HighsStatus status = solver->setDoubleParam(cParamName, value);
//
//    env->ReleaseStringUTFChars(paramName, cParamName);
//    return static_cast<jdouble>(status);
//}
//
//JNIEXPORT jdouble JNICALL
//Java_nl_jessenagel_jhighs_HiGHS_setStringParameter(JNIEnv* env, jobject obj, jlong solverPtr, jstring paramName, jstring value) {
//    if (solverPtr == 0) return -1.0;
//
//    Highs* solver = reinterpret_cast<Highs*>(solverPtr);
//    const char* cParamName = env->GetStringUTFChars(paramName, NULL);
//    const char* cValue = env->GetStringUTFChars(value, NULL);
//
//    HighsStatus status = solver->setStringParam(cParamName, cValue);
//
//    env->ReleaseStringUTFChars(paramName, cParamName);
//    env->ReleaseStringUTFChars(value, cValue);
//    return static_cast<jdouble>(status);
//}

} // extern "C"
