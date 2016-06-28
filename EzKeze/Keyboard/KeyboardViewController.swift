//
//  KeyboardViewController.swift
//  keyboard
//  YOU CAN FIT 25 M's ON THE KEYBOARD SCREEN
//  Created by Aron Gates on 4/30/16.
//  Copyright © 2016 Gates. All rights reserved.
//

import UIKit
import Alamofire

class KeyboardViewController: UIInputViewController
{
    var keyboardView: UIView!
    var keyboardNib: UINib!
    
    var dropdownLogic:Bool = false
    var lowercaseLogic:Bool = false
    var uppercaseLogic:Bool = false
    var doublecaseLogic:Bool = false
    var numbersOneLogic:Bool = false
    var numbersTwoLogic:Bool = false
    var keyboardLoaded:Bool = false
    var trackError:Bool = false
    var decryptedLogic:Bool = false
    var lastString:String = ""
    var based64:String = ""
    var decrypting:Bool = false
    
    var timer = NSTimer()
    var timerTwo = NSTimer()
    var deleteFast = NSTimer()
    
    let key: [UInt8] = [115, 98, 103, 111, 108, 121, 112, 105, 101, 115, 114, 100, 97, 105, 105, 97]
    let iv: [UInt8] = [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]
    let based64Key = "secret0key000000"
    let based64IV = "0123456789012345"
    
    var displayString:String = ""
    var memoryDisplayString:NSMutableString = ""
    
    let identifier = "{\"identifier\":\"MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM\",\"content\":"
    let identifierTwo = "}"
    
    var counter = 1.0
    var counterTwo = 1.0
    var globalMemoryIndex = 0
    let popupX:CGFloat = 5
    let popupY:CGFloat = 10
    
    var activityIndicator:UIActivityIndicatorView!
    
    // lowercase and uppercase buttons
    @IBOutlet var q: UIButton!
    @IBOutlet var w: UIButton!
    @IBOutlet var e: UIButton!
    @IBOutlet var r: UIButton!
    @IBOutlet var t: UIButton!
    @IBOutlet var y: UIButton!
    @IBOutlet var u: UIButton!
    @IBOutlet var i: UIButton!
    @IBOutlet var o: UIButton!
    @IBOutlet var p: UIButton!
    @IBOutlet var a: UIButton!
    @IBOutlet var s: UIButton!
    @IBOutlet var d: UIButton!
    @IBOutlet var f: UIButton!
    @IBOutlet var g: UIButton!
    @IBOutlet var h: UIButton!
    @IBOutlet var j: UIButton!
    @IBOutlet var k: UIButton!
    @IBOutlet var l: UIButton!
    @IBOutlet var z: UIButton!
    @IBOutlet var x: UIButton!
    @IBOutlet var c: UIButton!
    @IBOutlet var v: UIButton!
    @IBOutlet var b: UIButton!
    @IBOutlet var n: UIButton!
    @IBOutlet var m: UIButton!
    
    // numbers and symbols buttons
    @IBOutlet var one: UIButton!
    @IBOutlet var two: UIButton!
    @IBOutlet var three: UIButton!
    @IBOutlet var four: UIButton!
    @IBOutlet var five: UIButton!
    @IBOutlet var six: UIButton!
    @IBOutlet var seven: UIButton!
    @IBOutlet var eight: UIButton!
    @IBOutlet var nine: UIButton!
    @IBOutlet var zero: UIButton!
    @IBOutlet var dash: UIButton!
    @IBOutlet var forwardSlash: UIButton!
    @IBOutlet var colon: UIButton!
    @IBOutlet var semicolon: UIButton!
    @IBOutlet var leftParen: UIButton!
    @IBOutlet var rightParen: UIButton!
    @IBOutlet var dollar: UIButton!
    @IBOutlet var ampersand: UIButton!
    @IBOutlet var at: UIButton!
    @IBOutlet var quote: UIButton!
    @IBOutlet var period: UIButton!
    @IBOutlet var comma: UIButton!
    @IBOutlet var question: UIButton!
    @IBOutlet var exclamation: UIButton!
    @IBOutlet var singleQuote: UIButton!
    
