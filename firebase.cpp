#define ENABLE_SERVICE_AUTH
#define ENABLE_USER_AUTH
#define ENABLE_FIRESTORE
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include <limits>

#include "firebase.h"
#include "ExampleFunctions.h"
#include "ntp.h"
#include "arduino_secrets.h"

const int DATA_SEND_PERIOD = 1000;

unsigned long lastDataSend = 0;

UserAuth user_auth(
  SECRET_UA_API_KEY,
  SECRET_UA_EMAIL,
  SECRET_UA_PASS,
  3000);

//ServiceAuth sa_auth(
//  SECRET_GCP_FS_SA,
//  SECRET_GCP_PROJECT_ID,
//  SECRET_GCP_FS_PK,
//  3000); // expire period in seconds (<= 3600)

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient asyncClient(ssl_client);

RealtimeDatabase Database;
AsyncResult firebaseResult;

uint32_t getNtpTime32() {
  uint32_t ntp32Time;
  unsigned long ntpTime = getNtpTime();

  Serial.print("raw ntp time: ");
  Serial.println(ntpTime);

  if (ntpTime > std::numeric_limits<uint32_t>::max()) {
      Serial.println("Warning: value too large for uint32_t");
      ntp32Time = static_cast<uint32_t>(ntpTime);
  } else {
      ntp32Time = static_cast<uint32_t>(ntpTime);
  }
  
  Serial.println(ntp32Time);
  return ntp32Time;
}

void setupFirebase() {
  set_ssl_client_insecure_and_buffer(ssl_client);

  app.setTime(getNtpTime32());
  
  Serial.println("initializing firebase app...");
  initializeApp(
    asyncClient,
    app,
    getAuth(user_auth),
    auth_debug_print,
    "authTask"); 

  app.getApp<RealtimeDatabase>(Database);
  Database.url(SECRET_RT_DB_URL);
}

void handleResult(AsyncResult &aResult) {
  if (!aResult.isResult()) {
    return;
  }
  if (aResult.isEvent()) {
    //Firebase.printf(
    //  "Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(),
    //  aResult.eventLog().message().c_str(), aResult.eventLog().code());
  }
  if (aResult.isDebug()) {
    Firebase.printf( "Debug task: %s, msg: %s\n",
      aResult.uid().c_str(), aResult.debug().c_str());
  }
  if (aResult.isError()) {
    Firebase.printf( "Error task: %s, msg: %s, code: %d\n",
      aResult.uid().c_str(), aResult.error().message().c_str(),
      aResult.error().code());
  }
  if (aResult.available()) {
    //Firebase.printf( "task: %s, payload: %s\n",
    //  aResult.uid().c_str(), aResult.c_str());
  }
}

void pushRealtime(float windSpeed) {

  if (!app.ready()) {
    Serial.println("app not ready");
    return;
  }

  Database.set<number_t>(
    asyncClient,
    SECRET_RT_DB_WINDSPEED_PATH,
    number_t(windSpeed),
    firebaseResult);
}

void firebaseLoopStart() {
  app.loop();
}

void firebaseLoopEnd() {
  handleResult(firebaseResult);
}

void firebaseLoop(unsigned long millis, float windSpeed) {
  if (millis - lastDataSend >= DATA_SEND_PERIOD && app.ready()) {
    lastDataSend = millis;
    pushRealtime(windSpeed);
  }
}