//
//  keychainAccessibility.swift
//  EzKeze
//
//  Created by Aron Gates on 6/29/16.
//  Copyright © 2016 Gates. All rights reserved.
//

import Locksmith

class Keychain
{
    struct EzKz:
    ReadableSecureStorable,
    CreateableSecureStorable,
    DeleteableSecureStorable,
    GenericPasswordSecureStorable
    {
        private let username: String
        private let password: String
        
        let service = "EzKz"
        var account: String { return username }
        var data: [String: AnyObject]
        {
            return ["password": password]
        }
    }
    
    func saveNewKey(newUser: String, newPass: String)
    {
        let account = EzKz(username: newUser, password: newPass)
        do{
            _ = try account.createInSecureStore()
        } catch{
            
        }
    }
    
    func keyExists(username: String, password: String) -> Bool
    {
        let account = EzKz(username: username, password: password)
        let result = account.readFromSecureStore()
        
        if result != nil
        {
            return true
        }
        return false
    }
    
    private func getPassword(id: EzKz) -> String
    {
        let result = id.readFromSecureStore()
        if result != nil
        {
            var temp = String(result!.data!["password"])
            temp = temp.stringByReplacingOccurrencesOfString("Optional(", withString: "")
            temp = String(temp.characters.dropLast())
            return temp
        }
        return ""
    }
    
}