    // general buttons
    @IBOutlet var space: UIButton!
    @IBOutlet var initialize123Button: UIButton!
    @IBOutlet var initializeUpperButton: UIButton!
    @IBOutlet var initializeDeleteButton: UIButton!
    @IBOutlet var initializeEncryptButton: UIButton!
    @IBOutlet var initializeNextKeyboard: UIButton!
    @IBOutlet var dropUpButton: UIButton!
    @IBOutlet var normalDownButton: UIButton!
    @IBOutlet var viewLabel: UILabel!
    @IBOutlet var dropdownTextProxy: UITextView!
    @IBOutlet var slideImage: UIImageView!
    
    var heightConstraint:NSLayoutConstraint?
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        loadKeyboard()
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        let allAlphabetButtons:[UIButton] = [q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m, initializeUpperButton, initializeDeleteButton, space, initialize123Button,initializeEncryptButton, initializeNextKeyboard, normalDownButton]
        
        // setting the height priority
        self.heightConstraint = NSLayoutConstraint(item:self.inputView!, attribute:.Height, relatedBy:.Equal, toItem:nil, attribute:.NotAnAttribute, multiplier:0, constant:0)
        self.heightConstraint!.priority = 999
        self.heightConstraint!.active = true
        
        // take all number buttons out of the view
        let numbersButtons:[UIButton] = [one, two, three, four, five, six, seven, eight, nine, zero, dash, forwardSlash, colon, semicolon, leftParen, rightParen, dollar, ampersand, at, quote, period, comma, question, exclamation, singleQuote]
        var index = numbersButtons.startIndex
        repeat
        {
            numbersButtons[index].center.x -= view.bounds.width
            index += 1
        }
            while index < numbersButtons.endIndex
        
