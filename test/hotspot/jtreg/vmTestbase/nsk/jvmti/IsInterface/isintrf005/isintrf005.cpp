/*
 * Copyright (c) 2003, 2024, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <stdio.h>
#include <string.h>
#include "jvmti.h"
#include "agent_common.hpp"
#include "JVMTITools.h"

extern "C" {


#define PASSED 0
#define STATUS_FAILED 2

static jvmtiEnv *jvmti = nullptr;
static jint result = PASSED;
static jboolean printdump = JNI_FALSE;

#ifdef STATIC_BUILD
JNIEXPORT jint JNICALL Agent_OnLoad_isintrf005(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNICALL Agent_OnAttach_isintrf005(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNI_OnLoad_isintrf005(JavaVM *jvm, char *options, void *reserved) {
    return JNI_VERSION_1_8;
}
#endif
jint Agent_Initialize(JavaVM *jvm, char *options, void *reserved) {
    jint res;

    if (options != nullptr && strcmp(options, "printdump") == 0) {
        printdump = JNI_TRUE;
    }

    res = jvm->GetEnv((void **) &jvmti, JVMTI_VERSION_1_1);
    if (res != JNI_OK || jvmti == nullptr) {
        printf("Wrong result of a valid call to GetEnv!\n");
        return JNI_ERR;
    }

    return JNI_OK;
}

char const *jbooleanToString(jboolean flag) {
    return ((flag == JNI_TRUE) ? "true" : "false");
}

JNIEXPORT void JNICALL
Java_nsk_jvmti_IsInterface_isintrf005_check(JNIEnv *env, jclass cls, jclass clazz, jboolean flag) {
    jvmtiError err;
    jboolean isInterface;
    char *sig, *generic;

    if (jvmti == nullptr) {
        printf("JVMTI client was not properly loaded!\n");
        result = STATUS_FAILED;
        return;
    }

    err = jvmti->GetClassSignature(clazz, &sig, &generic);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetClassSignature) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
        return;
    }

    err = jvmti->IsInterface(clazz, &isInterface);
    if (err != JVMTI_ERROR_NONE) {
        printf("(IsInterface) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
    }

    if (printdump == JNI_TRUE) {
        printf(">>> %s - %s\n", sig, jbooleanToString(isInterface));
    }

    if (isInterface != flag) {
        printf("(%s) wrong isInterface value: %s, expected: %s\n",
               sig, jbooleanToString(isInterface), jbooleanToString(flag));
        result = STATUS_FAILED;
    }
}

JNIEXPORT int JNICALL Java_nsk_jvmti_IsInterface_isintrf005_getRes(JNIEnv *env, jclass cls) {
    return result;
}

}
