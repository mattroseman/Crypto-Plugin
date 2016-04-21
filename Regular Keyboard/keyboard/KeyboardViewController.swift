//
//  KeyboardViewController.swift
//  keyboard
//  YOU CAN FIT 25 M's ON THE KEYBOARD SCREEN
//  Created by Aron Gates on 4/14/16.
//  Copyright © 2016 Gates. All rights reserved.
//

import UIKit
import CryptoSwift

class KeyboardViewController: UIInputViewController
{
    var keyboardView: UIView!
    var keyboardNib: UINib!
    
    var lowercaseLogic:Bool = false
    var uppercaseLogic:Bool = false
    var doublecaseLogic:Bool = false
    var numbersOneLogic:Bool = false
    var numbersTwoLogic:Bool = false
    var dropDownNumbersLogic:Bool = false
    var dropDownLettersLogic:Bool = false
    var trackError:Bool = false
    var lastString:String = ""
    var based64:String = ""
    var buttonNorm = CGRect(origin: CGPoint(x: 0, y: 0), size: UIScreen.mainScreen().bounds.size)
    
    var timer = NSTimer()
    var timerTwo = NSTimer()
    var deleteFast = NSTimer()
    
    let key: [UInt8] = [115, 98, 103, 111, 108, 121, 112, 105, 101, 115, 114, 100, 97, 105, 105, 97]
    let iv: [UInt8] = [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]
    
    var displayString:String = ""
    var memoryDisplayString:NSMutableString = ""
    
    let identifier = "{\"identifier\":\"MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM\",\"content\":"
    let identifierTwo = "}"
    
    var counter = 1.0
    var counterTwo = 1.0
    var globalMemoryIndex = 0
    

    @IBOutlet var lowercaseField: UILabel!
    @IBOutlet var uppercaseField: UILabel!
    @IBOutlet var doublecaseField: UILabel!
    @IBOutlet var numbersOneField: UILabel!
    @IBOutlet var numbersTwoField: UILabel!
    @IBOutlet var dropdownNumbers: UITextView!
    @IBOutlet var dropdownLetters: UITextView!
    
    override func updateViewConstraints()
    {
        super.updateViewConstraints()        
        
        // Add custom view sizing constraints here
        
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        loadKeyboard()
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated
    }
    
    @IBAction func encryptFunc()
    {
        while displayString != ""
        {
            let proxyField = findProxy()
            let proxy = textDocumentProxy as UITextDocumentProxy
            let tempString = String(memoryDisplayString)
            let input = [UInt8](tempString.utf8)
            let encrypted: [UInt8] = try! AES(key: key, blockMode: .CTR).encrypt(input, padding: nil)
            let encryptedString = encrypted.description
        
            UIPasteboard.generalPasteboard().string = (identifier + encryptedString + identifierTwo)
            let temp = UIPasteboard.generalPasteboard().string
            
            let base64String = try! temp!.encrypt(AES(key: "secret0key000000", iv: "0123456789012345")).toBase64()
            proxy.insertText(base64String!)
            
            based64 = base64String!
            UIPasteboard.generalPasteboard().string = based64
            
            proxyField.text = ""
            displayString = ""
            memoryDisplayString = ""
            globalMemoryIndex = 0
        }
    }
    
    func turnDescriptionIntoJson(descriptionInput: String) -> [UInt8]
    {
        var replaced = descriptionInput.stringByReplacingOccurrencesOfString(identifier, withString: "")
        replaced = replaced.stringByReplacingOccurrencesOfString(identifierTwo, withString: "")
        replaced = replaced.stringByReplacingOccurrencesOfString(" ", withString: "")
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
        trackError = false
        let proxyField = findProxy()
        if UIPasteboard.generalPasteboard().string == " "
        {
            trackError = true
            proxyField.text = lastString
        }
        while !trackError
        {
            let clipboard = UIPasteboard.generalPasteboard().string
            let changingString:String = clipboard!
            var decryptedFirst:String
            do
            {
                decryptedFirst = try changingString.decryptBase64ToString(AES(key: "secret0key000000", iv: "0123456789012345"))
            }
            catch
            {
                lastString = " "
                trackError = true
                break
            }
            decryptedFirst = try! changingString.decryptBase64ToString(AES(key: "secret0key000000", iv: "0123456789012345"))

            let input: [UInt8] = turnDescriptionIntoJson(decryptedFirst)
            let decrypted: [UInt8] = try! AES(key: key, blockMode: .CTR).decrypt(input, padding: nil)
            let decryptedData: NSData = NSData(bytes: decrypted)
            let decryptedString = NSString(data: decryptedData, encoding: NSUTF8StringEncoding) as? String
            //proxyField.text = "Decryption successful!"
            lastString = decryptedString!
            UIPasteboard.generalPasteboard().string = " "
            trackError = true
        }
    }
    
    @IBAction func nextKeyboard()
    {
        advanceToNextInputMode()
    }
    
    func loadKeyboard()
    {
        lowercaseLogic = true
        let keyboardNib = UINib(nibName: "Lowercase", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = UIColor.blueColor().colorWithAlphaComponent(0.0)
        view.addSubview(keyboardView)
        lowercaseField.text = ""
    }
    
    @IBAction func toLowercase()
    {
        lowercaseLogic = true
        uppercaseLogic = false
        doublecaseLogic = false
        numbersOneLogic = false
        numbersTwoLogic = false
        dropDownNumbersLogic = false
        dropDownLettersLogic = false
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "Lowercase", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        lowercaseField.text = displayString
    }
    
    @IBAction func toUppercase()
    {
        lowercaseLogic = false
        uppercaseLogic = true
        doublecaseLogic = false
        numbersOneLogic = false
        numbersTwoLogic = false
        dropDownNumbersLogic = false
        dropDownLettersLogic = false
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "Uppercase", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        uppercaseField.text = displayString
    }
    
    @IBAction func toDoublecase()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = true
        numbersOneLogic = false
        numbersTwoLogic = false
        dropDownNumbersLogic = false
        dropDownLettersLogic = false
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "Doublecase", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        dispatch_async(dispatch_get_main_queue())
        {
            self.doublecaseField.text = self.displayString
        }
    }
    
