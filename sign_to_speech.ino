#include <WiFi.h>
#include <WebServer.h>

String ssid = "S23";
String password = "passward";

WebServer server(80);

/*========================================== Webpage =====================================*/

String webpage = R"====(
    <!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>sts</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }
      body {
        width: 100%;
        height: 100vh;
        background-color: white;
        display: flex;
        justify-content: center;
        align-items: center;
      }
      .display {
        background-color: #f67d31;
        font-size: 25px;
        padding: 20px;
        border-radius: 10px;
        color: black;
        font-family: "Courier New", Courier, monospace;
        font-weight: 800;
      }
    </style>
  </head>
  <body>
    <div>
      <div class="display"></div>
    </div>
    <script>
      let display = document.querySelector(".display");

      function setDisplay(text) {
        display.innerText = text;
      }
      function textTOSpeech() {
        let text = display.innerText;
        let speech = new SpeechSynthesisUtterance(text);
        speech.lang = "en-IN";
        window.speechSynthesis.speak(speech);
      }
      setInterval(async () => {
        let response = await fetch("/sign");
        let data = await response.text();
        setDisplay(data);
        textTOSpeech();
      }, 2000);
    </script>
  </body>
</html>

  )====";

/*========================================= Handlers ===================================*/

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void getSign() {
  String text = "";
  if (digitalRead(5) == 0) {
    text = "water";
  }
  server.send(200, "text/plain", text);
}
/*=====================================================================================*/

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(5, INPUT_PULLUP);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println("\nconnected");
  Serial.println(WiFi.localIP());


  /*===================================== controlling fxn ==================================*/


  server.on("/", handleRoot);
  server.on("/sign", getSign);


  /*======================================================================================*/
  server.begin();
}
void loop() {
  server.handleClient();
}