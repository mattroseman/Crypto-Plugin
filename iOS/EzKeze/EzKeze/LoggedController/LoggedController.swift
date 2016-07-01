//
//  LoggedController.swift
//  EzKeze
//
//  Created by Aron Gates on 6/30/16.
//  Copyright © 2016 Gates. All rights reserved.
//

import ChameleonFramework

class LoggedController: UIViewController
{
//    var firstColor = UIColor.flatWhiteColor()
//    var secondColor = UIColor.flatPowderBlueColor()
//    var gradient = UIGradientStyle.Radial
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
//        UIApplication.sharedApplication().delegate?.window??.backgroundColor = chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)
//        
//        view.backgroundColor = chamelonChangeColor(firstColor, secondColor: secondColor, gradientType: gradient)
    }
    
    func chamelonChangeColor(firstColor: UIColor, secondColor: UIColor, gradientType: UIGradientStyle) -> UIColor
    {
        let colors:[UIColor] = [
            firstColor,
            secondColor,
            ]
        return GradientColor(gradientType, frame: view.frame, colors: colors)
    }
    
}