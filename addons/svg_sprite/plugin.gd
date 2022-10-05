tool
extends EditorPlugin


func _enter_tree():
	add_custom_type("SVGSprite","Node2D",preload("res://addons/svg_sprite/svgsprite_library.gdns"),preload("res://addons/svg_sprite/icon_svgsprite.svg"))
	add_custom_type("SVGFile","Reference",preload("res://addons/svg_sprite/svg_file_library.gdns"),preload("res://addons/svg_sprite/icon_svgsprite.svg"))


func _exit_tree():
	remove_custom_type("SVGSprite")
	remove_custom_type("SVGFile")
