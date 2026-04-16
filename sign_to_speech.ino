#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64




Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
    <title>SignSpeak — Sign to Speech</title>
    <link rel="preconnect" href="https://fonts.googleapis.com" />
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
    <link href="https://fonts.googleapis.com/css2?family=Space+Mono:wght@400;700&family=Syne:wght@400;700;800&display=swap" rel="stylesheet" />
    <style>
      :root {
        --bg: #0a0e1a;
        --card: #111827;
        --accent: #00e5ff;
        --accent2: #ff6b35;
        --text: #e8eaf0;
        --muted: #4a5568;
        --border: #1e2d3d;
        --glow: 0 0 30px rgba(0, 229, 255, 0.15);
        --glow-strong: 0 0 60px rgba(0, 229, 255, 0.3);
      }

      * { margin: 0; padding: 0; box-sizing: border-box; }

      body {
        min-height: 100vh;
        background-color: var(--bg);
        background-image:
          radial-gradient(ellipse 80% 60% at 50% -10%, rgba(0,229,255,0.08) 0%, transparent 60%),
          radial-gradient(ellipse 40% 40% at 90% 90%, rgba(255,107,53,0.06) 0%, transparent 50%);
        font-family: 'Syne', sans-serif;
        color: var(--text);
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        padding: 20px;
        overflow-x: hidden;
      }

      /* ── Unmute Overlay ── */
      #unmute-overlay {
        position: fixed;
        inset: 0;
        background: rgba(10, 14, 26, 0.97);
        backdrop-filter: blur(10px);
        z-index: 1000;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        gap: 24px;
        transition: opacity 0.6s ease;
      }
      #unmute-overlay.hidden {
        opacity: 0;
        pointer-events: none;
      }
      .overlay-icon {
        font-size: 64px;
        animation: pulse-icon 2s ease-in-out infinite;
      }
      @keyframes pulse-icon {
        0%, 100% { transform: scale(1); }
        50% { transform: scale(1.1); }
      }
      .overlay-title {
        font-size: clamp(22px, 5vw, 32px);
        font-weight: 800;
        color: var(--accent);
        letter-spacing: 0.05em;
        text-align: center;
      }
      .overlay-sub {
        font-family: 'Space Mono', monospace;
        font-size: 13px;
        color: var(--muted);
        text-align: center;
        max-width: 280px;
        line-height: 1.6;
      }
      #unmute-btn {
        margin-top: 8px;
        padding: 16px 48px;
        background: var(--accent);
        color: #0a0e1a;
        font-family: 'Syne', sans-serif;
        font-size: 16px;
        font-weight: 800;
        border: none;
        border-radius: 50px;
        cursor: pointer;
        letter-spacing: 0.1em;
        text-transform: uppercase;
        box-shadow: 0 0 40px rgba(0,229,255,0.4);
        transition: transform 0.15s, box-shadow 0.15s;
      }
      #unmute-btn:hover {
        transform: scale(1.05);
        box-shadow: 0 0 60px rgba(0,229,255,0.6);
      }
      #unmute-btn:active { transform: scale(0.97); }

      /* ── Header ── */
      header {
        text-align: center;
        margin-bottom: 36px;
      }
      .logo-row {
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 12px;
        margin-bottom: 6px;
      }
      .logo-dot {
        width: 10px;
        height: 10px;
        border-radius: 50%;
        background: var(--accent);
        box-shadow: 0 0 12px var(--accent);
        animation: blink 2s ease-in-out infinite;
      }
      @keyframes blink {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.3; }
      }
      h1 {
        font-size: clamp(28px, 7vw, 48px);
        font-weight: 800;
        letter-spacing: -0.02em;
        color: var(--text);
      }
      h1 span { color: var(--accent); }
      .tagline {
        font-family: 'Space Mono', monospace;
        font-size: 11px;
        color: var(--muted);
        letter-spacing: 0.2em;
        text-transform: uppercase;
        margin-top: 4px;
      }

      /* ── Main Card ── */
      .card {
        width: 100%;
        max-width: 560px;
        background: var(--card);
        border: 1px solid var(--border);
        border-radius: 20px;
        padding: 32px;
        box-shadow: var(--glow);
        position: relative;
        overflow: hidden;
      }
      .card::before {
        content: '';
        position: absolute;
        top: 0; left: 0; right: 0;
        height: 2px;
        background: linear-gradient(90deg, transparent, var(--accent), transparent);
        opacity: 0.6;
      }

      .section-label {
        font-family: 'Space Mono', monospace;
        font-size: 10px;
        letter-spacing: 0.2em;
        text-transform: uppercase;
        color: var(--muted);
        margin-bottom: 12px;
      }

      /* ── Output Display ── */
      .output-box {
        background: #0d1520;
        border: 1px solid var(--border);
        border-radius: 12px;
        padding: 28px 24px;
        min-height: 100px;
        display: flex;
        align-items: center;
        justify-content: center;
        position: relative;
        margin-bottom: 24px;
        transition: border-color 0.3s, box-shadow 0.3s;
      }
      .output-box.speaking {
        border-color: var(--accent);
        box-shadow: 0 0 20px rgba(0,229,255,0.2);
      }
      #message {
        font-size: clamp(16px, 4vw, 22px);
        font-weight: 700;
        text-align: center;
        line-height: 1.4;
        color: var(--text);
        transition: opacity 0.3s;
      }
      #message.empty {
        color: var(--muted);
        font-weight: 400;
        font-size: 14px;
      }

      /* Speaking animation bars */
      .sound-bars {
        position: absolute;
        right: 16px;
        bottom: 16px;
        display: flex;
        gap: 3px;
        align-items: flex-end;
        height: 20px;
        opacity: 0;
        transition: opacity 0.3s;
      }
      .output-box.speaking .sound-bars { opacity: 1; }
      .bar {
        width: 3px;
        background: var(--accent);
        border-radius: 2px;
        animation: bar-anim 0.8s ease-in-out infinite;
      }
      .bar:nth-child(1) { height: 8px; animation-delay: 0s; }
      .bar:nth-child(2) { height: 16px; animation-delay: 0.15s; }
      .bar:nth-child(3) { height: 12px; animation-delay: 0.3s; }
      .bar:nth-child(4) { height: 20px; animation-delay: 0.1s; }
      .bar:nth-child(5) { height: 10px; animation-delay: 0.25s; }
      @keyframes bar-anim {
        0%, 100% { transform: scaleY(0.4); }
        50% { transform: scaleY(1); }
      }

      /* ── Status Row ── */
      .status-row {
        display: flex;
        align-items: center;
        justify-content: space-between;
        flex-wrap: wrap;
        gap: 12px;
        margin-bottom: 24px;
      }
      .status-chip {
        display: flex;
        align-items: center;
        gap: 6px;
        font-family: 'Space Mono', monospace;
        font-size: 11px;
        padding: 6px 12px;
        border-radius: 50px;
        border: 1px solid var(--border);
        background: #0d1520;
      }
      .status-dot {
        width: 6px;
        height: 6px;
        border-radius: 50%;
        background: var(--muted);
      }
      .status-chip.online .status-dot {
        background: #22c55e;
        box-shadow: 0 0 8px #22c55e;
        animation: blink 2s infinite;
      }
      .status-chip.speaking-chip .status-dot {
        background: var(--accent);
        box-shadow: 0 0 8px var(--accent);
        animation: blink 0.6s infinite;
      }
      #binary-code {
        font-family: 'Space Mono', monospace;
        font-size: 13px;
        font-weight: 700;
        color: var(--accent2);
        letter-spacing: 0.15em;
      }

      /* ── Gesture Grid ── */
      .grid-title {
        margin-bottom: 14px;
      }
      .gesture-grid {
        display: grid;
        grid-template-columns: repeat(auto-fill, minmax(130px, 1fr));
        gap: 8px;
      }
      .gesture-item {
        background: #0d1520;
        border: 1px solid var(--border);
        border-radius: 10px;
        padding: 10px 12px;
        display: flex;
        align-items: center;
        gap: 10px;
        transition: border-color 0.2s, background 0.2s;
        cursor: default;
      }
      .gesture-item.active {
        border-color: var(--accent);
        background: rgba(0, 229, 255, 0.06);
      }
      .gesture-code {
        font-family: 'Space Mono', monospace;
        font-size: 10px;
        color: var(--accent2);
        min-width: 34px;
      }
      .gesture-label {
        font-size: 11px;
        color: var(--muted);
        line-height: 1.3;
        transition: color 0.2s;
      }
      .gesture-item.active .gesture-label { color: var(--text); }

      /* ── Footer ── */
      footer {
        margin-top: 32px;
        text-align: center;
        font-family: 'Space Mono', monospace;
        font-size: 10px;
        color: var(--muted);
        letter-spacing: 0.1em;
      }
      footer span { color: var(--accent); }
    </style>
  </head>
  <body>

    <!-- Unmute Overlay -->
    <div id="unmute-overlay">
      <div class="overlay-icon">🔊</div>
      <div class="overlay-title">Enable Voice Output</div>
      <div class="overlay-sub">Tap below to activate audio. This allows the device to speak detected gestures aloud.</div>
      <button id="unmute-btn" onclick="activateAudio()">TAP TO ENABLE</button>
    </div>

    <!-- Header -->
    <header>
      <div class="logo-row">
        <div class="logo-dot"></div>
        <h1>Gesture<span>Talk</span></h1>
        <div class="logo-dot"></div>
      </div>
      <p class="tagline">Intelligent sign to voice communication system</p>
    </header>

    <!-- Main Card -->
    <div class="card">

      <!-- Output -->
      <p class="section-label">// detected output</p>
      <div class="output-box" id="output-box">
        <div id="message" class="empty">Waiting for gesture...</div>
        <div class="sound-bars">
          <div class="bar"></div>
          <div class="bar"></div>
          <div class="bar"></div>
          <div class="bar"></div>
          <div class="bar"></div>
        </div>
      </div>

      <!-- Status -->
      <div class="status-row">
        <div class="status-chip online">
          <div class="status-dot"></div>
          <span>DEVICE ONLINE</span>
        </div>
        <div style="display:flex; align-items:center; gap:8px;">
          <span style="font-family:'Space Mono',monospace;font-size:10px;color:var(--muted)">INPUT</span>
          <div id="binary-code">0000</div>
        </div>
        <div class="status-chip" id="speak-status">
          <div class="status-dot"></div>
          <span id="speak-label">IDLE</span>
        </div>
      </div>

      <!-- Gesture Reference -->
      <p class="section-label grid-title">// gesture map</p>
      <div class="gesture-grid" id="gesture-grid">
        <!-- filled by JS -->
      </div>

    </div>

    <footer>Final Year Project &nbsp;·&nbsp; <span>ESP32</span> Sign-to-Speech &nbsp;·&nbsp; 2026</footer>

    <script>
      const gestureMap = {
        "0001": "I want to go to the washroom",
        "0010": "I need water",
        "0011": "I am hungry",
        "0100": "I need help",
        "0101": "Please call someone",
        "0110": "I feel pain",
        "0111": "I want to rest",
        "1000": "Please give me food",
        "1001": "I want to go outside",
        "1010": "Please bring my phone",
        "1011": "I am feeling cold",
        "1100": "I am feeling hot",
        "1101": "Please turn on the light",
        "1110": "Please turn off the light",
        "1111": "Thank you"
      };

      // Build gesture grid
      const grid = document.getElementById("gesture-grid");
      for (const [code, label] of Object.entries(gestureMap)) {
        const item = document.createElement("div");
        item.className = "gesture-item";
        item.id = "g-" + code;
        item.innerHTML = `<span class="gesture-code">${code}</span><span class="gesture-label">${label}</span>`;
        grid.appendChild(item);
      }

      let lastSpoken = "";
      let audioUnlocked = false;
      const outputBox = document.getElementById("output-box");
      const messageEl = document.getElementById("message");
      const binaryEl = document.getElementById("binary-code");
      const speakStatus = document.getElementById("speak-status");
      const speakLabel = document.getElementById("speak-label");

      function activateAudio() {
        audioUnlocked = true;
        // Speak a silent utterance to unlock the audio context
        const unlock = new SpeechSynthesisUtterance(" ");
        unlock.volume = 0;
        window.speechSynthesis.speak(unlock);

        const overlay = document.getElementById("unmute-overlay");
        overlay.classList.add("hidden");
        setTimeout(() => overlay.style.display = "none", 700);
      }

      function highlightGesture(code) {
        document.querySelectorAll(".gesture-item").forEach(el => el.classList.remove("active"));
        const target = document.getElementById("g-" + code);
        if (target) target.classList.add("active");
      }

      function setSpeaking(isSpeaking) {
        if (isSpeaking) {
          outputBox.classList.add("speaking");
          speakStatus.classList.add("speaking-chip");
          speakLabel.textContent = "SPEAKING";
        } else {
          outputBox.classList.remove("speaking");
          speakStatus.classList.remove("speaking-chip");
          speakLabel.textContent = "IDLE";
        }
      }

      function speakText(text) {
        if (!audioUnlocked) return;
        window.speechSynthesis.cancel();
        const utt = new SpeechSynthesisUtterance(text);
        utt.lang = "en-IN";
        utt.rate = 0.95;
        utt.onstart = () => setSpeaking(true);
        utt.onend = () => setSpeaking(false);
        utt.onerror = () => setSpeaking(false);
        window.speechSynthesis.speak(utt);
      }

      async function fetchSign() {
        try {
          const response = await fetch("/sign");
          const data = await response.text();

          // Update binary display — extract from ESP or parse from text
          // We'll request binary separately via /binary endpoint
          // For now, reverse-lookup binary from text
          const binary = Object.keys(gestureMap).find(k => gestureMap[k] === data) || "0000";
          binaryEl.textContent = binary;
          highlightGesture(binary);

          if (data && data !== lastSpoken) {
            lastSpoken = data;
            messageEl.classList.remove("empty");
            messageEl.textContent = data;

            // Fade in effect
            messageEl.style.opacity = "0";
            setTimeout(() => { messageEl.style.opacity = "1"; }, 50);

            speakText(data);
          } else if (!data) {
            messageEl.classList.add("empty");
            messageEl.textContent = "Waiting for gesture...";
            binaryEl.textContent = "0000";
          }
        } catch (e) {
          console.error("Fetch error:", e);
        }
      }

      setInterval(fetchSign, 1000);
    </script>
  </body>
