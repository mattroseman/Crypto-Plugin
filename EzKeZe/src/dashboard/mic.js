window.AudioContext = window.AudioContext || window.webkitAudioContext;

var keyData;
var micButtonHTML;
var loadingDialogHTML = '<div class="mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active"></div>';

function saveRecording(blob) {
var time = new Date(),
    url = URL.createObjectURL(blob),
    html = "<audio controls style='width: 256px;' src='" + url + "'></audio> ";
  var div = document.createElement('div');
  div.innerHTML = html;
  document.getElementById("audioPreview").innerHTML = "";
  document.getElementById("audioPreview").appendChild(div);
//$recordingList.prepend($(html));
  console.log('recording saved!');
}


var audioContext = new AudioContext();
var audioInput = null,
    realAudioInput = null,
    inputPoint = null,
    audioRecorder = null;
var rafID = null;
var analyserContext = null;
var canvasWidth, canvasHeight;
var recIndex = 0;

/* TODO:

- offer mono option
- "Monitor input" switch
*/

function saveAudio() {
    audioRecorder.exportWAV( doneEncoding );
    // could get mono instead by saying
    // audioRecorder.exportMonoWAV( doneEncoding );
}

function gotBuffers( buffers ) {
    var canvas = document.getElementById( "wavedisplay" );

    drawBuffer( canvas.width, canvas.height, canvas.getContext('2d'), buffers[0] );

    // the ONLY time gotBuffers is called is right after a new recording is completed -
    // so here's where we should set up the download.
    audioRecorder.exportWAV( doneEncoding );
}

var encodedBlob;
function doneEncoding( blob ) {
    saveRecording(blob);
    encodedBlob = blob;
    console.log("Encoding finished")
}

var serverSync = function(){
  var micSpinner = document.getElementById("micSpinner");
  if(encodedBlob){
    console.log("sending data to the server");
    micSpinner.classList.add("is-active");

    var timestamp = new Date().getTime();
    blobToBase64(encodedBlob, function(base64){ // encode
      var update = {'blob': base64};
      var wavData = { email : 'ig11', blob : base64, blob_name: 'ig11' + timestamp};
      $.ajax({
        type: 'POST',
        url: 'http://stoh.io:8080/api/server_sync',
        dataType: 'json',
        data: wavData
      }).done(function(data) {
          micSpinner.classList.remove("is-active");
          document.getElementById("fabIcon").innerHTML = "check";
          recordButton.style.color = "green";
          keyData = data;
          createKeyList(data);
      });
    })
  }
}

var toggleRecording = function( e ) {
    if (e) {
        // stop recording
        audioRecorder.stop();
        audioRecorder.getBuffers( gotBuffers );
        console.log("stopped recording");
    } else {
        // start recording
        if (!audioRecorder)
            return;
        audioRecorder.clear();
        audioRecorder.record();
        console.log("Started recording");
    }
}

function convertToMono( input ) {
    var splitter = audioContext.createChannelSplitter(2);
    var merger = audioContext.createChannelMerger(2);

    input.connect( splitter );
    splitter.connect( merger, 0, 0 );
    splitter.connect( merger, 0, 1 );
    return merger;
}

function gotStream(stream) {
    inputPoint = audioContext.createGain();

    // Create an AudioNode from the stream.
    realAudioInput = audioContext.createMediaStreamSource(stream);
    audioInput = realAudioInput;
    audioInput.connect(inputPoint);

//    audioInput = convertToMono( input );

    analyserNode = audioContext.createAnalyser();
    analyserNode.fftSize = 2048;
    inputPoint.connect( analyserNode );

    audioRecorder = new Recorder( inputPoint );

    zeroGain = audioContext.createGain();
    zeroGain.gain.value = 0.0;
    inputPoint.connect( zeroGain );
    zeroGain.connect( audioContext.destination );
}


function toggleMono() {
    if (audioInput != realAudioInput) {
        audioInput.disconnect();
        realAudioInput.disconnect();
        audioInput = realAudioInput;
    } else {
        realAudioInput.disconnect();
        audioInput = convertToMono( realAudioInput );
    }

    audioInput.connect(inputPoint);
}


function initAudio() {
        if (!navigator.getUserMedia)
            navigator.getUserMedia = navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
        if (!navigator.cancelAnimationFrame)
            navigator.cancelAnimationFrame = navigator.webkitCancelAnimationFrame || navigator.mozCancelAnimationFrame;
        if (!navigator.requestAnimationFrame)
            navigator.requestAnimationFrame = navigator.webkitRequestAnimationFrame || navigator.mozRequestAnimationFrame;

    navigator.getUserMedia(
        {
            "audio": {
                "mandatory": {
                    "googEchoCancellation": "false",
                    "googAutoGainControl": "false",
                    "googNoiseSuppression": "false",
                    "googHighpassFilter": "false"
                },
                "optional": []
            },
        }, gotStream, function(e) {
            alert('Error getting audio');
            console.log(e);
        });
}

window.addEventListener('load', initAudio );

// converts blob to base64
var blobToBase64 = function(blob, cb) {
  var reader = new FileReader();
  reader.onload = function() {
    var dataUrl = reader.result;
    var base64 = dataUrl.split(',')[1];
    cb(base64);
  };
  reader.readAsDataURL(blob);
};

function drawBuffer( width, height, context, data ) {
    var step = Math.ceil( data.length / width );
    var amp = height / 2;
    context.fillStyle = "silver";
    context.clearRect(0,0,width,height);
    for(var i=0; i < width; i++){
        var min = 1.0;
        var max = -1.0;
        for (j=0; j<step; j++) {
            var datum = data[(i*step)+j];
            if (datum < min)
                min = datum;
            if (datum > max)
                max = datum;
        }
        context.fillRect(i,(1+min)*amp,1,Math.max(1,(max-min)*amp));
    }
}

var createKeyList = function(list){
  var lst = document.getElementById('keyList');
  // clean the list before we append to it
  console.log(list);
  for(var i = 0; i < list.length; i++){
    var entry = list[i];
    console.log(entry);
    var entryID = entry.id;
    var entryKey = entry.key;
    var rawHTML = '<span class="mdl-list__item-primary-content"> <i class="material-icons  mdl-list__item-avatar">lock</i>' +
        'Unlock Wtih Custom Key' + '</span> <span class="mdl-list__item-secondary-action"> <label class="mdl-checkbox mdl-js-checkbox mdl-js-ripple-effect" for="list-checkbox-1"> <input type="checkbox" id="list-checkbox-1" class="mdl-checkbox__input"/> </label> </span>';
  //x = document.getElementById("keyList"), 
    //c = x.childNodes;
	//for (var i = 0; i < 4; i++) {
		   //x.removeChild(c[i--]);
	//}
    var listingItem = document.createElement('li');
    listingItem.className = "mdl-list__item";
    listingItem.innerHTML = rawHTML;
    lst.appendChild(listingItem);
  }
}
