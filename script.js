let display = document.querySelector("#display");
let btn = document.querySelector("#btn");

function convert() {
  console.log("i am start");
  let text = display.value;
  let speech = new SpeechSynthesisUtterance(text);
  speech.lang = "en-IN";
  window.speechSynthesis.speak(speech);
  console.log("i am finish");
}
btn.addEventListener('click',convert);

// setInterval(convert, 1000);
