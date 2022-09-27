extends Node2D

onready var svg_sprite:SVGSprite=$SVGSprite
onready var svg_sprite2:SVGSprite=$SVGSprite2


func _ready():
	pass


func _process(delta:float):
	rotation+=delta
	svg_sprite.rotation-=2.0*delta
	svg_sprite2.scale=(2.0+sin(rotation))*Vector2(0.25,0.25)
