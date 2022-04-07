#include "waffle.h"

#include "Cache.hpp"
#include "FrequencySmoother.hpp"

#include <cstdint>

#define FREQUENCY_FILE_PATH "/home/narahari387/Desktop/freqFile.txt"
#define KEY_FILE_PATH "/home/narahari387/Desktop/waffle/proxy/secretKeys/sodiumKey"

FrequencySmoother *fs;
Cache *cache;

JNIEXPORT void JNICALL Java_site_ycsb_db_waffle_WaffleClient_nativeInit(JNIEnv *, jobject, jint maxDeltaRead, jint maxDeltaWrite, jint cacheSize)
{
	printf("----------------\nInitializing Waffle\n----------------\n");
	fs = new FrequencySmoother((int)maxDeltaRead, (int)maxDeltaWrite, FREQUENCY_FILE_PATH, KEY_FILE_PATH);
	cache = new Cache((int)cacheSize, fs);
}

JNIEXPORT void JNICALL Java_site_ycsb_db_waffle_WaffleClient_nativeCleanup(JNIEnv *, jobject)
{
	printf("----------------\nCleaning up Waffle\n");
	cache->printStatistics();
	cache->flush();
	fs->serialize(FREQUENCY_FILE_PATH);
	fs->printStatistics();
	printf("----------------\n");
	delete fs;
	delete cache;
}

JNIEXPORT jint JNICALL Java_site_ycsb_db_waffle_WaffleClient_nativeCreate(JNIEnv * env, jobject, jstring jkey, jbyteArray value)
{
	uint32_t size = env->GetArrayLength(value);
	jbyte* valuePtr = env->GetByteArrayElements(value, 0);

	char* data = (char*)malloc(size);
	memcpy(data, valuePtr, size);

	env->ReleaseByteArrayElements(value, valuePtr, 0);

	jboolean isCopy = false;
	uint8_t keySize = env->GetStringLength(jkey);
	const char* keyChars = env->GetStringUTFChars(jkey, &isCopy);
	std::string key(keyChars, keySize);
	env->ReleaseStringUTFChars(jkey, keyChars);

	Record* record = new Record(key, data, size);
	cache->put(record);

	return 1;
}

JNIEXPORT jbyteArray JNICALL Java_site_ycsb_db_waffle_WaffleClient_nativeRead(JNIEnv * env, jobject, jstring jkey)
{
	jboolean isCopy = false;
	uint8_t keySize = env->GetStringLength(jkey);
	const char* keyChars = env->GetStringUTFChars(jkey, &isCopy);
	std::string key(keyChars, keySize);
	env->ReleaseStringUTFChars(jkey, keyChars);

	Record* record = cache->get(key);

	jbyteArray jData = (env)->NewByteArray(record->size);
	env->SetByteArrayRegion(jData, 0, record->size, (jbyte*)record->data);

	return jData;
}

JNIEXPORT jint JNICALL Java_site_ycsb_db_waffle_WaffleClient_nativeUpdate(JNIEnv * env, jobject, jstring jkey, jbyteArray value)
{
	uint32_t size = env->GetArrayLength(value);
	jbyte* valuePtr = env->GetByteArrayElements(value, 0);

	char* data = (char*)malloc(size);
	memcpy(data, valuePtr, size);

	env->ReleaseByteArrayElements(value, valuePtr, 0);

	jboolean isCopy = false;
	uint8_t keySize = env->GetStringLength(jkey);
	const char* keyChars = env->GetStringUTFChars(jkey, &isCopy);
	std::string key(keyChars, keySize);
	env->ReleaseStringUTFChars(jkey, keyChars);

	Record* record = new Record(key, data, size);
	cache->put(record);

	return 1;
}