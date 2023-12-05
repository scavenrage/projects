void Wirelesscon() {
  lampeggio();
  //disconnetto da eventuali reti memorizzate (FONDAMENTALE!!, se omesso non si connette alla rete!)
  Serial.println("mi disconnetto prima");
  WiFi.disconnect();
  delay(20);
  Serial.println("inizio connessione");
  WiFi.begin(ssid, password);
  delay(10);
  Serial.println("Connessione in corso");
  int count = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    count++;
    delay(100);
    Serial.print(".");
    if (count == 100) {
      digitalWrite(ledstato, HIGH);
      break;
    }
  }
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Stato WiFi: ");
    Serial.println(WiFi.waitForConnectResult());
    Serial.println("");
    Serial.println("Connesso al WiFi");
    Serial.println(WiFi.localIP());
    Serial.println("termino mdns prima di farlo partire");
    MDNS.end();
    Serial.println("attendo 2 secondi");
    delay(2000);
    Serial.println("avvia mdns");
    if (!MDNS.begin(nome)) {
      // Start the mDNS responder for esp8266.local
      Serial.println("Errore nel settaggio di mDNS!");
    }
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    Serial.println("mDNS partito");
    MDNS.addService("http", "tcp", 80);
  }
  Serial.println("webserver avviato");
  //richiamo la funzione connect per connettermi al server Raspberry e iscrivermi al Topic in ingresso
  connect();
}

void connect() {
  lampeggio();
  Serial.println("mi connetto al broker");
  //mi connetto al server Raspberry
  client.begin(broker, net);

  for (int i = 0; i < 10; i++) {
    if (!client.connect(nome)) {
      Serial.print(".");
      if (i == 9) {
        Serial.println("");
        Serial.println("Indirizzo IP Broker non corretto");
      }
    } else {
      Serial.println("Connesso al broker");
      digitalWrite(ledstato, LOW);
      client.subscribe(inTopic);
      break;
    }
  }
  Serial.println("");
}

void vercon() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Non connesso al Wi-fi, mi riconnetto...");
    Wirelesscon();
    Serial.println("");
    Serial.println("attendo 10 secondi...");
    delay(10000);
  } else if (!client.connected()) {
    digitalWrite(ledstato, HIGH);
    Serial.println("");
    Serial.println("Non connesso al broker, mi riconnetto fra 10 secondi...");
    delay(10000);
    connect();
  }
}

void lampeggio() {
  digitalWrite(ledstato, !digitalRead(ledstato));
  delay(500);
  digitalWrite(ledstato, !digitalRead(ledstato));
  delay(500);
  digitalWrite(ledstato, !digitalRead(ledstato));
  delay(500);
  digitalWrite(ledstato, !digitalRead(ledstato));
  delay(500);
}

void pagina() {
  httpServer.on("/", []() {
    numero = httpServer.arg("numero");
    httpServer.send(200, "text/html", nome);
  });



  httpServer.begin();
}
