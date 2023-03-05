extends Node2D

@onready var svg_sprite:SVGSprite=$SVGSprite
@onready var svg_sprite2:SVGSprite=$SVGSprite2
@onready var svg_sprite3:SVGSprite=$SVGSprite3


func _ready():
	pass


func _process(delta:float):
	rotation+=delta
	svg_sprite.rotation-=2.0*delta
	svg_sprite2.scale=(2.0+sin(rotation))*Vector2(0.25,0.25)
	svg_sprite3.rotation+=2.0*delta
