/* Copyright 1998, 2005 The Apache Software Foundation or its licensors, as applicable
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(nethelp_h)
#define nethelp_h
#include "iohelp.h"
#include "exceptions.h"
#include "hysocket.h"
#define JAVASOCKOPT_TCP_NODELAY 1
#define JAVASOCKOPT_SO_REUSEADDR 4
#define JAVASOCKOPT_MCAST_ADD_MEMBERSHIP 19
#define JAVASOCKOPT_MCAST_DROP_MEMBERSHIP 20
#define JAVASOCKOPT_MCAST_TTL 17
#define JAVASOCKOPT_SO_KEEPALIVE 8
#define JAVASOCKOPT_MCAST_TIME_TO_LIVE 10       /* Currently unused */
#define JAVASOCKOPT_SO_BROADCAST 32
#define JAVASOCKOPT_SO_BINDADDR 15
#define JAVASOCKOPT_MCAST_INTERFACE 16
#define JAVASOCKOPT_SO_LINGER 128
#define JAVASOCKOPT_SO_REUSEPORT 512
#define JAVASOCKOPT_SO_SNDBUF 4097
#define JAVASOCKOPT_SO_RCVBUF 4098
#define JAVASOCKOPT_SO_RCVTIMEOUT  4102
#define JAVASOCKOPT_IP_TOS 3
#define JAVASOCKOPT_IP_MULTICAST_LOOP 18
#define JAVASOCKOPT_IP_MULTICAST_IF2 31
#define JAVASOCKOPT_SO_OOBINLINE  4099
#define JAVASOCKOPT_REUSEADDR_AND_REUSEPORT  10001


void *
getJavaIoFileDescriptorContentsAsAPointer (JNIEnv * env, jobject fd);
void throwJavaNetBindException (JNIEnv * env, I_32 errorNumber);
jobject newJavaNetInetAddressGenericBS (JNIEnv * env, jbyte * address,
          U_32 length, char *hostName,
          U_32 scope_id);
void throwJavaNetUnknownHostException (JNIEnv * env, I_32 errorNumber);
jobject newJavaNetInetAddressGenericB (JNIEnv * env, jbyte * address,
               U_32 length, U_32 scope_id);
jobject newJavaLangByte (JNIEnv * env, U_8 aByte);
U_8 byteValue (JNIEnv * env, jobject aByte);
I_32 intValue (JNIEnv * env, jobject anInteger);
void throwJavaNetPortUnreachableException (JNIEnv * env, I_32 errorNumber);
jobject newJavaByteArray (JNIEnv * env, jbyte * bytes, jint length);
jobjectArray createAliasArrayFromAddrinfo (JNIEnv * env,
             hyaddrinfo_t addresses,
             char *hName);
BOOLEAN booleanValue (JNIEnv * env, jobject aBoolean);
BOOLEAN jcl_supports_ipv6 (JNIEnv * env);
jobject newJavaLangInteger (JNIEnv * env, I_32 anInt);
BOOLEAN preferIPv4Stack (JNIEnv * env);
char *netLookupErrorString (JNIEnv * env, I_32 anErrorNum);
void netInitializeIDCaches (JNIEnv * env, jboolean ipv6_support);
jobject newJavaLangBoolean (JNIEnv * env, BOOLEAN aBool);
void throwJavaLangIllegalArgumentException (JNIEnv * env, I_32 errorNumber);
void netGetJavaNetInetAddressValue (JNIEnv * env, jobject anInetAddress,
            U_8 * buffer, U_32 * length);
void throwJavaIoInterruptedIOException (JNIEnv * env, I_32 errorNumber);
void throwJavaNetSocketTimeoutException (JNIEnv * env, I_32 errorNumber);
void callThreadYield (JNIEnv * env);
void throwJavaNetConnectException (JNIEnv * env, I_32 errorNumber);
void netGetJavaNetInetAddressScopeId (JNIEnv * env, jobject anInetAddress,
              U_32 * scope_id);
BOOLEAN preferIPv6Addresses (JNIEnv * env);
jobjectArray createAliasArray (JNIEnv * env, jbyte ** addresses,
             I_32 * family, U_32 count, char *hName,
             U_32 * scope_id_array);
void throwJavaNetSocketException (JNIEnv * env, I_32 errorNumber);
I_32 netGetSockAddr (JNIEnv * env, jobject fileDescriptor,
         hysockaddr_t sockaddrP, jboolean preferIPv6Addresses);
         
         jclass 
getJavaLangBooleanClass(JNIEnv * env);

jmethodID
getJavaLangBooleanInit(JNIEnv * env);

jfieldID
getJavaLangBooleanValue(JNIEnv * env);

jclass
getJavaLangByteClass(JNIEnv * env);

jmethodID
getJavaLangByteInit(JNIEnv * env);

jfieldID
getJavaLangByteValue(JNIEnv * env);

jclass
getJavaLangIntegerClass(JNIEnv * env);

jmethodID
getJavaLangIntegerInit(JNIEnv * env);

jfieldID
getJavaLangIntegerValue(JNIEnv * env);

jclass
getJavaNetInetAddressClass(JNIEnv * env);

jfieldID
getJavaNetInetAddressIpaddress(JNIEnv * env);

jmethodID
getJavaNetInetAddressPreferIPv6Addresses(JNIEnv * env);

jmethodID
getJavaNetInetAddressGetByAddressStringByte(JNIEnv * env);

jmethodID
getJavaNetInetAddressGetByAddressByteArray(JNIEnv * env);
jmethodID
getJavaNetInetAddressInitByteArray(JNIEnv * env);

jmethodID
getJavaNetInetAddressInitByteString(JNIEnv * env);

jclass
getJavaNetSocketClass(JNIEnv * env);

jmethodID
getJavaNetSocketPreferIPv4Stack(JNIEnv * env);

jclass
getJavaLangThreadClass(JNIEnv * env);

jmethodID
getJavaLangThreadYield(JNIEnv * env);

jclass
getJavaNetDatagramPacketClass(JNIEnv * env);

jfieldID
getJavaNetDatagramPacketAddress(JNIEnv * env);

jfieldID
getJavaNetDatagramPacketLength(JNIEnv * env);

jfieldID
getJavaNetDatagramPacketPort(JNIEnv * env);

jfieldID
getJavaNetSocketImplAddress(JNIEnv * env);

jfieldID
getJavaNetSocketImplPort(JNIEnv * env);

void
setJavaIoFileDescriptorContents (JNIEnv * env, jobject fd,
                                          void *value);
#endif /* nethelp_h */
