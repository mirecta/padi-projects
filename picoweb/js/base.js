function httpGet(url, callback) {
  callback = typeof callback  === 'undefined' ? null : callback;
  var xmlHttp = new XMLHttpRequest();
  if (callback != null) {
    xmlHttp.onreadystatechange = function () {
      if (xmlHttp.readyState == 4) {
        callback(xmlHttp.status, xmlHttp.responseText);
      }
    };
  }
  xmlHttp.open("GET", url, true);
  xmlHttp.send(null);
}
function gEl(id) {
  return document.getElementById(id);
}
function gElCls(cls) {
  return document.getElementsByClassName(cls);
}
function rmCls(el, cls) {
  if (HTMLCollection.prototype.isPrototypeOf(el)) {
    for (i = 0; i < el.length; i++) {
      el[i].classList.remove(cls);
    }
  } else {
    el.classList.remove(cls);
  }
}
function addCls(el, cls) {
  if (HTMLCollection.prototype.isPrototypeOf(el)) {
    for (i = 0; i < el.length; i++) {
      el[i].classList.add(cls);
    }
  } else {
    el.classList.add(cls);
  }
}
function setCls(el, cls) {
  if (HTMLCollection.prototype.isPrototypeOf(el)) {
    for (i = 0; i < el.length; i++) {
      el[i].className = cls;
    }
  } else {
    el.className = cls;
  }
}
function createMeter(id, color) {
  color = typeof color  === 'undefined' ? 'green' : color;
  el = gEl(id);
  setCls(el, "meter " + color);
  el.innerHTML = "<span style='width: 0%'></span>";
}
function meterVal(id, val) {
  gEl(id).childNodes[0].style.width = String(val) + '%';
}
function ledOff(id) {
  setCls(gEl(id), "led");
}
function ledOn(id, color) {

  setCls(gEl(id), "led led-" + color);
}
function createCirc(id) {
  el = gEl(id);
  el.innerHTML = '<div class="val">0%</div><div class="wrapper"><div class="circle" ></div><div class="circle" ></div></div><div class="frame"></div>';
  setCls(el, "circ");
}
function circVal(id, val) {
  el = gEl(id);
  var angl = 360.0 / 100 * val;
  if (val >= 50) {
    setCls(el.childNodes[1], "wrapper afterhalf");
  } else {
    setCls(el.childNodes[1], "wrapper");
  }
  if (val <= 50) {
    el.childNodes[1].childNodes[1].style.transform = 'rotate(' + String(angl) + 'deg)';
  } else {
    el.childNodes[1].childNodes[1].style.transform = 'rotate(180deg)';
  }
  el.childNodes[1].childNodes[0].style.transform = 'rotate(' + String(angl) + 'deg)';
  el.childNodes[0].innerHTML = String(val) + '%';
}
function createArr(id) {
  el = gEl(id);
  setCls(el, "frame arrowCont");
  el.innerHTML = '<div class="arrow"></div>';
}
function arrVal(id, deg) {
  gEl(id).childNodes[0].style.transform = 'rotate(' + String(deg) + 'deg)';
}
function modalOpen(title,msg){
  gEl('modalTitle').innerHTML = title;
  gEl('modalContent').innerHTML = msg;
  gEl('modal').style.display='block';
}
function modalClose(){
  gEl('modal').style.display = 'none';
}