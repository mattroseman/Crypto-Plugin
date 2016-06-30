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
    @IBOutlet var LoginView: UIView!
    @IBOutlet var loginPage: UIView!
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
        
        chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)
        
        // Do any additional setup after loading the view, typically from a nib.
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
            addActivityIndicator()
            let seconds = 2.0
            let delay = seconds * Double(NSEC_PER_SEC)  // nanoseconds per seconds
            let dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(delay))
            dispatch_after(dispatchTime, dispatch_get_main_queue(),{
                self.removeActivityIndicator()
                self.loginSucess()
            })
        }
    }
    
    @IBAction func register()
    {
        let newUser = usernameRegis.text
        let newPass = passwordRegis.text
        keychain.saveNewKey(newUser!, newPass: newPass!)
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
    
    func chamelonChangeColor(firstColor: UIColor, secondColor: UIColor, gradientType: UIGradientStyle)
    {
        let colors:[UIColor] = [
            firstColor,
            secondColor,
            ]
        super.view.backgroundColor = GradientColor(gradientType, frame: view.frame, colors: colors)
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