        // animate the loading keyboard
        if keyboardLoaded == false
        {
            animateKeyboard(allAlphabetButtons)
            keyboardLoaded = true
        }
    }
    
    override func updateViewConstraints()
    {
        super.updateViewConstraints()
        
        guard self.heightConstraint != nil && self.view.frame.size.width != 0 && self.view.frame.size.height != 0 else { return }
        
        let portraitHeight: CGFloat = (216 + (e.frame.width / 2))
        let landscapeHeight: CGFloat = 200
        let screenSize = UIScreen.mainScreen().bounds.size
        
        let newHeight = screenSize.width > screenSize.height ? landscapeHeight : portraitHeight
        
        if (self.heightConstraint!.constant != newHeight)
        {
            self.heightConstraint!.constant = newHeight
        }
        view.backgroundColor = UIColor.blueColor().colorWithAlphaComponent(0.0)
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated
    }
    
    @IBAction func encryptFunc()
    {
        
    }
    
    func turnDescriptionIntoJson(descriptionInput: String) -> [UInt8]
    {
        var replaced = descriptionInput.stringByReplacingOccurrencesOfString(" ", withString: "")
        replaced = String(replaced.characters.map {
            $0 == "[" ? "," : $0
            })
        replaced = String(replaced.characters.map {
            $0 == "]" ? "," : $0
            })
        let index1 = replaced.endIndex.advancedBy(-1)
        replaced = replaced.substringToIndex(index1)
        replaced = String(replaced.characters.dropFirst())
        
        let myArray = replaced.componentsSeparatedByString(",")
        let jsonEncrypted: [UInt8] = myArray.map { UInt8($0)!}
        
        return jsonEncrypted
    }
    
    @IBAction func decryptFunc()
    {
        if !decrypting
        {
            decrypting = true
            addActivityIndicator()
            Alamofire.request(.GET, "https://httpbin.org/deny", parameters: [:]).responseString
                { response in
                    //let value = response.result.value
                    let seconds = 2.0
                    let delay = seconds * Double(NSEC_PER_SEC)  // nanoseconds per seconds
                    let dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(delay))
                    
                    dispatch_after(dispatchTime, dispatch_get_main_queue(),
                    {
                        if !self.dropdownLogic
                        {
                            self.toDropUp()
                        }
                        self.removeActivityIndicator()
                        self.dropdownTextProxy.text = "Lorem ipsum dolor sit er elit lamet, consectetaur cillium adipisicing pecu, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Nam liber te conscient to factor tum poen legum odioque civiuda."
                        self.decryptUp()
                    })
            }
        }
    }
    
    @IBAction func nextKeyboard()
    {
        advanceToNextInputMode()
    }
    
    @IBAction func decryptUp()
    {
        decrypting = false
    }
    
    func loadKeyboard()
    {
        lowercaseLogic = true
        let keyboardNib = UINib(nibName: "keyboardView", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        viewLabel.text = ""
    }
    
    @IBAction func toLowercase()
    {
        let alphabet:[UIButton] = [q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m]
        
        // if coming from either numbers, removing those buttons from view and adding the alphabet buttons back into view
        if numbersOneLogic || numbersTwoLogic
        {
            let numbersButtons:[UIButton] = [one, two, three, four, five, six, seven, eight, nine, zero, dash, forwardSlash, colon, semicolon, leftParen, rightParen, dollar, ampersand, at, quote, period, comma, question, exclamation, singleQuote]
            var index = numbersButtons.startIndex
            repeat
            {
                numbersButtons[index].center.x -= view.bounds.width
                index += 1
            }
                while index < numbersButtons.endIndex
            
            index = alphabet.startIndex
            repeat
            {
                alphabet[index].center.x += view.bounds.width
                index += 1
            }
                while index < alphabet.endIndex
        }
        
        // setting all alphabet buttons back to lowercase and resetting the upperButtonIndicator
        var index = alphabet.startIndex
        repeat
        {
            var temp = alphabet[index].titleLabel?.text
            temp = temp!.lowercaseString
            let addingPNG = temp! + "Low.png"
            let image = UIImage(named: addingPNG)
            alphabet[index].setImage(image, forState: UIControlState.Normal)
            alphabet[index].setTitle(temp, forState: UIControlState.Normal)
            index = index + 1
        }
            while index < alphabet.endIndex
        initializeUpperButton.setImage(UIImage(named: "lowercase.png"), forState: UIControlState.Normal)
        initializeUpperButton.removeTarget(nil, action: nil, forControlEvents: .AllEvents)
        initializeUpperButton.addTarget(self, action: #selector(KeyboardViewController.toUppercase), forControlEvents: UIControlEvents.TouchUpInside)
        initialize123Button.setImage(UIImage(named: "123.png"), forState: UIControlState.Normal)
        initialize123Button.removeTarget(nil, action: nil, forControlEvents: .AllEvents)
        initialize123Button.addTarget(self, action: #selector(KeyboardViewController.toNumbersOne), forControlEvents: UIControlEvents.TouchUpInside)
        
        lowercaseLogic = true
        uppercaseLogic = false
        doublecaseLogic = false
        numbersOneLogic = false
        numbersTwoLogic = false
    }
    
    @IBAction func toUppercase()
    {
        let alphabet:[UIButton] = [q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m]
        
        // turning each alphabet button into uppercase and resetting the upperButtonIndicator
        var index = alphabet.startIndex
        repeat
        {
            var temp = alphabet[index].titleLabel?.text
            temp = temp?.stringByReplacingOccurrencesOfString("Low", withString: "")
            temp = temp?.capitalizedString
            let addingPNG = temp! + ".png"
            let image = UIImage(named: addingPNG)
            alphabet[index].setImage(image, forState: UIControlState.Normal)
            alphabet[index].setTitle(temp, forState: UIControlState.Normal)
            index = index + 1
        } while index < alphabet.endIndex
        initializeUpperButton.setImage(UIImage(named: "uppercase.png"), forState: UIControlState.Normal)
        initializeUpperButton.removeTarget(nil, action: nil, forControlEvents: .AllEvents)
        initializeUpperButton.addTarget(self, action: #selector(KeyboardViewController.toLowercase), forControlEvents: UIControlEvents.TouchUpInside)
        
        lowercaseLogic = false
        uppercaseLogic = true
        doublecaseLogic = false
        numbersOneLogic = false
        numbersTwoLogic = false
    }
    
    @IBAction func toDoublecase()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = true
        numbersOneLogic = false
        numbersTwoLogic = false
    }
    
    @IBAction func toNumbersOne()
    {
        let numbersButtons:[UIButton] = [one, two, three, four, five, six, seven, eight, nine, zero, dash, forwardSlash, colon, semicolon, leftParen, rightParen, dollar, ampersand, at, quote, period, comma, question, exclamation, singleQuote]
        let numbersOneOutput:[String] = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "/", ":", ";", "(", ")", "$", "&", "@", "\""]
        let numbersOneImage:[UIImage] = [
            UIImage(named: "1.png")!,
            UIImage(named: "2.png")!,
            UIImage(named: "3.png")!,
            UIImage(named: "4.png")!,
            UIImage(named: "5.png")!,
            UIImage(named: "6.png")!,
            UIImage(named: "7.png")!,
            UIImage(named: "8.png")!,
            UIImage(named: "9.png")!,
            UIImage(named: "0.png")!,
            UIImage(named: "-.png")!,
            UIImage(named: "forwardSlash.png")!,
            UIImage(named: "colon.png")!,
            UIImage(named: ";.png")!,
            UIImage(named: "(.png")!,
            UIImage(named: ").png")!,
            UIImage(named: "$.png")!,
            UIImage(named: "&.png")!,
            UIImage(named: "@.png")!,
            UIImage(named: "\".png")!]
        
        // puts numbers buttons back into the view
        if lowercaseLogic || uppercaseLogic || doublecaseLogic && !numbersTwoLogic
        {
            var index = numbersButtons.startIndex
            repeat
            {
                numbersButtons[index].center.x += view.bounds.width
                index += 1
            }
                while index < numbersButtons.endIndex
            
            // puts alphabet buttons out of the view
            let alphabetButtons:[UIButton] = [q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m]
            index = alphabetButtons.startIndex
            repeat
            {
                alphabetButtons[index].center.x -= view.bounds.width
                index += 1
            }
                while index < alphabetButtons.endIndex
        }
        
        // changes buttons image and output from numbersTwo back to numbersOne, and account for if the user switches from numbersTwo to lower case - in that situation, the numbers buttons weren't reset to numbersOne. This resets it back to numbersOne if in that case
        if numbersTwoLogic || lowercaseLogic
        {
            var index = numbersButtons.startIndex
            repeat
            {
                numbersButtons[index].setImage(numbersOneImage[index], forState: .Normal)
                numbersButtons[index].setTitle(numbersOneOutput[index], forState: .Normal)
                index += 1
            }
                while index < (numbersButtons.endIndex - 5)
        }
        
        // changes general initialize buttons image and function to match new view
        initializeUpperButton.setImage(UIImage(named: "symbols.png"), forState: UIControlState.Normal)
        initializeUpperButton.removeTarget(nil, action: nil, forControlEvents: .AllEvents)
        initializeUpperButton.addTarget(self, action: #selector(KeyboardViewController.toNumbersTwo), forControlEvents: UIControlEvents.TouchUpInside)
        initialize123Button.setImage(UIImage(named: "ABC.png"), forState: UIControlState.Normal)
        initialize123Button.removeTarget(nil, action: nil, forControlEvents: .AllEvents)
        initialize123Button.addTarget(self, action: #selector(KeyboardViewController.toLowercase), forControlEvents: UIControlEvents.TouchUpInside)
        
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = false
        numbersOneLogic = true
        numbersTwoLogic = false
    }
    
    @IBAction func toNumbersTwo()
    {
        let numbersButtons:[UIButton] = [one, two, three, four, five, six, seven, eight, nine, zero, dash, forwardSlash, colon, semicolon, leftParen, rightParen, dollar, ampersand, at, quote, period, comma, question, exclamation, singleQuote]
        let numbersTwoOutput:[String] = ["[", "]", "{", "}", "#", "%", "^", "*", "+", "=", "_", "\\", "|", "~", "<", ">", "€", "£", "¥", "•"]
        let numbersTwoImage:[UIImage] = [
            UIImage(named: "[.png")!,
            UIImage(named: "].png")!,
            UIImage(named: "{.png")!,
            UIImage(named: "}.png")!,
            UIImage(named: "#.png")!,
            UIImage(named: "%.png")!,
            UIImage(named: "^.png")!,
            UIImage(named: "*.png")!,
            UIImage(named: "+.png")!,
            UIImage(named: "=.png")!,
            UIImage(named: "_.png")!,
            UIImage(named: "backslash.png")!,
            UIImage(named: "longline.png")!,
            UIImage(named: "tilda.png")!,
            UIImage(named: "<.png")!,
            UIImage(named: ">.png")!,
            UIImage(named: "euro.png")!,
            UIImage(named: "pound.png")!,
            UIImage(named: "yen.png")!,
            UIImage(named: "bullet.png")!]
        
        // changes buttons image and output from numbersOne back to numbersTwo
        if numbersOneLogic
        {
            var index = numbersButtons.startIndex
            repeat
            {
                numbersButtons[index].setImage(numbersTwoImage[index], forState: .Normal)
                numbersButtons[index].setTitle(numbersTwoOutput[index], forState: .Normal)
                index += 1
            }
                while index < (numbersButtons.endIndex - 5)
        }
        
        // changes general initialize buttons image and function to match new view
        initializeUpperButton.setImage(UIImage(named: "123.png"), forState: UIControlState.Normal)
        initializeUpperButton.removeTarget(nil, action: nil, forControlEvents: .AllEvents)
        initializeUpperButton.addTarget(self, action: #selector(KeyboardViewController.toNumbersOne), forControlEvents: UIControlEvents.TouchUpInside)
        
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = false
        numbersOneLogic = false
        numbersTwoLogic = true
        
    }
    
    @IBAction func toNormalDown()
    {
        UIView.animateWithDuration(0.2)
        {
            let size = CGSize(width: self.dropdownTextProxy.frame.width, height: 0)
            let rect = CGRect(
                origin: CGPoint(x: self.dropdownTextProxy.frame.origin.x, y: self.dropdownTextProxy.frame.origin.y),
                size: size
            )
            self.dropdownTextProxy.frame = rect
            self.dropdownTextProxy.center.y -= -(216 - self.normalDownButton.frame.height)
        }
        
        memoryDisplayString = NSMutableString(string: dropdownTextProxy.text!)
        let endIndex = memoryDisplayString.description.characters.endIndex
        if memoryDisplayString.description.characters.count >= 20
        {
            let beginIndex = memoryDisplayString.description.characters.endIndex.advancedBy(-20)
            displayString = memoryDisplayString.description[Range(beginIndex ..< endIndex)]
        }
        else
        {
            displayString = memoryDisplayString.description
        }
        if decryptedLogic
        {
            displayString = ""
            memoryDisplayString = ""
            globalMemoryIndex = 0
            decryptedLogic = false
        }
        globalMemoryIndex = memoryDisplayString.description.characters.count
        viewLabel.text = displayString
        dropdownTextProxy.text = ""
        
        let seconds = 0.19
        let delay = seconds * Double(NSEC_PER_SEC)  // nanoseconds per seconds
        let dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(delay))
        
        dispatch_after(dispatchTime, dispatch_get_main_queue(),
        {
            self.slideImage.image = UIImage(named: "slideUp.png")

            self.viewLabel.center.x += self.view.center.x
            self.normalDownButton.center.x -= self.view.bounds.width
            self.dropUpButton.center.x += self.view.bounds.width
            
            self.heightConstraint!.constant = (216 + (self.e.frame.width / 2))
            self.heightConstraint = NSLayoutConstraint(item:self.inputView!, attribute:.Height, relatedBy:.Equal, toItem:nil, attribute:.NotAnAttribute, multiplier:0, constant:0)
            self.heightConstraint!.active = true
            let height:CGFloat = -216
            self.editKeyboardKeysHeight(height)
        })
        
        dropdownLogic = false
    }
    
    @IBAction func toDropUp()
    {
        dropdownTextProxy.text = memoryDisplayString.description
        slideImage.image = UIImage(named: "slideDown.png")
        
        let newHeight:CGFloat = 448
        if (self.heightConstraint!.constant != newHeight)
        {
            self.heightConstraint!.constant = newHeight
            self.heightConstraint = NSLayoutConstraint(item:self.inputView!, attribute:.Height, relatedBy:.Equal, toItem:nil, attribute:.NotAnAttribute, multiplier:0, constant:0)
            self.heightConstraint!.priority = 1
            self.heightConstraint!.active = true
        }
        
        let height:CGFloat = 216
        editKeyboardKeysHeight(height)
        
        UIView.animateWithDuration(0.2)
        {
            let size = CGSize(width: self.dropdownTextProxy.frame.width, height: (self.space.center.y - (216 + (self.e.frame.width / 2) + (self.e.frame.width / 8))))
            let rect = CGRect(
                origin: CGPoint(x: self.dropdownTextProxy.frame.origin.x, y: self.dropdownTextProxy.frame.origin.y),
                size: size
            )
            self.dropdownTextProxy.frame = rect
            self.dropdownTextProxy.center.y -= (216 - self.normalDownButton.frame.height)
        }
        
        viewLabel.center.x -= view.center.x
        normalDownButton.center.x += view.bounds.width
        dropUpButton.center.x -= view.bounds.width
        
        viewLabel.text = ""
        dropdownLogic = true
    }
    
    @IBAction func keypressPopup(sender: UIButton!)
    {
        dispatch_async(dispatch_get_main_queue())
        {
            let buttonNorm = sender.frame
            let imagePop = sender.currentImage
            if sender.titleLabel!.text != "q" && sender.titleLabel!.text != "Q" && sender.titleLabel!.text != "1" && sender.titleLabel!.text != "-" && sender.titleLabel!.text != "[" && sender.titleLabel!.text != "_" && sender.titleLabel!.text != "A" && sender.titleLabel!.text != "a"
            {
                sender.frame = CGRectMake(buttonNorm.origin.x - self.popupX, buttonNorm.origin.y - self.popupY, buttonNorm.size.width + self.popupY, buttonNorm.size.height + self.popupY)
                sender.setImage(imagePop, forState: .Highlighted)
            }
        }
    }
    
    @IBAction func keypressPopupEnd(sender: UIButton!)
    {
        let buttonNorm = sender.frame
        if sender.titleLabel!.text != "q" && sender.titleLabel!.text != "Q" && sender.titleLabel!.text != "1" && sender.titleLabel!.text != "-" && sender.titleLabel!.text != "[" && sender.titleLabel!.text != "_" && sender.titleLabel!.text != "A" && sender.titleLabel!.text != "a"
        {
            sender.frame = CGRectMake(buttonNorm.origin.x + self.popupX, buttonNorm.origin.y + self.popupY, buttonNorm.size.width - self.popupY, buttonNorm.size.height - self.popupY)
        }
    }
    @IBAction func keypress(sender: UIButton!)
    {
        let buttonNorm = sender.frame
        if sender.titleLabel!.text != "q" && sender.titleLabel!.text != "Q" && sender.titleLabel!.text != "1" && sender.titleLabel!.text != "-" && sender.titleLabel!.text != "[" && sender.titleLabel!.text != "_" && sender.titleLabel!.text != "A" && sender.titleLabel!.text != "a"
        {
            dispatch_async(dispatch_get_main_queue())
            {
                sender.frame = CGRectMake(buttonNorm.origin.x + self.popupX, buttonNorm.origin.y + self.popupY, buttonNorm.size.width - self.popupY, buttonNorm.size.height - self.popupY)
            }
        }
        if dropdownLogic == false
        {
            let proxy = viewLabel
            let typedCharacter = sender.titleLabel?.text
            if proxy.text!.isEmpty == false && displayString == ""
            {
                proxy.text = ""
            }
            displayString += typedCharacter!
            memoryDisplayString.insertString(typedCharacter!, atIndex: globalMemoryIndex)
            globalMemoryIndex += 1
            let stringChars = findMaxUILabelCharacters(displayString)
            if stringChars.width >= (proxy.frame.size.width - 20) // making sure line doesn't get truncated
            {
                displayString = String(displayString.characters.dropFirst())
                displayString = String(displayString.characters.dropFirst())
                displayString = String(displayString.characters.dropFirst())
                displayString = String(displayString.characters.dropFirst())
                proxy.text = displayString
            }
            else
            {
                proxy.text = displayString
            }
            if (uppercaseLogic)
            {
                toLowercase()
            }
        }
        else
        {
            let typedCharacter = sender.titleLabel?.text
            dropdownTextProxy.insertText(typedCharacter!)
        }
    }
    
    @IBAction func spacePress()
    {
        if dropdownLogic == false
        {
            let proxy = viewLabel
            let typedCharacter = " "
            if proxy.text!.isEmpty == false && displayString == ""
            {
                proxy.text = ""
            }
            displayString += typedCharacter
            memoryDisplayString.insertString(typedCharacter, atIndex: globalMemoryIndex)
            globalMemoryIndex += 1
            let stringChars = findMaxUILabelCharacters(displayString)
            if stringChars.width >= (proxy.frame.size.width - 20) // making sure line doesn't get truncated
            {
                displayString = String(displayString.characters.dropFirst())
                displayString = String(displayString.characters.dropFirst())
                displayString = String(displayString.characters.dropFirst())
                displayString = String(displayString.characters.dropFirst())
                proxy.text = displayString
            }
            else
            {
                proxy.text = displayString
            }
        }
        else
        {
            let typedCharacter = " "
            dropdownTextProxy.insertText(typedCharacter)
        }
    }
    
    @IBAction func delete()
    {
        if dropdownLogic == false
        {
            var proxy = viewLabel
            if proxy.text!.isEmpty == false && displayString == ""
            {
                proxy.text = ""
            }
            if uppercaseLogic == true
            {
                toLowercase()
            }
            if displayString.isEmpty == false
            {
                proxy = viewLabel
                displayString.removeAtIndex(displayString.endIndex.predecessor())
                memoryDisplayString.deleteCharactersInRange(NSMakeRange(memoryDisplayString.length - 1, 1))
                globalMemoryIndex -= 1
                if memoryDisplayString.length > displayString.characters.count
                {
                    var indexOfAdditiveChar = (memoryDisplayString.length - displayString.characters.count)
                    indexOfAdditiveChar -= 1
                    let additiveChar = Character(memoryDisplayString.substringWithRange(NSRange(location: indexOfAdditiveChar, length: 1)))
                    displayString.insert(additiveChar, atIndex: displayString.startIndex)
                    proxy.text = displayString
                }
                else
                {
                    proxy.text = displayString
                }
            }
        }
        else
        {
            dropdownTextProxy.deleteBackward()
        }
    }
    
    @IBAction func deleteFastFunc()
    {
        deleteFast = NSTimer.scheduledTimerWithTimeInterval(0.09, target: self, selector:#selector(KeyboardViewController.deleteTwo), userInfo: nil, repeats: true)
    }
    
    func deleteTwo()
    {
        if dropdownLogic == false
        {
            var proxy = viewLabel
            if proxy.text!.isEmpty == false && displayString == ""
            {
                proxy.text = ""
            }
            if uppercaseLogic == true
            {
                toLowercase()
            }
            if displayString.isEmpty == false
            {
                proxy = viewLabel
                displayString.removeAtIndex(displayString.endIndex.predecessor())
                memoryDisplayString.deleteCharactersInRange(NSMakeRange(memoryDisplayString.length - 1, 1))
                globalMemoryIndex -= 1
                if memoryDisplayString.length > displayString.characters.count
                {
                    var indexOfAdditiveChar = (memoryDisplayString.length - displayString.characters.count)
                    indexOfAdditiveChar -= 1
                    let additiveChar = Character(memoryDisplayString.substringWithRange(NSRange(location: indexOfAdditiveChar, length: 1)))
                    displayString.insert(additiveChar, atIndex: displayString.startIndex)
                    proxy.text = displayString
                }
                else
                {
                    proxy.text = displayString
                }
            }
        }
        else
        {
            dropdownTextProxy.deleteBackward()
        }
    }
    
    @IBAction func deleteButtonUp()
    {
        deleteFast.invalidate()
    }
    
    func timerFunc()
    {
        timer = NSTimer.scheduledTimerWithTimeInterval(0.15, target: self, selector: Selector(counterFunc()), userInfo: nil, repeats: true)
    }
    
    func timerTwoFunc()
    {
        timerTwo = NSTimer.scheduledTimerWithTimeInterval(0.15, target: self, selector: Selector(counterFuncTwo()), userInfo: nil, repeats: true)
    }
    
    func counterFunc()
    {
        counter += 0.15
    }
    
    func counterFuncTwo()
    {
        counterTwo += 0.15
    }
    
    func findMaxUILabelCharacters(testString: String) -> CGRect
    {
        let label = viewLabel
        let sizeOfUILabel = CGSize(width: label.frame.size.width, height: label.frame.size.height)
        let labelFont = label.font
        let myAttribute = [ NSFontAttributeName: labelFont]
        let textSize = testString.boundingRectWithSize(sizeOfUILabel, options: NSStringDrawingOptions.UsesLineFragmentOrigin, attributes: myAttribute, context: nil)
        return textSize
    }
    
    func editKeyboardKeysHeight(height: CGFloat)
    {
        var allButtonArray:[UIButton] = [q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m, one, two, three, four, five, six, seven, eight, nine, zero, dash, forwardSlash, colon, semicolon, leftParen, rightParen, dollar, ampersand, at, quote, period, comma, question, exclamation, singleQuote, space, initialize123Button, initializeUpperButton, initializeDeleteButton, initializeNextKeyboard, initializeEncryptButton]
        viewLabel.center.y += height
        
        var index = allButtonArray.startIndex
        repeat
        {
            allButtonArray[index].center.y += height
            index += 1
        }
            while index < allButtonArray.endIndex
    }
    
    func animateKeyboard(buttonArray: [UIButton])
    {
        slideImage.center.x -= view.bounds.width
        
        var index = buttonArray.startIndex
        repeat
        {
            buttonArray[index].center.x -= view.bounds.width
            index += 1
        }
            while index < buttonArray.endIndex
        
        UIView.animateWithDuration(0.4, animations: {
            self.slideImage.center.x += self.view.bounds.width
            
            index = buttonArray.startIndex
            repeat
            {
                buttonArray[index].center.x += self.view.bounds.width
                index += 1
            }
                while index < 10
        })
        
        UIView.animateWithDuration(0.4, delay: 0.1, options: [], animations: {
            repeat
            {
                buttonArray[index].center.x += self.view.bounds.width
                index += 1
            }
                while index >= 10 && index < 19
            }, completion: nil)
        
        UIView.animateWithDuration(0.4, delay: 0.2, options: [], animations: {
            repeat
            {
                buttonArray[index].center.x += self.view.bounds.width
                index += 1
            }
                while index >= 19 && index < 28
            }, completion: nil)
        
        UIView.animateWithDuration(0.4, delay: 0.3, options: [], animations: {
            repeat
            {
                buttonArray[index].center.x += self.view.bounds.width
                index += 1
            }
                while index >= 28 && index < (buttonArray.endIndex - 1)
            }, completion: nil)
        
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