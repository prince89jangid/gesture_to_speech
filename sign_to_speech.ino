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
      }, 1000);
    </script>
  </body>
</html>

  )====";

/*========================================= Handlers ===================================*/

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void getSign() {

  //  d c b a
  int a = digitalRead(18);
  int b = digitalRead(5);
  int c = digitalRead(17);
  int d = digitalRead(16);

  String input = (String)d + (String)c + (String)b + (String)a;

  Serial.println(input);
  String text = "";

  if (input == "0001") {
    text = "I want to go to the washroom";
  } else if (input == "0010") {
    text = "I need water";

  } else if (input == "0011") {
    text = "I am hungry";
  } else if (input == "0100") {
    text = "I need help";

  } else if (input == "0101") {
    text = "Please call someone";
  } else if (input == "0110") {
    text = "I feel pain";
  } else if (input == "0111") {
    text = "I want to rest";
  } else if (input == "1000") {
    text = "Please give me food";
  } else if (input == "1001") {
    text = "I want to go outside";
  } else if (input == "1010") {
    text = "Please bring my phone";

  } else if (input == "1011") {
    text = "I am feeling cold";
  } else if (input == "1100") {
    text = "I am feeling hot";
  } else if (input == "1101") {
    text = "Please turn on the light";

  } else if (input == "1110") {
    text = "Please turn off the light";
  } else if (input == "1111") {
    text = "thankyou";
  } else {
    text = "";
  }


  server.send(200, "text/plain", text);

}
/*=====================================================================================*/

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(18, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);

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
