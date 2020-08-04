
(cl:in-package :asdf)

(defsystem "stroll_bearnav-srv"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "SetDistance" :depends-on ("_package_SetDistance"))
    (:file "_package_SetDistance" :depends-on ("_package"))
  ))