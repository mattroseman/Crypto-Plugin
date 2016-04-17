
var email = localStorage.getItem('google_email');
if (email) {
  document.querySelector('.step-box[data-step="1"]').classList.add('completed');
  document.querySelector('.step-box[data-step="1"] span.tip').textContent = 'Signed in to ' + email;
}
var stepBtns = document.querySelectorAll('.step-box .mdl-button');
function stepBtnOnClick(e) {
  switch (parseInt(e.target.closest('.step-box').dataset.step)) {
    case 1:
      break;
    default:
      console.log(e.target.closest('.step-box'), e.target.closest('.step-box').dataset.step);
  }
}
for (var i = 0; i < stepBtns.length; i++) {
  stepBtns[i].addEventListener("click", stepBtnOnClick, false);
}
