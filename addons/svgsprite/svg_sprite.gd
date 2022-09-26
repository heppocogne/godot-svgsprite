extends Node2D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var p:Node2D=get_parent()
	p.rotation_degrees+=1.0
#	p.scale+=0.01*Vector2(1,1)
