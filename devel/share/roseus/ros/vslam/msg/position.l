;; Auto-generated. Do not edit!


(when (boundp 'vslam::position)
  (if (not (find-package "VSLAM"))
    (make-package "VSLAM"))
  (shadow 'position (find-package "VSLAM")))
(unless (find-package "VSLAM::POSITION")
  (make-package "VSLAM::POSITION"))

(in-package "ROS")
;;//! \htmlinclude position.msg.html
(if (not (find-package "GEOMETRY_MSGS"))
  (ros::roseus-add-msgs "geometry_msgs"))


(defclass vslam::position
  :super ros::object
  :slots (_angle _p ))

(defmethod vslam::position
  (:init
   (&key
    ((:angle __angle) 0.0)
    ((:p __p) (instance geometry_msgs::Point :init))
    )
   (send-super :init)
   (setq _angle (float __angle))
   (setq _p __p)
   self)
  (:angle
   (&optional __angle)
   (if __angle (setq _angle __angle)) _angle)
  (:p
   (&rest __p)
   (if (keywordp (car __p))
       (send* _p __p)
     (progn
       (if __p (setq _p (car __p)))
       _p)))
  (:serialization-length
   ()
   (+
    ;; float32 _angle
    4
    ;; geometry_msgs/Point _p
    (send _p :serialization-length)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _angle
       (sys::poke _angle (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; geometry_msgs/Point _p
       (send _p :serialize s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _angle
     (setq _angle (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; geometry_msgs/Point _p
     (send _p :deserialize buf ptr-) (incf ptr- (send _p :serialization-length))
   ;;
   self)
  )

(setf (get vslam::position :md5sum-) "b61d066c73d062e24b69a478e96849ce")
(setf (get vslam::position :datatype-) "vslam/position")
(setf (get vslam::position :definition-)
      "float32 angle
geometry_msgs/Point p

#是注释
#使用geometry_msgs 需要在find_package和generate_messages添加geometry_msgs

================================================================================
MSG: geometry_msgs/Point
# This contains the position of a point in free space
float64 x
float64 y
float64 z

")



(provide :vslam/position "b61d066c73d062e24b69a478e96849ce")


