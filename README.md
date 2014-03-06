Keyboard dynamics for intrusion detection
=========================================
The primary goal of this project is to create a system to model a user's keyboard habits which can be used to detect potential intruders.  The program should actively monitor the user‘s keystrokes and create useful statistics which can then be analysed to help detect intrusion. 

Initial goals of the project include:
Create a program which will prompt the user to enter their password into a terminal window n times.
Use a SystemTap script to monitor system calls and keystrokes which is then outputted to a text file.
Analyze this output within the program and perform various statistical operations on it to create a user profile.
Use this statistical information to determine if subsequent attempts to enter the password belong to the original user.

Further goals of the project include:
Integrate this method of detection into the Linux authentication system.
Create a GUI to allow the user to quickly set up their profile.
Encrypt the password data pulled from the SystemTap script to help prevent potential security threats. 
Create a secure system in which the user can bypass the detection in the case that they cannot replicate their own keystrokes.

The further goals will be planned and implemented once the initial system is in place and satisfactory/stable.  It is expected that most of the extended goals should be reached.  However, it should be noted that these are additions to the system and are not part of the core requirements defined above.  


Programming Languages
---------------------
The keyboard tracking will be done by a scripting language called Systemtap which should create an output file that can be further analysed.  The system will primarily be designed in C/C++.  



Programming Tools
-----------------
g++/gcc for (C/C++ compiling)
Github (Collaboration, Version Control and Sharing)


Learning challenges
-  Systemtap scripting language.
-  Statistical analysis.
-  OS specific C programming
-  Version Control and online collaboration

Hardware/Software Platform
--------------------------
The only required platform for this project is the Fedora OS which we are targeting our program at.  

