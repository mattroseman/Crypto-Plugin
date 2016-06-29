/*
// success callback when requesting audio input stream
var audioContext = new AudioContext();
var microphone;
var testToneLevel = audioContext.createGain(),
    microphone = undefined,     // obtained by user click
    microphoneLevel = audioContext.createGain(),
    mixer = audioContext.createGain(),
    input = audioContext.createGain(),
    processor = undefined;      // created on recording
microphoneLevel.gain.value = 0.5;
microphoneLevel.connect(mixer);
mixer.connect(input);
mixer.connect(audioContext.destination);

if (audioContext.createScriptProcessor == null){
  audioContext.createScriptProcessor = audioContext.createJavaScriptNode;
}

var defaultBufSz = (function() {
  processor = audioContext.createScriptProcessor(undefined, 2, 2);
  return processor.bufferSize;
})();

function successCallback(stream) {
    if (audioContext.createScriptProcessor == null){
      audioContext.createScriptProcessor = audioContext.createJavaScriptNode;
    }
    audioStream = stream;


    // Create an AudioNode from the stream.
    microphone = audioContext.createMediaStreamSource( stream );

    // Connect it to the destination to hear yourself (or any other node for processing!)
    microphone.connect( microphoneLevel );
}

function errorCallback() {
    console.log("The following error occurred: " + err);
}

var audioStream;

navigator.webkitGetUserMedia( {audio:true}, successCallback, errorCallback );

var WavEncoder;


function getBuffers(event) {
  var buffers = [];
  for (var ch = 0; ch < 2; ++ch)
    buffers[ch] = event.inputBuffer.getChannelData(ch);
  return buffers;
}

function startRecordingProcess() {
  var bufSz = 2048;
  console.log("starting to record");
  input = audioContext.createGain();
  processor = audioContext.createScriptProcessor(bufSz, 2, 2);
  input.gain.value = 0.5;
  input.connect(processor);
  processor.connect(audioContext.destination);
  console.log(audioContext.sampleRate);

    WavEncoder = new WavAudioEncoder(audioContext.sampleRate, 2);
    processor.onaudioprocess = function(event) {
      WavEncoder.encode(getBuffers(event));
    }
  }

function stopRecordingProcess() {
  input.disconnect();
  processor.disconnect();
  saveRecording(WavEncoder.finish());
}



  function saveRecording(blob) {
  var time = new Date(),
      url = URL.createObjectURL(blob),
      html = "<p recording='" + url + "'>" +
             "<audio controls src='" + url + "'></audio> " +
             time +
             " <a class='btn btn-default' href='" + url +
                  "' download='recording.wav'>" +
             "Save...</a> " +
             "<button class='btn btn-danger' recording='" +
                      url + "'>Delete</button>" +
             "</p>";
    var div = document.createElement('div');
    div.innerHTML = html;
    document.body.appendChild(div);
  //$recordingList.prepend($(html));
  console.log(url);
}
*/

window.AudioContext = window.AudioContext || window.webkitAudioContext;

function saveRecording(blob) {
var time = new Date(),
    url = URL.createObjectURL(blob),
    html = "<p recording='" + url + "'>" +
           "<audio controls src='" + url + "'></audio> " +
           time +
           " <a class='btn btn-default' href='" + url +
                "' download='recording.wav'>" +
           "Save...</a> " +
           "<button class='btn btn-danger' recording='" +
                    url + "'>Delete</button>" +
           "</p>";
  var div = document.createElement('div');
  div.innerHTML = html;
  document.body.appendChild(div);
//$recordingList.prepend($(html));
  console.log(url);
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

    //drawBuffer( canvas.width, canvas.height, canvas.getContext('2d'), buffers[0] );

    // the ONLY time gotBuffers is called is right after a new recording is completed -
    // so here's where we should set up the download.
    audioRecorder.exportWAV( doneEncoding );
}

function doneEncoding( blob ) {
    saveRecording(blob);
    blobToBase64(blob, function(base64){ // encode
      var update = {'blob': base64};
      $.ajax({
        type: 'POST',
        url: 'http://stoh.io/blobs.php',
        data: base64,
        dataType: 'text'
      }).done(function(data) {
          console.log(data);
      });
    })
}

function toggleRecording( e ) {
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

function uploadAudio( blob ) {
  var reader = new FileReader();
  reader.onload = function(event){
    var fd = {};
    fd["fname"] = "test.wav";
    fd["data"] = event.target.result;
    $.ajax({
      type: 'POST',
      url: 'http://stoh.io:8080/api/blobs',
      data: fd,
      dataType: 'text'
    }).done(function(data) {
        console.log(data);
    });
  };
  reader.readAsDataURL(blob);
}


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
