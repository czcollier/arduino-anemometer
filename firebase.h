#ifndef FIREBASE_H
#define FIREBASE_H

#include "arduino_secrets.h"

const char SECRET_GCP_FS_PK[] PROGMEM = {SECRET_SA_KEY};

class AsyncResult; //forward decl.

void processData(AsyncResult &aResult);

void setupFirebase();

void pushRealtime(float windSpeed);

void firebaseLoop(unsigned long millis, float windSpeed);

void firebaseLoopStart();

void firebaseLoopEnd();

#endif //FIREBASE_H