    @IBAction func toNumbersOne()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = false
        numbersOneLogic = true
        numbersTwoLogic = false
        dropDownNumbersLogic = false
        dropDownLettersLogic = false
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "Numbers1", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        numbersOneField.text = displayString
    }
    
    @IBAction func toNumbersTwo()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = false
        numbersOneLogic = false
        numbersTwoLogic = true
        dropDownNumbersLogic = false
        dropDownLettersLogic = false
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "Numbers2", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        numbersTwoField.text = displayString
    }
    
    @IBAction func toDropDown()
    {
        let proxy = findProxy()
        proxy.text = ""
        whichDropDown()
    }
    
    func toDropDownLetters()
    {
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "DropdownLetters", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        dropdownLetters.text = memoryDisplayString.description
    }
    
    func toDropDownNumbers()
    {
        for view in keyboardView.subviews
        {
            view.removeFromSuperview()
        }
        keyboardNib = UINib(nibName: "DropdownNumbers", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.addSubview(keyboardView)
        dropdownNumbers.text = memoryDisplayString.description
    }
    
    func whichDropDown()
    {
        if lowercaseLogic || uppercaseLogic || doublecaseLogic
        {
            toDropDownLetters()
            dropDownLettersLogic = true
        }
        else if numbersOneLogic || numbersTwoLogic
        {
            toDropDownNumbers()
            dropDownNumbersLogic = true
        }
    }
    
    @IBAction func returnToWhichView()
    {
        if lowercaseLogic
        {
            toLowercase()
        }
        else if uppercaseLogic
        {
            toUppercase()
        }
        else if doublecaseLogic
        {
            toDoublecase()
        }
        else if numbersOneLogic
        {
            toNumbersOne()
        }
        else if numbersTwoLogic
        {
            toNumbersTwo()
        }
    }
    
    @IBAction func justForQPop(sender: UIButton!)
    {
        dispatch_async(dispatch_get_main_queue())
        {
            self.buttonNorm = sender.frame
            let imagePop = sender.currentImage
            sender.frame = CGRectMake(self.buttonNorm.origin.x, self.buttonNorm.origin.y, self.buttonNorm.size.width + 3, self.buttonNorm.size.height + 3)
            sender.setImage(imagePop, forState: .Highlighted)
        }
    }
    
    @IBAction func keypressPopup(sender: UIButton!)
    {
        dispatch_async(dispatch_get_main_queue())
        {
            self.buttonNorm = sender.frame
            let imagePop = sender.currentImage
            sender.frame = CGRectMake(self.buttonNorm.origin.x - 2.8, self.buttonNorm.origin.y - 5.6, self.buttonNorm.size.width + 5.6, self.buttonNorm.size.height + 5.6)
            sender.setImage(imagePop, forState: .Highlighted)
        }
    }
    
    @IBAction func keypressPopupEnd(sender: UIButton!)
    {
        sender.frame = CGRectMake(self.buttonNorm.origin.x, self.buttonNorm.origin.y, self.buttonNorm.size.width, self.buttonNorm.size.height)
    }
    @IBAction func keypress(sender: UIButton!)
    {
        dispatch_async(dispatch_get_main_queue())
        {
            sender.frame = CGRectMake(self.buttonNorm.origin.x, self.buttonNorm.origin.y, self.buttonNorm.size.width, self.buttonNorm.size.height)
        }
        let proxy = findProxy()
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
    
    @IBAction func spacePress()
    {
        let proxy = findProxy()
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
    
    @IBAction func delete()
    {
        var proxy = findProxy()
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
            proxy = findProxy()
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
    
    @IBAction func deleteFastFunc()
    {
        deleteFast = NSTimer.scheduledTimerWithTimeInterval(0.09, target: self, selector:#selector(KeyboardViewController.deleteTwo), userInfo: nil, repeats: true)
    }
    
    func deleteTwo()
    {
        var proxy = findProxy()
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
            proxy = findProxy()
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
    
    func findProxy() -> UILabel
    {
        var proxy = UILabel()
        if lowercaseLogic == true
        {
            proxy = lowercaseField
        }
        else if uppercaseLogic == true
        {
            proxy = uppercaseField
        }
        else if doublecaseLogic == true
        {
            proxy = doublecaseField
        }
        else if numbersOneLogic == true
        {
            proxy = numbersOneField
        }
        else if numbersTwoLogic == true
        {
            proxy = numbersTwoField
        }
        return proxy
    }
    
    func findMaxUILabelCharacters(testString: String) -> CGRect
    {
        let label = findProxy()
        let sizeOfUILabel = CGSize(width: label.frame.size.width, height: label.frame.size.height)
        let labelFont = label.font
        let myAttribute = [ NSFontAttributeName: labelFont]
        let textSize = testString.boundingRectWithSize(sizeOfUILabel, options: NSStringDrawingOptions.UsesLineFragmentOrigin, attributes: myAttribute, context: nil)
        return textSize
    }

}

extension NSMutableString
{
    subscript (i: Int) -> Character
    {
        return (self[i] as Character)
    }
}