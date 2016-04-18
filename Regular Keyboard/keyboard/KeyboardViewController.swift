//
//  KeyboardViewController.swift
//  keyboard
//
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
    var symbolsOneLogic:Bool = false
    var symbolsTwoLogic:Bool = false
    var error:Bool = false
    var lastString:String = ""
    var based64:String = ""
    
    var timer = NSTimer()
    var timerTwo = NSTimer()
    var deleteFast = NSTimer()
    
    let key: [UInt8] = [115, 98, 103, 111, 108, 121, 112, 105, 101, 115, 114, 100, 97, 105, 105, 97]
    let iv: [UInt8] = [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]
    
    var gorgeousString:String = ""
    let identifier = "{\"identifier\":\"MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM\",\"content\":"
    let identifierTwo = "}"
    
    var counter = 1.0
    var counterTwo = 1.0
    
    @IBOutlet var lowercaseField: UILabel!
    @IBOutlet var uppercaseField: UILabel!
    @IBOutlet var doublecaseField: UILabel!
    @IBOutlet var symbolsOneField: UILabel!
    @IBOutlet var symbolsTwoField: UILabel!
    
    override func updateViewConstraints()
    {
        super.updateViewConstraints()        
        
        // Add custom view sizing constraints here
        
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        print("test")
        loadKeyboard()
    }
    
    @IBAction func encryptFunc()
    {
        let proxyField = findProxy()
        let proxy = textDocumentProxy as UITextDocumentProxy
        let input = [UInt8](gorgeousString.utf8)
        let encrypted: [UInt8] = try! AES(key: key, blockMode: .CTR).encrypt(input, padding: nil)
        let encryptedString = encrypted.description
        //proxyField.text = "Encryption successful!"
        
        UIPasteboard.generalPasteboard().string = (identifier + encryptedString + identifierTwo)
        let temp = UIPasteboard.generalPasteboard().string
        
        let base64String = try! temp!.encrypt(AES(key: "secret0key000000", iv: "0123456789012345")).toBase64()
        proxy.insertText(base64String!)
        
        based64 = base64String!
        UIPasteboard.generalPasteboard().string = based64
        
        proxyField.text = ""
        gorgeousString = "" 
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
        error = false
        let proxyField = findProxy()
        let proxyReal = textDocumentProxy as UITextDocumentProxy
        
        

        
        if UIPasteboard.generalPasteboard().string == " "
        {
            error = true
            proxyField.text = lastString
        }
        while !error
        {
            let clipboard = UIPasteboard.generalPasteboard().string
            let changingString:String = clipboard!
            let decryptedFirst = try! changingString.decryptBase64ToString(AES(key: "secret0key000000", iv: "0123456789012345"))

            let input: [UInt8] = try! turnDescriptionIntoJson(decryptedFirst)
            let decrypted: [UInt8] = try! AES(key: key, blockMode: .CTR).decrypt(input, padding: nil)
            let decryptedData: NSData = NSData(bytes: decrypted)
            let decryptedString = NSString(data: decryptedData, encoding: NSUTF8StringEncoding) as? String
            //proxyField.text = "Decryption successful!"
            lastString = decryptedString!
            UIPasteboard.generalPasteboard().string = " "
            error = true
        }
    }
    
    @IBAction func nextKeyboard()
    {
        advanceToNextInputMode()
    }
    
    @IBAction func toLowercase()
    {
        lowercaseLogic = true
        uppercaseLogic = false
        doublecaseLogic = false
        symbolsOneLogic = false
        symbolsTwoLogic = false
        keyboardNib = UINib(nibName: "Small alphabet", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = keyboardView.backgroundColor
        view.addSubview(keyboardView)
        lowercaseField.text = gorgeousString
    }
    
    @IBAction func toUppercase()
    {
        lowercaseLogic = false
        uppercaseLogic = true
        doublecaseLogic = false
        symbolsOneLogic = false
        symbolsTwoLogic = false
        keyboardNib = UINib(nibName: "Big alphabet", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = keyboardView.backgroundColor
        view.addSubview(keyboardView)
        uppercaseField.text = gorgeousString
    }
    
    @IBAction func toDoubleUppercase()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = true
        symbolsOneLogic = false
        symbolsTwoLogic = false
        keyboardNib = UINib(nibName: "Doublecase", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = keyboardView.backgroundColor
        view.addSubview(keyboardView)
        dispatch_async(dispatch_get_main_queue())
        {
            self.doublecaseField.text = self.gorgeousString
        }
    }
    
    @IBAction func toNumbersOne()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = false
        symbolsOneLogic = true
        symbolsTwoLogic = false
        keyboardNib = UINib(nibName: "Numbers & punc1", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = keyboardView.backgroundColor
        view.addSubview(keyboardView)
        symbolsOneField.text = gorgeousString
    }
    
    @IBAction func toNumbersTwo()
    {
        lowercaseLogic = false
        uppercaseLogic = false
        doublecaseLogic = false
        symbolsOneLogic = false
        symbolsTwoLogic = true
        keyboardNib = UINib(nibName: "Numbers & punc2", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = keyboardView.backgroundColor
        view.addSubview(keyboardView)
        symbolsTwoField.text = gorgeousString
    }
    
    @IBAction func keypress(sender: UIButton!)
    {
        let proxy = findProxy()
        let typedCharacter = sender.titleLabel?.text
        gorgeousString += typedCharacter!
        proxy.text = gorgeousString
        if (uppercaseLogic)
        {
            toLowercase()
        }
    }
    
    @IBAction func spacePress()
    {
        let proxy = findProxy()
        let typedCharacter = " "
        gorgeousString += typedCharacter
        proxy.text = gorgeousString
    }
    
    @IBAction func delete()
    {
        if uppercaseLogic == true
        {
            toLowercase()
        }
        if gorgeousString.isEmpty == false
        {
            let proxy = findProxy()
            gorgeousString.removeAtIndex(gorgeousString.endIndex.predecessor())
            proxy.text = gorgeousString
        }
    }
    
    @IBAction func deleteFastFunc()
    {
        deleteFast = NSTimer.scheduledTimerWithTimeInterval(0.09, target: self, selector:#selector(KeyboardViewController.deleteTwo), userInfo: nil, repeats: true)
    }
    
    func deleteTwo()
    {
        if uppercaseLogic == true
        {
            toLowercase()
        }
        if gorgeousString.isEmpty == false
        {
            let proxy = findProxy()
            gorgeousString.removeAtIndex(gorgeousString.endIndex.predecessor())
            proxy.text = gorgeousString
        }
    }
    
    @IBAction func deleteButtonUp()
    {
        deleteFast.invalidate()
    }
    
    func loadKeyboard()
    {
        lowercaseLogic = true
        let keyboardNib = UINib(nibName: "Small alphabet", bundle: nil)
        keyboardView = keyboardNib.instantiateWithOwner(self, options: nil)[0] as! UIView
        view.backgroundColor = keyboardView.backgroundColor
        view.addSubview(keyboardView)
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
        else if symbolsOneLogic == true
        {
            proxy = symbolsOneField
        }
        else if symbolsTwoLogic == true
        {
            proxy = symbolsTwoField
        }
        return proxy
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated
    }

    override func textWillChange(textInput: UITextInput?)
    {
        // The app is about to change the document's contents. Perform any preparation here.
    }

    override func textDidChange(textInput: UITextInput?)
    {
        // The app has just changed the document's contents, the document context has been updated.
    
    }

}
