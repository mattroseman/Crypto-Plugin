(function() {
  if (window.__geo_hunt) return false;
  window.__geo_hunt = true;
  function dismissHuntBox() {
    var huntBox = document.querySelector('#geo-hunt-box');
    if (!huntBox || huntBox.classList.contains('exiting')) return false;
    huntBox.classList.add('exiting');
    setTimeout(function() {
      if (huntBox && huntBox.parentNode) {
        huntBox.parentNode.removeChild(huntBox);
      }
    }, 400);
  }
  function handleMouseDown(e) {
    var huntBox = document.querySelector('#geo-hunt-box');
    if (e.target.closest('#geo-hunt-box')) return false;
    dismissHuntBox();
  }
  function handleMouseUp(e) {
    var selected = String(document.getSelection()),
    streetRegex = /((?:(?:\d+(?:\x20+\w+\.?)+)))(?:(?:\,\s+)|\n)?((?:[#0-9A-Za-z\-]+\x20*)+)?(?:\,\s+|\n)((?:[A-Za-z]+\x20*)+)\,\s+(A[LKSZRAP]|C[AOT]|D[EC]|F[LM]|G[AU]|HI|I[ADLN]|K[SY]|LA|M[ADEHINOPST]|N[CDEHJMVY]|O[HKR]|P[ARW]|RI|S[CD]|T[NX]|UT|V[AIT]|W[AIVY]|[A-Z][a-z]+)(?:\s+(\d{5}(?:-\d{4})?))?/,
    extracted = streetRegex.exec(selected);
    if (extracted) {
      if (e.target.closest('#geo-hunt-box')) return false;
      var huntBox = document.querySelector('#geo-hunt-box');
      if (!huntBox) {
        huntBox = document.createElement("div");
        huntBox.id = "geo-hunt-box";
        var huntBoxTitle = document.createElement("div");
        huntBoxTitle.appendChild(document.createTextNode("GeoHunt"));
        huntBoxTitle.classList.add('header');
        huntBox.appendChild(huntBoxTitle);
        huntBox.contentDiv = document.createElement("div");
        huntBox.contentDiv.classList.add('content');
        huntBox.appendChild(huntBox.contentDiv);
        var favBtn = document.createElement("button");
        favBtn.className = 'mdl-button mdl-js-button mdl-button--icon';
        var favIcon = document.createElement("i");
        favIcon.className = 'material-icons';
        favIcon.textContent = 'favorite_border';
        favIcon.addEventListener("click", function() {
          favIcon.textContent = favIcon.textContent == 'favorite_border' ?
            'favorite' : 'favorite_border';
        });
        favBtn.appendChild(favIcon);
        huntBox.appendChild(favBtn);
        var mapImg = document.createElement("img");
        mapImg.src = chrome.extension.getURL(extracted[5] + '.png');
        huntBox.appendChild(mapImg);
        document.body.appendChild(huntBox);
      } else {
        if (huntBox.classList.contains('exiting')) return false;
      }
      huntBox.contentDiv.innerHTML = extracted[1] + ', ' +
        extracted[3] + ', ' + extracted[4] + ' ' + extracted[5];
      huntBox.style.left = e.x + 'px';
      huntBox.style.top = (e.y - 200) + 'px';
    }
  }
  document.onmousedown = handleMouseDown;
  document.onmouseup = handleMouseUp;
})();
