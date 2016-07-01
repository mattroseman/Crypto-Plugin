//
//  ViewController.swift
//  EzKeze
//
//  Created by Aron Gates on 5/23/16.
//  Copyright © 2016 Gates. All rights reserved.
//

import UIKit
import Locksmith
import Alamofire
import SwiftyJSON
import ChameleonFramework

class ViewController: UIViewController
{
    @IBOutlet weak var unlockedImage: UIImageView!
    @IBOutlet weak var lockedImage: UIImageView!
    let keychain = Keychain()
    var activityIndicator:UIActivityIndicatorView!
    
    @IBOutlet weak var usernameField: UITextField!
    @IBOutlet weak var passwordField: UITextField!
    @IBOutlet weak var logButton: UIButton!
    
    @IBOutlet weak var usernameRegis: UITextField!
    @IBOutlet weak var passwordRegis: UITextField!
    @IBOutlet weak var regisButton: UIButton!
    
    var firstColor = UIColor.flatWhiteColor()
    var secondColor = UIColor.flatMintColor()
    var gradient = UIGradientStyle.Radial
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        UIApplication.sharedApplication().delegate?.window??.backgroundColor = chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)

        view.backgroundColor = chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)
        
        // Do any additional setup after loading the view, typically from a nib.
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject!)
    {
        if (segue.identifier == "FirstNote")
        {
            
        }
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func log()
    {
        let username = usernameField.text
        let password = passwordField.text
        
        if keychain.keyExists(username!, password: password!)
        {
            spinImage(lockedImage)
            let dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(0.7 * Double(NSEC_PER_SEC)))
            dispatch_after(dispatchTime, dispatch_get_main_queue(), {
                self.loginSucess()
            })
        }
    }
    
    @IBAction func register()
    {
        let newUser = usernameRegis.text
        let newPass = passwordRegis.text
        keychain.saveNewKey(newUser!, newPass: newPass!)
        spinImage(unlockedImage)
        
        let dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(0.7 * Double(NSEC_PER_SEC)))
        
        dispatch_after(dispatchTime, dispatch_get_main_queue(), {
            self.performSegueWithIdentifier("FirstNote", sender: nil)
        })
        
    }
    
    @IBAction func getKeys()
    {
        let parameters = [
            "email" : "ig11"
        ]
        Alamofire.request(.POST, "http://stoh.io:8080/api/server_sync", parameters: parameters, encoding: .JSON, headers: nil)
            .validate()
            .responseJSON { response in
            if let value: AnyObject = response.result.value
            {
                switch response.result {
                case .Success:
                    //use received key json here
                    //let json = JSON(value)
                    //self.testingText.text = json.arrayObject?.debugDescription
                    break
                case .Failure:
                    break
                }
            }
        }
    }
    
    func loginSucess()
    {
        performSegueWithIdentifier("loginPage", sender: nil)
    }
    
    func chamelonChangeColor(firstColor: UIColor, secondColor: UIColor, gradientType: UIGradientStyle) -> UIColor
    {
        let colors:[UIColor] = [
            firstColor,
            secondColor,
            ]
        return GradientColor(gradientType, frame: view.frame, colors: colors)
    }
    
    func spinImage(Image: UIImageView)
    {
        var dispatchTime: dispatch_time_t = dispatch_time(DISPATCH_TIME_NOW, Int64(0.1 * Double(NSEC_PER_SEC)))
        
        dispatch_after(dispatchTime, dispatch_get_main_queue(), {
            Image.startRotating()
        })
        
        dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(0.55 * Double(NSEC_PER_SEC)))
        
        dispatch_after(dispatchTime, dispatch_get_main_queue(), {
            Image.stopRotating()
        })
    }

    func addActivityIndicator()
    {
        activityIndicator = UIActivityIndicatorView(frame: view.bounds)
        activityIndicator.activityIndicatorViewStyle = .WhiteLarge
        activityIndicator.backgroundColor = UIColor(white: 0, alpha: 0.25)
        activityIndicator.startAnimating()
        view.addSubview(activityIndicator)
    }
    
    func removeActivityIndicator()
    {
        activityIndicator.removeFromSuperview()
        activityIndicator = nil
    }
    
}

extension UIView {
    func startRotating(duration: Double = 0.5) {
        let kAnimationKey = "rotation"
        
        if self.layer.animationForKey(kAnimationKey) == nil {
            let animate = CABasicAnimation(keyPath: "transform.rotation")
            animate.duration = duration
            animate.repeatCount = Float.infinity
            animate.fromValue = 0.0
            animate.toValue = Float(M_PI * 2.0)
            self.layer.addAnimation(animate, forKey: kAnimationKey)
        }
    }
    func stopRotating() {
        let kAnimationKey = "rotation"
        
        if self.layer.animationForKey(kAnimationKey) != nil {
            self.layer.removeAnimationForKey(kAnimationKey)
        }
    }
}
