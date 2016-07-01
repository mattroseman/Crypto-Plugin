//
//  NotesController.swift
//  EzKeze
//
//  Created by Aron Gates on 6/30/16.
//  Copyright © 2016 Gates. All rights reserved.
//

import ChameleonFramework
import UIKit
import AVFoundation

class NotesController: UIViewController, AVAudioPlayerDelegate, AVAudioRecorderDelegate
{
    var firstColor = UIColor.flatSkyBlueColor()
    var secondColor = UIColor.flatPowderBlueColor()
    var gradient = UIGradientStyle.LeftToRight
    
    @IBOutlet weak var stopButton: UIButton!
    @IBOutlet weak var playButton: UIButton!
    @IBOutlet weak var recordButton: UIButton!
    var audioPlayer: AVAudioPlayer?
    var audioRecorder: AVAudioRecorder?
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        UIApplication.sharedApplication().delegate?.window??.backgroundColor = chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)
        view.backgroundColor = chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)
        
        let dirPaths =
            NSSearchPathForDirectoriesInDomains(.DocumentDirectory,
                                                .UserDomainMask, true)
        let docsDir = dirPaths[0] 
        let soundFilePath =
            docsDir + "sound.caf"
        let soundFileURL = NSURL(fileURLWithPath: soundFilePath)
        let recordSettings =
            [AVEncoderAudioQualityKey: AVAudioQuality.Min.rawValue,
             AVEncoderBitRateKey: 16,
             AVNumberOfChannelsKey: 2,
             AVSampleRateKey: 44100.0]
        
        var error: NSError?
        
        let audioSession = AVAudioSession.sharedInstance()
        try! audioSession.setCategory(AVAudioSessionCategoryPlayAndRecord)
        
        if let err = error {
            print("audioSession error: \(err.localizedDescription)")
        }
        
        audioRecorder = try! AVAudioRecorder(URL: soundFileURL,
                                        settings: recordSettings as [NSObject : AnyObject] as! [String : AnyObject])
        
        if let err = error {
            print("audioSession error: \(err.localizedDescription)")
        } else {
            audioRecorder?.prepareToRecord()
        }
    }
    
    func chamelonChangeColor(firstColor: UIColor, secondColor: UIColor, gradientType: UIGradientStyle) -> UIColor
    {
        let colors:[UIColor] = [
            firstColor,
            secondColor,
            ]
        return GradientColor(gradientType, frame: view.frame, colors: colors)
    }
    
    @IBAction func recordAudio(sender: AnyObject) {
        if audioRecorder?.recording == false {
            playButton.enabled = false
            stopButton.enabled = true
            audioRecorder?.record()
        }
    }
    
    @IBAction func stopAudio(sender: AnyObject) {
        stopButton.enabled = false
        playButton.enabled = true
        recordButton.enabled = true
        
        if audioRecorder?.recording == true {
            audioRecorder?.stop()
        } else {
            audioPlayer?.stop()
        }
    }
    
    @IBAction func playAudio(sender: AnyObject) {
        if audioRecorder?.recording == false {
            stopButton.enabled = true
            recordButton.enabled = false
            
            audioPlayer = try! AVAudioPlayer(contentsOfURL: (audioRecorder?.url)!)
            
            audioPlayer?.delegate = self
            
            audioPlayer?.play()
            
        }
    }
    
    func audioPlayerDecodeErrorDidOccur(player: AVAudioPlayer, error: NSError?) {
        print("Audio Play Decode Error")
    }
    
    func audioRecorderDidFinishRecording(recorder: AVAudioRecorder, successfully flag: Bool) {
    }
    
    func audioRecorderEncodeErrorDidOccur(recorder: AVAudioRecorder, error: NSError?) {
        print("Audio Record Encode Error")
    }

}
