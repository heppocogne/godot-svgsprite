tool
extends EditorPlugin

const _rawsvg_root:="res://_rawsvg"
var res_file_sys:EditorFileSystem
var file_sys:FileSystemDock


func _enter_tree():
	if Engine.editor_hint:
		res_file_sys=get_editor_interface().get_resource_filesystem()
		res_file_sys.connect("resources_reimported",self,"_on_resources_reimported")
		file_sys=get_editor_interface().get_file_system_dock()
		file_sys.connect("file_removed",self,"_on_file_removed")
		file_sys.connect("files_moved",self,"_on_files_moved")
		
		var dir:=Directory.new()
		if !dir.dir_exists(_rawsvg_root):
			dir.make_dir(_rawsvg_root)
	
	add_custom_type("SVGSprite","Node2D",preload("res://addons/svg_sprite/svgsprite_library.gdns"),preload("res://addons/svg_sprite/icon_svgsprite.svg"))


func _exit_tree():
	remove_custom_type("SVGSprite")
	
	if Engine.editor_hint:
		res_file_sys.disconnect("resources_reimported",self,"_on_resources_reimported")
		file_sys.disconnect("file_removed",self,"_on_file_removed")
		file_sys.disconnect("files_moved",self,"_on_files_moved")


func get_rawsvg_path(path:String)->String:
	return _rawsvg_root.plus_file(path.get_file()+"-"+path.get_basename().sha256_text()+".rawsvg")


func _on_resources_reimported(paths:PoolStringArray):
	for path in paths:
		if path.ends_with(".svg"):
			var d:=Directory.new()
			d.copy(path,get_rawsvg_path(path))


func _on_file_removed(path:String):
	if path.ends_with(".svg"):
		var d:=Directory.new()
		d.remove(get_rawsvg_path(path))


func _on_files_moved(old_path:String,new_path:String):
	if old_path.ends_with(".svg"):
		var d:=Directory.new()
		d.rename(get_rawsvg_path(old_path),get_rawsvg_path(new_path))
