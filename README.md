# PASTILDA: open-source password manager

DESCRIPTION

It is an open-source hardware designed to manage our credentials in handy and secure way.  It provides easy and safe auto-login to bank accounts, mailboxes, corporate network or social media.

So if you also have to daily remember and enter a number of strong passwords — assign it to Pastilda.

Pastilda never reveals master key to the host. Decrypted data stays onboard, unreachable for malware. 

Requires no drivers or client software. Pastilda impersonates a common keyboard, so it works with most systems by default.

Summon a control menu to any text field.

Applicable for command line interfaces. 

COMPLETE OPENNESS

Publishes under GNU GPL license. Feel free to use both software and hardware in your own projects.


HOW DOES IT WORK

Pastilda has two USB slots: one to connect keyboard, one to connect it to your PC. So, your OS recognize Pastilda as composite device: keyboard and flash drive. And your real keyboard now is visible just for Pastilda, your PC doesn't see it at all.

On flash drive you have to store KeePass 2.x portable and encrypted database (.kdbx file).

In a regular mode, everyting you type on you keyboard translates through Pastilda to your PC.

If you need to sign in to social media for example, you need to switch to "Pastilda mode". To do that you have to set your mouse cursor inside a login textbox and then type combination Ctrl + ~. After that, Pastilda will ask you to enter password from your KeePass database. 

If password is correct Pastilda decrypts your database and you can start to navigate trough it with arrows left, right, up and down. As soon as you find the login you were looking for, you should type Enter and after that, Pastilda automaticly substitutes the corresponding password in password textbox and enters you account.

If password is incorrect Pastilda will offer you to enter password again. So, you can either continue to enter password or go back to regular mode by pressing Esc key on your keyboard.

WHAT IS DONE FOR NOW

We have finished prototyping and released version 0.1. We have implemented just the basic functionality by now, so there are few limitations:

1. Pastilda can't handle media keyboards.
2. You can load just up to 30Kb database.
3. When you load KeePass 2.x database to the flash drive, you have to change extension from .kdbx to .kdb.
4. You have to disable GZip compression in KeePass 2.x application.

HOW TO BUILD

We are using Eclipse + GNU ARM Eclipse + OpenOCD. You can read about it here: http://gnuarmeclipse.github.io.

When you fork or clone the repository, you have to add two submodules:

1. libopencm3 to folder /lib (use this fork: https://github.com/thirdpin/libopencm3).
2. libopencm3_cpp_extensions to folder /emb/pastilda/lib (available here: https://github.com/thirdpin/libopencm3_cpp_extensions).

CONTACTS

If you have any questions, please feel free to contact us:
Anastasiia Lazareva: a.lazareva@thirdpin.ru
Сorporate e-mail: info@ThirdPin.ru



