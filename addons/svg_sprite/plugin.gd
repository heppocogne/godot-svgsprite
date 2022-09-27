tool
extends EditorPlugin


func _enter_tree():
	add_custom_type("SVGSprite","Node2D",preload("res://addons/svg_sprite/svgsprite_library.gdns"),preload("res://addons/svg_sprite/icon_svgsprite.svg"))


func _exit_tree():
	remove_custom_type("SVGSprite")