</html>
)====";

/*========================================= Handlers ===================================*/



  void showText(String msg) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.print(msg);
    display.display();
  }




void handleRoot() {
  server.send(200, "text/html", webpage);
}

void getSign() {



  int S0 = analogRead(35);
  int S1 = analogRead(34);
  int S2 = analogRead(33);
  int S3 = analogRead(32);

  S0 /= 100;
  S1 /= 100;
  S2 /= 100;
  S3 /= 100;



  if (S3 >= 8) {
    S3 = 1;
  } else {
    {
      S3 = 0;
    }
  }

  if (S2 >= 4) {
    S2 = 1;
  } else {
    {
      S2 = 0;
    }
  }

  if (S1 >= 2) {
    S1 = 1;
  } else {
    {
      S1 = 0;
    }
  }

  if (S0 >= 8) {
    S0 = 1;
  } else {
    S0 = 0;
  }


  Serial.print(S3);
  Serial.print(" ");

  Serial.print(S2);
  Serial.print(" ");

  Serial.print(S1);
  Serial.print(" ");

  Serial.println(S0);
  delay(200);

  String input = (String)S3 + (String)S2 + (String)S1 + (String)S0;
  Serial.println(input);

  String text = "";
  if (input == "0001") text = "I want to go to the washroom";
  else if (input == "0010") text = "I need water";
  else if (input == "0011") text = "I am hungry";
  else if (input == "0100") text = "I need help";
  else if (input == "0101") text = "Please call someone";
  else if (input == "0110") text = "I feel pain";
  else if (input == "0111") text = "I want to rest";
  else if (input == "1000") text = "Please give me food";
  else if (input == "1001") text = "I want to go outside";
  else if (input == "1010") text = "Please bring my phone";
  else if (input == "1011") text = "I am feeling cold";
  else if (input == "1100") text = "I am feeling hot";
  else if (input == "1101") text = "Please turn on the light";
  else if (input == "1110") text = "Please turn off the light";
  else if (input == "1111") text = "";
  else if (input == "0000") text = "Hello";
  else text = "";

  if (text != "") showText(text);
  server.send(200, "text/plain", text);
}

/*=====================================================================================*/

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(500);

  // pinMode(15, INPUT_PULLUP);
  // pinMode(17, INPUT_PULLUP);
  // pinMode(4, INPUT_PULLUP);
  // pinMode(16, INPUT_PULLUP);


  WiFi.begin(ssid, password);
  showText("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());
  showText(WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/sign", getSign);

  server.begin();
}

void loop() {
  server.handleClient();
}
