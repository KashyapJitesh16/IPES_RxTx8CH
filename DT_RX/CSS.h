String webpage = ""; //String to save the html code

void append_page_header() {
  webpage  = F("<!DOCTYPE html><html>");
  webpage += F("<head>");
  webpage += F("<title>MC Server</title>"); // NOTE: 1em = 16px
  webpage += F("<meta name='viewport' content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
  webpage += F("<style>");//From here style:
  webpage += F("body{max-width:65%;margin:0 auto;font-family:arial;font-size:100%;}");
  webpage += F("ul{list-style-type:none;padding:0;border-radius:0em;overflow:hidden;background-color:#d90707;font-size:1em;}");
  webpage += F("li{float:left;border-radius:0em;border-right:0em solid #bbb;}");
  webpage += F("li a{color:white; display: block;border-radius:0.375em;padding:0.44em 0.44em;text-decoration:none;font-size:100%}");
  webpage += F("li a:hover{background-color:#e86b6b;border-radius:0em;font-size:100%}");
  webpage += F("h1{color:white;border-radius:0em;font-size:1.5em;padding:0.2em 0.2em;background:#d90707;}");
  webpage += F("h2{color:blue;font-size:0.8em;}");
  webpage += F("h3{font-size:0.8em;}");
  webpage += F("table{font-family:arial,sans-serif;font-size:0.9em;border-collapse:collapse;width:85%;}"); 
  webpage += F("th,td {border:0.06em solid #dddddd;text-align:left;padding:0.3em;border-bottom:0.06em solid #dddddd;}"); 
  webpage += F("tr:nth-child(odd) {background-color:#eeeeee;}");
  webpage += F(".rcorners_n {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:20%;color:white;font-size:75%;}");
  webpage += F(".rcorners_m {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:50%;color:white;font-size:75%;}");
  webpage += F(".rcorners_w {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:70%;color:white;font-size:75%;}");
  webpage += F(".column{float:left;width:50%;height:45%;}");
  webpage += F(".row:after{content:'';display:table;clear:both;}");
  webpage += F("*{box-sizing:border-box;}");
  webpage += F("a{font-size:75%;}");
  webpage += F("p{font-size:75%;}");
  webpage += F("</style></head><body><h1>File Manager</h1>");
  webpage += F("<ul>");
  webpage += F("<li><a href='/'>Files</a></li>"); //Menu bar with commands
  webpage += F("<li><a href='/upload'>Configuration</a></li>"); 
  webpage += F("</ul>");
  webpage += F("<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"<meta charset=\"UTF-8\">"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"  <title>Progress Bar Popup</title>"
"<style>"
"body {"
"  font-family: Arial, sans-serif;"
"}"
"#openPopupBtn {"
"  padding: 10px 20px;"
"  font-size: 16px;"
"}"
".popup {"
"  display: none;"
"  position: fixed;"
"  z-index: 1;"
"  left: 0;"
"  top: 0;"
"  width: 100%;"
"  height: 100%;"
"  overflow: auto;"
"  background-color: rgba(0, 0, 0, 0.4);"
"}"
".popup-content {"
"  background-color: #fff;"
"  margin: 15% auto;"
"  padding: 20px;"
"  border: 1px solid #888;"
"  width: 80%;"
"  max-width: 400px;"
"  text-align: center;"
"}"
".close-btn {"
"  color: #aaa;"
"  float: right;"
"  font-size: 28px;"
"  font-weight: bold;"
"}"
".close-btn:hover,"
".close-btn:focus {"
"  color: black;"
"  text-decoration: none;"
"  cursor: pointer;"
"}"
".progress-container {"
"  width: 100%;"
"  background-color: #f3f3f3;"
"  border: 1px solid #ccc;"
"  border-radius: 5px;"
"  margin: 20px 0;"
"  overflow: hidden;"
"}"
".progress-bar {"
"  height: 30px;"
"  width: 0;"
"  background-color: #4caf50;"
"  line-height: 30px;"
"  color: white;"
"  text-align: center;"
"  transition: width 0.1s;"
"}"
"#progressText {"
"  font-size: 18px;"
"}"
"</style>"
"</head>"
"<body>"
"  <button id=\"download_btn\">Open Popup</button>"
"  <div id=\"popup\" class=\"popup\">"
"    <div class=\"popup-content\">"
"      <h2>Download in progress</h2>"
"      <div class=\"progress-container\">"
"        <div id=\"progressBar\" class=\"progress-bar\"></div>"
"      </div>"
"      <p id=\"progressText\">0%</p>"
"    </div>"
"  </div>"
"  <script>"
" document.getElementById('download_btn').addEventListener('click', function() {"
"  showLoading()"
"});"
"document.getElementById('closePopupBtn').addEventListener('click', function() {"
"  document.getElementById('popup').style.display = 'none';"
"  resetProgressBar();"
"});"
"function showLoading(time) {"
"  document.getElementById('popup').style.display = 'block';"
"  startProgressBar(time);"
"}"
"function startProgressBar(time) {"
"  let progressBar = document.getElementById('progressBar');"
"  let progressText = document.getElementById('progressText');"
"  let width = 0;"
"  let duration = time;"
"  let interval = duration * 10;"
"  let progressInterval = setInterval(function() {"
"    width += 1;"
"    progressBar.style.width = width + '%';"
"    progressText.innerText = width + '%';"
"    if (width >= 100) {"
"      clearInterval(progressInterval);"
"       document.getElementById('popup').style.display = 'none';"
"       resetProgressBar();"
"    }"
"  }, interval);"
"}"
"function resetProgressBar() {"
"  let progressBar = document.getElementById('progressBar');"
"  let progressText = document.getElementById('progressText');"
"  progressBar.style.width = '0';"
"  progressText.innerText = '0%';"
"}"
"</script>"
"</body>"
"</html>");
}
//Saves repeating many lines of code for HTML page footers
//void append_page_footer()
//{ 
//  webpage +=("</body></html>");
//}
