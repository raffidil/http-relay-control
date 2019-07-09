bool serverSetup() {
  Serial.println("Server setup");
  // Start WifiManager
  wifiManager.setSTAStaticIPConfig(IPAddress(192, 168, 31, 200), IPAddress(192, 168, 31, 1), IPAddress(255, 255, 255, 0));
  delay(1000);
  wifiManager.autoConnect("Relay");
  Serial.println("connected :)");

  // Start Server
  Serial.println("Server started");
  server.begin();

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void serverLoop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("main new client");

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one

  if (request.indexOf("/relay") != -1) {
    Serial.println("relay enabled");
    

    if (state == LOW) {
      Serial.println("Relays off");
      client.println("relays turned off");
      state = HIGH;
      digitalWrite(PIN_RELAY1, state);
      digitalWrite(PIN_RELAY2, state);
    } else {
      Serial.println("Relays on");
      client.println("relays turned on");
      state = LOW;
      digitalWrite(PIN_RELAY1, state);
      digitalWrite(PIN_RELAY2, state);

    }
    
  }

  if (request.indexOf("/timeron") != -1) {
    Serial.print("timer ON set: ");
    client.print("timer ON set: ");
    char tmr[4];
    memcpy( tmr, &request[12], 3 );
    tmr[4] = '\0';
    int relayTimer = strtol( tmr, NULL, 10);
    Serial.println(relayTimer);
    client.println(relayTimer);

    int afterEvent = t.after(relayTimer * 1000 * 60, doAfterOn);
    digitalWrite(PIN_LED_ON, LOW);
    timerQueue.push(afterEvent);
    Serial.print("Timer ON event started id=");
    Serial.println(afterEvent);

  }

  if (request.indexOf("/timeroff") != -1) {
    Serial.print("timer OFF set: ");
    client.print("timer OFF set: ");
    char tmr[4];
    memcpy( tmr, &request[13], 3 );
    tmr[4] = '\0';
    int relayTimer = strtol( tmr, NULL, 10);
    Serial.println(relayTimer);
    client.println(relayTimer);

    int afterEvent = t.after(relayTimer * 1000 * 60, doAfterOff);
    digitalWrite(PIN_LED_OFF, LOW);
    timerQueue.push(afterEvent);
    Serial.print("Timer OFF event started id=");
    Serial.println(afterEvent);

  }

  if (request.indexOf("/stoptimers") != -1) {
    Serial.print("Stopping All timers...");
    client.print("Stopping All timers...");

    while(!timerQueue.empty()){
      t.stop(timerQueue.front());
      timerQueue.pop();
    }
    digitalWrite(PIN_LED_ON, HIGH);
    digitalWrite(PIN_LED_OFF, HIGH);
    Serial.print("All timers stoped");
    client.print("All timers stoped");

  }


  if (request.indexOf("/wifireset") != -1) {
    Serial.println("reset wifi");
    client.println("reset wifi");
    wifiManager.resetSettings();
  }

}
